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

const char *codybot_version_string = "0.0.4";

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
char *server_ip_blinkenshell = "194.14.45.5";
char *server_ip_freenode = "204.225.96.251";
SSL *pSSL;

void HelpShow(void) {
	printf("Usage: codybot { -h/--help | -V/--version }\n");
}

// raw should hold something like ":esselfe!~bsfc@unaffiliated/esselfe PRIVMSG #esselfe :!fortune"
char *raw_to_word(char *raw) {
	char *c = raw;
	unsigned int cnt = 0, cnt_total = strlen(raw);
	while (1) {
		++c;
		++cnt;
		if (*c == ':')
			break;
		else if (*c == '\0')
			break;
	}
	char *word = (char *)malloc(cnt_total-cnt+1);
	cnt = 0;
	while (1) {
		//sprintf(word, "%s", c);
		word[cnt] = *(c+cnt);
		if (*(c+cnt+1) == ' ' || *(c+cnt+1) == '\0') {
			word[cnt+1] = '\0';
			break;
		}
	}

	return word;
}

void *ThreadFunc(void *argp) {
	struct timeval tv0;
	struct tm *tm0;
	time_t t0;
	while (!endmainloop) {
		memset(buffer_rx, 0, 1024);
		SSL_read(pSSL, buffer_rx, 1023);
		gettimeofday(&tv0, NULL);
		t0 = (time_t)tv0.tv_sec;
		tm0 = gmtime(&t0);
		printf("%02d:%02d:%02d.%03ld:<<%s>>\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
			tv0.tv_usec, buffer_rx);
		// respond to ping request from the server
		if (buffer_rx[0] == 'P' && buffer_rx[1] == 'I' && buffer_rx[2] == 'N' &&
			buffer_rx[3] == 'G' && buffer_rx[4] == ' ' && buffer_rx[5] == ':') {
			SSL_write(pSSL, "PONG\n", 11);
			gettimeofday(&tv0, NULL);
			t0 = (time_t)tv0.tv_sec;
			tm0 = gmtime(&t0);
			printf("%02d:%02d:%02d.%03ld:[[PONG sent]]\n", tm0->tm_hour, tm0->tm_min, tm0->tm_sec,
				tv0.tv_usec);
		}
		//printf("raw_to_word(): <%s>\n", raw_to_word(buffer_rx));
		//printf("raw_to_word()\n");
		usleep(10000);
	}
	return NULL;
}

void ReadCommandLoop(void) {
	while (!endmainloop) {
		memset(buffer_cmd, 0, 1024);
		fgets(buffer_cmd, 1024, stdin);
		if (strcmp(buffer_cmd, "exit")==0)
			endmainloop = 1;
		else
			SSL_write(pSSL, buffer_cmd, strlen(buffer_cmd));
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
	addr.sin_addr.s_addr = inet_addr("192.168.2.168");
	//addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(16423);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "codybot error: Cannot bind(): %s\n", strerror(errno));
		exit(1);
	}
	else
		printf("bind() 192.168.2.168:16423 successful\n");
	
	struct sockaddr_in host;
	//blinkenshell
	//host.sin_addr.s_addr = inet_addr("194.14.45.5");
	//freenode
	//host.sin_addr.s_addr = inet_addr("204.225.96.251");
	host.sin_addr.s_addr = inet_addr(server_ip);
	host.sin_family = AF_INET;
	host.sin_port = htons(6697);
	if (connect(fd, (struct sockaddr *)&host, sizeof(host)) < 0) {
		fprintf(stderr, "codybot error: Cannot connect(): %s\n", strerror(errno));
		exit(1);
	}
	else
		printf("connect() 194.14.45.5 successful\n");

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
	
	pthread_t thr;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thr, &attr, ThreadFunc, NULL);
	pthread_detach(thr);
	pthread_attr_destroy(&attr);

	SSL_write(pSSL, "PASS none\n", 10);
	SSL_write(pSSL, "NICK codybot\n", 13);
	if (server_ip == server_ip_freenode)
		SSL_write(pSSL, "USER bsfc BSFC-PC04 irc.freenode.net Steph\n", 43);
	else
		SSL_write(pSSL, "USER bsfc BSFC-PC04 irc.blinkenshell.org Steph\n", 46);

	ReadCommandLoop();

	SSL_shutdown(pSSL);
	SSL_free(pSSL);
	ret = close(fd);
	printf("close() returns %d\n", ret);
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

	return 0;
}

