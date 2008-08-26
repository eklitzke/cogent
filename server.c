#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "protocol.h"

#define COGENT_PORT 22122

int main(int argc, char **argv)
{
	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == -1) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//servaddr.sin_port = htons(COGENT_PORT);
	servaddr.sin_port = htons(0);
	if (bind(sock, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	/* Find out the port we bind()ed to */
	int dummy = sizeof(servaddr);
	if (getsockname(sock, (struct sockaddr *) &servaddr, &dummy) < 0) {
		perror("getsockname()");
		exit(EXIT_FAILURE);
	}

	printf("Bound to UDP port %hu\n", ntohs(servaddr.sin_port));
	void *recv_buf = malloc(1 << 16);

	while (1) {
		size_t sz = recv(sock, recv_buf, 1 << 16, 0);
		void *s = parse_buffer(recv_buf, sz);
		switch (CMD_BYTE(s)) {
			case CMD_CLIENT_GET:
				printf("got a GET\n");
				break;
			default:
				printf("unknown\n");
		}
	}

	exit(EXIT_SUCCESS);
}
