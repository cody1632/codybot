#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

const char *codybot_version_string = "0.1.0";

static const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'V'},
	{"blinkenshell", no_argument, NULL, 'b'},
	{"freenode", no_argument, NULL, 'f'},
	{NULL, 0, NULL, 0}
};
static const char *short_options = "hVbf";

int fd, ret, endmainloop;
char *buffer, *buffer_rx, *buffer_cmd, *server_ip;
struct timeval tv0;
struct tm *tm0;
time_t t0;

// sao.blinkenshell.org
char *server_ip_blinkenshell = "194.14.45.5";

// medusa.blinkenshell.org
//char *server_ip_blinkenshell = "69.164.197.11";

// livingstone.freenode.net
char *server_ip_freenode = "107.182.226.199";

SSL *pSSL;

void HelpShow(void) {
	printf("Usage: codybot { -h/--help | -V/--version | -b/--blinkenshell | -f/--freenode }\n");
}

// raw should hold something like ":esselfe!~bsfc@unaffiliated/esselfe PRIVMSG #esselfe :!fortune"
char *raw_to_word(char *raw) {
	char *c = raw;
	unsigned int cnt = 0;
	while (1) {
		++c;
		++cnt;
		if (*c == ':')
			break;
		else if (*c == '\0')
			break;
	}
	char *word = (char *)malloc(1024);
	memset(word, 0, 1024);
	cnt = 0;
	if (*c != '\0') {
		++c;
		while (1) {
			word[cnt] = *(c+cnt);
			if (*(c+cnt+1) == ' ' || *(c+cnt+1) == '\0')
				break;
			++cnt;
		}
	}

	return word;
}

void fortune(void) {
	FILE *fp = fopen("linux.fortune", "r");
	if (fp == NULL) {
		fprintf(stderr, "codybot error: Cannot open linux.fortune: %s\n", strerror(errno));
		if (server_ip == server_ip_blinkenshell)
			sprintf(buffer_cmd, "privmsg #blinkenshell :fortune error: cannot open database\n");
		else
			sprintf(buffer_cmd, "privmsg ##linux-offtopic :fortune error: cannot open database\n");
		SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
		return;
	}

	fseek(fp, 0, SEEK_END);
	unsigned long filesize = (unsigned long)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	srand((unsigned int)time(NULL));
	unsigned int position = rand()%(filesize-500);
	fseek(fp, position, SEEK_CUR);

	int c = 0, cprev, cnt = 0;
	char fortune_line[4096];
	memset(fortune_line, 0, 4096);
	while (1) {
		cprev = c;
		c = fgetc(fp);
		if (c == -1) {
			c = ' ';
			break;
		}
		fputc(c, stdout);
		if (c == '%' && cprev == '\n') {
			fgetc(fp);
			c = ' ';
			break;
		}
	}

	while (1) {
		cprev = c;
		c = fgetc(fp);
		if (c == -1)
			break;
		else if (c == '\t' && cprev == '\n')
			break;
		else if (c == '%' && cprev == '\n')
			break;
		else if (c == '\n' && cprev == '\n')
			fortune_line[cnt++] = ' ';
		else if (c == '\n' && cprev != '\n')
			fortune_line[cnt++] = ' ';
		else
			fortune_line[cnt++] = c;
	}

	if (strlen(fortune_line) > 0) {
		if (server_ip == server_ip_freenode)
//			sprintf(buffer_cmd, "privmsg ##linux-offtopic :fortune: %s\n", fortune_line);
			sprintf(buffer_cmd, "privmsg #codybot :fortune: %s\n", fortune_line);
		else
			sprintf(buffer_cmd, "privmsg #blinkenshell :fortune: %s\n", fortune_line);
		SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
	}

	fclose(fp);
}

void *ThreadFunc(void *argp) {
	while (!endmainloop) {
		memset(buffer_rx, 0, 1024);
		SSL_read(pSSL, buffer_rx, 1023);
		if (strlen(buffer_rx) == 0) {
			endmainloop = 1;
			break;
		}
		gettimeofday(&tv0, NULL);
		t0 = (time_t)tv0.tv_sec;
		tm0 = gmtime(&t0);
		buffer_rx[strlen(buffer_rx)-2] = '\0';
		printf("%02d:%02d:%02d.%03ld <<%s>>\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
			tv0.tv_usec, buffer_rx);
		// respond to ping request from the server
		if (buffer_rx[0] == 'P' && buffer_rx[1] == 'I' && buffer_rx[2] == 'N' &&
			buffer_rx[3] == 'G' && buffer_rx[4] == ' ' && buffer_rx[5] == ':') {
			if (server_ip == server_ip_blinkenshell) {
				sprintf(buffer_cmd, "%s\n", buffer_rx);
				buffer_cmd[1] = 'O';
				SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
				gettimeofday(&tv0, NULL);
				t0 = (time_t)tv0.tv_sec;
				tm0 = gmtime(&t0);
				buffer_cmd[strlen(buffer_cmd)-2] = '\0';
				printf("%02d:%02d:%02d.%03ld <<%s>>\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
					tv0.tv_usec, buffer_cmd);
			}
			else {
				SSL_write(pSSL, "PONG\n", 5);
				gettimeofday(&tv0, NULL);
				t0 = (time_t)tv0.tv_sec;
				tm0 = gmtime(&t0);
				printf("%02d:%02d:%02d.%03ld ##PONG sent##\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
					tv0.tv_usec);
			}
		}
		if (strcmp(raw_to_word(buffer_rx), "!fortune")==0)
			fortune();
		else if (strcmp(raw_to_word(buffer_rx), "!codybot_version")==0) {
			if (server_ip == server_ip_blinkenshell)
				sprintf(buffer_cmd, "privmsg #blinkenshell :codybot %s\n", codybot_version_string);
			else
				//sprintf(buffer_cmd, "privmsg ##linux-offtopic :codybot %s\n", codybot_version_string);
				sprintf(buffer_cmd, "privmsg #codybot :codybot %s\n", codybot_version_string);
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
		}

		usleep(10000);
	}
	return NULL;
}

