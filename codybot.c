#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

const char *codybot_version_string = "0.0.1";

static const struct option long_options[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'V'},
	{NULL, 0, NULL, 0}
};
static const char *short_options = "hV";

void HelpShow(void) {
	printf("Usage: codybot { -h/--help | -V/--version }\n");
}

void ConnectClient(void) {
	const SSL_METHOD *method = TLS_method();
	SSL_CTX *ctx = SSL_CTX_new(method);
	if (!ctx) {
		fprintf(stderr, "codybot error: Cannot create SSL context\n");
		exit(1);
	}

	SSL_CTX_set_ecdh_auto(ctx, 1);
	if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "codybot error: Cannot load nick.cer\n");
		exit(1);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0 ) {
        fprintf(stderr, "codybot error: Cannot load nick.key\n");
		exit(1);
    }

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		fprintf(stderr, "codybot error: Cannot socket(): %s\n", strerror(errno));
		exit(1);
	}
	else
		printf("fd: %d\n", fd);
	
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = inet_addr("192.168.2.168");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(16416);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "codybot error: Cannot bind(): %s\n", strerror(errno));
		exit(1);
	}
	else
		printf("bind() 192.168.2.168:16416 successful\n");
	
	struct sockaddr_in host;
	host.sin_addr.s_addr = inet_addr("194.14.45.5");
	host.sin_family = AF_INET;
	host.sin_port = htons(6697);
	if (connect(fd, (struct sockaddr *)&host, sizeof(host)) < 0) {
		fprintf(stderr, "codybot error: Cannot connect(): %s\n", strerror(errno));
		exit(1);
	}
	else
		printf("connect() 194.14.45.5 successful\n");
	
	char *buffer = (char *)malloc(1024);
	memset(buffer, 0, 1024);
	sprintf(buffer, "PASS 16326486##");
	int ret = send(fd, buffer, strlen(buffer), 0);
	printf("send() returns %d\n", ret);

	char *buffer_rx = (char *)malloc(1024);
	memset(buffer_rx, 0, 1024);
	ret = recv(fd, buffer, 1023, 0);
	printf("recv() returns %d, <%s>\n", ret, buffer);

	ret = close(fd);
	printf("close() returns %d\n", ret);
}

int main(int argc, char **argv) {
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
		default:
			fprintf(stderr, "codybot error: Unknown argument: %c/%d\n", (char)c, c);
			break;
		}
	}

	ConnectClient();

	return 0;
}

