#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "protocol.h"
#include "cache.h"
#include "lru.h"

#define COGENT_PORT 22122

#define RECVBUFSZ (1 << 16)

void *global_set;
void *global_get;

/* Ports are in NETWORK ORDER */

inline void handle_get(cogent_cache *cache, int sock, struct sockaddr_in *from, proto_client_get *req)
{
	cache_item *item = cache_fetch(cache, req->key);

	size_t resp_len;
	void *resp_buf;

	if (item == NULL)
		resp_buf = construct_server_get(FLAG_MISS, 0, "", &resp_len);
	else
		resp_buf = construct_server_get(0, (uint16_t) item->size, item->data, &resp_len);

	puts("sending");
	sendto(sock, resp_buf, resp_len, 0, (struct sockaddr *) from, sizeof(from));
	puts("done sending");

	g_slice_free1(req->key_len, req->key);
	g_slice_free(proto_client_get, req);
}

inline void handle_set(cogent_cache *cache, int sock, struct sockaddr_in *from, proto_client_set *req)
{
	cache_store(cache, req->key, req->val, req->val_len);
	g_slice_free1(req->val_len, req->val);
	g_slice_free(proto_client_set, req);
}

int main(int argc, char **argv)
{
	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock == -1) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}


	struct sockaddr_in servaddr, from;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(COGENT_PORT);
	if (bind(sock, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		perror("bind()");
		exit(EXIT_FAILURE);
	}

	socklen_t from_sz = sizeof(from);

	void *recv_buf = g_slice_alloc(RECVBUFSZ);

	cogent_cache *cache = cache_init(1e8);

	while (1) {
		ssize_t sz = recvfrom(sock, recv_buf, RECVBUFSZ, 0, (struct sockaddr *) &from, &from_sz);
		//printf("from port %hu\n", ntohs(from.sin_port));
		void *s = parse_buffer(recv_buf, sz);

		uint8_t cmd_byte = ((uint8_t *) s)[0];
		switch (CMD_BYTE(s)) {
			case CMD_CLIENT_GET:
				handle_get(cache, sock, &from, (proto_client_get *) s);
				break;
			case CMD_CLIENT_SET:
				handle_set(cache, sock, &from, (proto_client_set *) s);
				break;
			default:
				printf("unknown\n");
		}
	}

	exit(EXIT_SUCCESS);
}