void ReadCommandLoop(void) {
	while (!endmainloop) {
		memset(buffer_cmd, 0, 1024);
		fgets(buffer_cmd, 1024, stdin);
		if (buffer_cmd[0] == '\n')
			continue;
		else if (strcmp(buffer_cmd, "exit")==0)
			endmainloop = 1;
		else {
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
			gettimeofday(&tv0, NULL);
			t0 = (time_t)tv0.tv_sec;
			tm0 = gmtime(&t0);
			buffer_cmd[strlen(buffer_cmd)-2] = '\0';
			printf("%02d:%02d:%02d.%03ld ##%s##\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
				tv0.tv_usec, buffer_cmd);
		}
	}
}

void ConnectClient(void) {
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		fprintf(stderr, "codybot error: Cannot socket(): %s\n", strerror(errno));
		exit(1);
	}
	else
		printf("socket fd: %d\n", fd);
	
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	if (server_ip == server_ip_blinkenshell)
		addr.sin_port = htons(16423);
	else
		addr.sin_port = htons(16424);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "codybot error: Cannot bind(): %s\n", strerror(errno));
		exit(1);
	}
	else
		printf("bind() 0.0.0.0 successful\n");
	
	struct sockaddr_in host;
	host.sin_addr.s_addr = inet_addr(server_ip);
	host.sin_family = AF_INET;
	host.sin_port = htons(6697);
	if (connect(fd, (struct sockaddr *)&host, sizeof(host)) < 0) {
		fprintf(stderr, "codybot error: Cannot connect(): %s\n", strerror(errno));
		exit(1);
	}
	else
		printf("connect() %s successful\n", server_ip);

	SSL_load_error_strings();
	SSL_library_init();
	OpenSSL_add_all_algorithms();

	const SSL_METHOD *method = TLS_method();
	SSL_CTX *ctx = SSL_CTX_new(method);
	if (!ctx) {
		fprintf(stderr, "codybot error: Cannot create SSL context\n");
		exit(1);
	}

	SSL_CTX_set_ecdh_auto(ctx, 1);
	if (SSL_CTX_use_certificate_file(ctx, "nick.cert", SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "codybot error: Cannot load nick.cert\n");
		exit(1);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "nick.key", SSL_FILETYPE_PEM) <= 0 ) {
        fprintf(stderr, "codybot error: Cannot load nick.key\n");
		exit(1);
    }

	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
	pSSL = SSL_new(ctx);
	SSL_set_fd(pSSL, fd);
	BIO *bio = BIO_new_socket(fd, BIO_CLOSE);
	SSL_set_bio(pSSL, bio, bio);
	SSL_connect(pSSL);
	ret = SSL_accept(pSSL);
	if (ret <= 0) {
		fprintf(stderr, "codybot error: SSL_accept() failed, ret: %d\n", ret);
		fprintf(stderr, "error: %d\n", SSL_get_error(pSSL, 0));
		close(fd);
		exit(1);
	}

	buffer_rx = (char *)malloc(1024);
	memset(buffer_rx, 0, 1024);
	buffer_cmd = (char *)malloc(1024);
	memset(buffer_cmd, 0, 1024);
	buffer = (char *)malloc(1024);
	memset(buffer, 0, 1024);
	
	// start the thread here, as soon as we can
	pthread_t thr;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thr, &attr, ThreadFunc, NULL);
	pthread_detach(thr);
	pthread_attr_destroy(&attr);

	sprintf(buffer_cmd, "PASS none\n");
	SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
	sprintf(buffer_cmd, "NICK codybot\n");
	SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
	if (server_ip == server_ip_freenode) {
		sprintf(buffer_cmd, "USER bsfc BSFC-PC04 irc.freenode.net Steph\n");
		SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
	}
	else {
		sprintf(buffer_cmd, "USER codybot BSFC-PC04 irc.blinkenshell.org :Steph\n");
		SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
	}
}

void SignalFunc(int signum) {
	close(fd);
}

int main(int argc, char **argv) {
	signal(SIGINT, SignalFunc);
	int c;
	while (1) {
		c = getopt_long(argc, argv, short_options, long_options, NULL);
		if (c == -1)
			break;
		switch (c) {
		case 'h':
			HelpShow();
			exit(0);
		case 'V':
			printf("codybot %s\n", codybot_version_string);
			exit(0);
		case 'b':
			server_ip = server_ip_blinkenshell;
			break;
		case 'f':
			server_ip = server_ip_freenode;
			break;
		default:
			fprintf(stderr, "codybot error: Unknown argument: %c/%d\n", (char)c, c);
			break;
		}
	}

	if (!server_ip)
		server_ip = server_ip_freenode;

	ConnectClient();
	ReadCommandLoop();

	SSL_shutdown(pSSL);
	SSL_free(pSSL);
	ret = close(fd);

	return 0;
}

