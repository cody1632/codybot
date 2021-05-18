#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>

#include "codybot.h"

unsigned int server_port, local_port;
char *server_ip, *server_name;
SSL *pSSL;

// Retrieve an IP from a domain name.
void ServerGetIP(char *hostname2) {
	struct hostent *he;
	struct in_addr **addr_list;
	int cnt = 0;

	he = gethostbyname(hostname2);
	if (he == NULL) {
		sprintf(buffer, "||codybot::ServerGetIP() error: Cannot gethostbyname(%s)",
			hostname2);
		Log(LOCAL, buffer);
		exit(1);
	}

	addr_list = (struct in_addr **)he->h_addr_list;

	char *tmpstr = inet_ntoa(*addr_list[0]);
	server_ip = (char *)malloc(strlen(tmpstr)+1);
	sprintf(server_ip, "%s", tmpstr);
	server_name = malloc(strlen(hostname2)+1);
	sprintf(server_name, "%s", hostname2);

	if (debug) {
		sprintf(buffer, "||codybot::ServerGetIP(%s): other IPs:", hostname2);
		Log(LOCAL, buffer);
		for (cnt = 0; addr_list[cnt] != NULL; cnt++) {
			sprintf(buffer, "  %s", inet_ntoa(*addr_list[cnt]));
			Log(LOCAL, buffer);
		}
	}
}

void ServerConnect(void) {
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
		sprintf(buffer, "||codybot::ServerConnect() error: Cannot socket(): %s",
			strerror(errno));
		Log(LOCAL, buffer);
		exit(1);
	}
	else {
		if (debug) {
			sprintf(buffer, "||socket_fd: %d", socket_fd);
			Log(LOCAL, buffer);
		}
	}
	
	struct sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	if (local_port)
		addr.sin_port = htons(local_port);
	if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		sprintf(buffer, "||codybot::ServerConnect() error: Cannot bind(): %s",
			strerror(errno));
		Log(LOCAL, buffer);
		close(socket_fd);
		exit(1);
	}
	else
		if (debug)
			Log(LOCAL, "||bind() 0.0.0.0 successful");
	
	struct sockaddr_in host;
	host.sin_addr.s_addr = inet_addr(server_ip);
	host.sin_family = AF_INET;
	host.sin_port = htons(server_port);
	if (connect(socket_fd, (struct sockaddr *)&host, sizeof(host)) < 0) {
		sprintf(buffer, "||codybot::ServerConnect() error: Cannot connect(): %s",
			strerror(errno));
		Log(LOCAL, buffer);
		close(socket_fd);
		exit(1);
	}
	else {
		if (debug) {
			sprintf(buffer, "||connect() %s successful", server_ip);
			Log(LOCAL, buffer);
		}
	}

	if (use_ssl) {
		SSL_load_error_strings();
		SSL_library_init();
		OpenSSL_add_all_algorithms();

		const SSL_METHOD *method = TLS_method();
		SSL_CTX *ctx = SSL_CTX_new(method);
		if (!ctx) {
			Log(LOCAL, "||codybot::ServerConnect() error: Cannot create SSL context");
			close(socket_fd);
			exit(1);
		}
		SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION);
	
		pSSL = SSL_new(ctx);
		//long opt_ssl;
		/* if (debug) {
			opt_ssl = SSL_get_options(pSSL);
			printf("||opt_ssl: %ld\n", opt_ssl);
			SSL_set_options(pSSL, SSL_OP_NO_COMPRESSION);
			opt_ssl = SSL_get_options(pSSL);
			printf("||opt_ssl: %ld\n", opt_ssl);
			SSL_set_fd(pSSL, socket_fd);
			opt_ssl = SSL_get_options(pSSL);
			printf("||opt_ssl: %ld\n", opt_ssl);
		}
		else */
			SSL_set_options(pSSL, SSL_OP_NO_COMPRESSION);
	
		BIO *bio = BIO_new_socket(socket_fd, BIO_CLOSE);
		SSL_set_bio(pSSL, bio, bio);
		SSL_connect(pSSL);
		ret = SSL_accept(pSSL);
		if (ret <= 0) {
			sprintf(buffer, "||codybot::ServerConnect() error: "
				"SSL_accept() failed, ret: %d", ret);
			Log(LOCAL, buffer);
			sprintf(buffer, "||SSL error number: %d", SSL_get_error(pSSL, 0));
			Log(LOCAL, buffer);
			close(socket_fd);
			exit(1);
		}
	}

	// The order of the initial IRC connection commands are from RFC1459
	////////////////

	// Don't expose password since this source code can be read by the bot
/*	if (use_ssl)
		SSL_write(pSSL, "PASS none\n", 10);
	else
		write(socket_fd, "PASS none\n", 10);
*/
	struct stat st;
	if (stat(".passwd", &st) == 0) {
		FILE *fp = fopen(".passwd", "r");
		if (fp != NULL) {
			char pass[128];
			sprintf(pass, "PASS ");
			fread(pass+5, 1, 127-5, fp);
			pass[strlen(pass)] = '\n';
			if (use_ssl)
				SSL_write(pSSL, pass, strlen(pass));
			memset(pass, 0, 128);
			fclose(fp);
		}
	}

	sprintf(buffer, "NICK %s\n", nick);
	MsgRaw(buffer);

	sprintf(buffer, "USER %s %s %s %s\n", nick, hostname,
		server_name, full_user_name);
	MsgRaw(buffer);
}

void ServerClose(void) {
	if (use_ssl) {
		SSL_shutdown(pSSL);
		SSL_free(pSSL);
	}
	close(socket_fd);
}

