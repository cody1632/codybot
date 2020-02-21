#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>

#include "codybot.h"

unsigned int server_port, local_port;
char *server_ip, *server_ip_blinkenshell = "194.14.45.5",
// livingstone.freenode.net, NY, US
//	*server_ip_freenode = "107.182.226.199";
// cherryh.freenode.net, MTL, CA
//	*server_ip_freenode = "149.56.134.238";
	*server_ip_freenode = "204.225.96.251";
SSL *pSSL;

void ServerGetIP(char *hostname) {
    struct hostent *he;
    struct in_addr **addr_list;
    int cnt = 0;

    he = gethostbyname(hostname);
    if (he == NULL) {
        fprintf(stderr, "##codybot::ServerGetIP() error: Cannot gethostbyname()\n");
        exit(1);
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    char *tmpstr = inet_ntoa(*addr_list[0]);
    server_ip = (char *)malloc(strlen(tmpstr)+1);
    sprintf(server_ip, "%s", tmpstr);

    if (debug) {
        for (cnt = 0; addr_list[cnt] != NULL; cnt++) {
            printf("%s\n", inet_ntoa(*addr_list[cnt]));
        }
    }
}

void ServerConnect(void) {
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		fprintf(stderr, "||codybot::ServerConnect() error: Cannot socket(): %s\n", strerror(errno));
		exit(1);
	}
	else {
		if (debug)
			printf("||socket_fd: %d\n", socket_fd);
	}
	
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	if (local_port)
		addr.sin_port = htons(local_port);
	if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "||codybot::ServerConnect() error: Cannot bind(): %s\n", strerror(errno));
		close(socket_fd);
		exit(1);
	}
	else {
		if (debug)
			printf("||bind() 0.0.0.0 successful\n");
	}
	
	struct sockaddr_in host;
	host.sin_addr.s_addr = inet_addr(server_ip);
	printf("####inet_addr()\n");
	host.sin_family = AF_INET;
	host.sin_port = htons(server_port);
	printf("####htons()\n");
	if (connect(socket_fd, (struct sockaddr *)&host, sizeof(host)) < 0) {
		fprintf(stderr, "||codybot::ServerConnect() error: Cannot connect(): %s\n", strerror(errno));
		close(socket_fd);
		exit(1);
	}
	else {
		if (debug)
			printf("||connect() %s successful\n", server_ip);
	}

	if (use_ssl) {
		SSL_load_error_strings();
		SSL_library_init();
		OpenSSL_add_all_algorithms();

		const SSL_METHOD *method = TLS_method();
		SSL_CTX *ctx = SSL_CTX_new(method);
		if (!ctx) {
			fprintf(stderr, "||codybot::ServerConnect() error: Cannot create SSL context\n");
			close(socket_fd);
			exit(1);
		}
		long opt_ctx;
		if (debug) {
			opt_ctx = SSL_CTX_get_options(ctx);
			printf("||opt_ctx: %ld\n", opt_ctx);
			SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION);
			opt_ctx = SSL_CTX_get_options(ctx);
			printf("||opt_ctx: %ld\n", opt_ctx);
			SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
		}
		else
			SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION);
	
		pSSL = SSL_new(ctx);
		long opt_ssl;
		if (debug) {
			opt_ssl = SSL_get_options(pSSL);
			printf("||opt_ssl: %ld\n", opt_ssl);
			SSL_set_options(pSSL, SSL_OP_NO_COMPRESSION);
			opt_ssl = SSL_get_options(pSSL);
			printf("||opt_ssl: %ld\n", opt_ssl);
			SSL_set_fd(pSSL, socket_fd);
			opt_ssl = SSL_get_options(pSSL);
			printf("||opt_ssl: %ld\n", opt_ssl);
		}
		else
			SSL_set_options(pSSL, SSL_OP_NO_COMPRESSION);
	
		BIO *bio = BIO_new_socket(socket_fd, BIO_CLOSE);
		SSL_set_bio(pSSL, bio, bio);
		SSL_connect(pSSL);
		ret = SSL_accept(pSSL);
		if (ret <= 0) {
			fprintf(stderr, "||codybot::ServerConnect() error: SSL_accept() failed, ret: %d\n", ret);
			fprintf(stderr, "||SSL error number: %d\n", SSL_get_error(pSSL, 0));
			close(socket_fd);
			exit(1);
		}
	}

	if (use_ssl)
		SSL_write(pSSL, "PASS none\n", 10);
	else
		write(socket_fd, "PASS none\n", 10);

	sprintf(buffer, "NICK %s\n", nick);
	if (use_ssl)
		SSL_write(pSSL, buffer, strlen(buffer));
	else
		write(socket_fd, buffer, strlen(buffer));

	if (server_ip == server_ip_freenode)
		sprintf(buffer, "USER %s %s irc.freenode.net %s\n", nick, hostname, full_user_name);
	else
		sprintf(buffer, "USER %s %s irc.blinkenshell.org :%s\n", nick, hostname, full_user_name);
	if (use_ssl)
		SSL_write(pSSL, buffer, strlen(buffer));
	else
		write(socket_fd, buffer, strlen(buffer));
}

void ServerClose(void) {
	if (use_ssl) {
		SSL_shutdown(pSSL);
    	SSL_free(pSSL);
	}
    close(socket_fd);
}

