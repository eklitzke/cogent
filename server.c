#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>

#include "protocol.h"
#include "cache.h"
#include "lru.h"

#define MIN_MEM 1
#define MAX_MEM 1024

#define COGENT_PORT 22122

#define RECVBUFSZ (1 << 16)

void *global_set;
void *global_get;

/* only log if DEBUG is defined */
void log_msg(const char *format_str, ...)
{
#ifdef DEBUG
	va_list ap;
	va_start(ap, format_str);
	vprintf(format_str, ap);
	va_end(ap);
#endif
}

void send_response(int sock, struct sockaddr_in *from, void *resp_buf, size_t resp_len)
{
	if (sendto(sock, resp_buf, resp_len, 0, (struct sockaddr *) from, sizeof(struct sockaddr)) < 0)
		perror("sendto()");
	log_msg("[%c] sent msg to port %hu using UDP transport\n", BUF_CHAR(resp_buf), ntohs(from->sin_port));
}

inline void handle_get(cogent_cache *cache, int sock, struct sockaddr_in *from, proto_client_get *req)
{
	cache_item *item = cache_fetch(cache, req->key);

	void *resp_buf;
	size_t resp_len;

	if (item == NULL)
		resp_buf = construct_server_get(FLAG_MISS, 0, "", &resp_len);
	else
		resp_buf = construct_server_get(0, (uint16_t) item->size, item->data, &resp_len);

	send_response(sock, from, resp_buf, resp_len);

	g_slice_free1(req->key_len, req->key);
	g_slice_free(proto_client_get, req);
}

inline void handle_set(cogent_cache *cache, int sock, struct sockaddr_in *from, proto_client_set *req)
{
	cache_store(cache, req->key, req->val, req->val_len);

	/* TODO: implement flags */
	size_t resp_len;
	void *resp_buf = construct_server_set(0, &resp_len);

	send_response(sock, from, resp_buf, resp_len);

	g_slice_free1(req->val_len, req->val);
	g_slice_free(proto_client_set, req);
}

inline void handle_del(cogent_cache *cache, int sock, struct sockaddr_in *from, proto_client_set *req)
{
	cache_store(cache, req->key, req->val, req->val_len);

	/* TODO: implement flags */
	size_t resp_len = 12;
	void *resp_buf = construct_server_del(0, &resp_len);

	send_response(sock, from, resp_buf, resp_len);

	g_slice_free1(req->val_len, req->val);
	g_slice_free(proto_client_set, req);
}

int main(int argc, char **argv)
{
	int c;
	int memory = 64; /* in megabytes */
	gboolean daemonize = FALSE;
	opterr = 0;

	while ((c = getopt(argc, argv, "dhm:")) != -1)
		switch (c)
		{
			case 'd':
				daemonize = TRUE;
				break;
			case 'h':
				/* FIXME */
				break;
			case 'm':
				memory = atoi(optarg);
				break;
			case '?':
				if (optopt == 'm')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown uption `-%c',\n", optopt);
				else
					fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				exit(EXIT_FAILURE);
			default:
				abort();
		}

	if ((memory < MIN_MEM) || (memory > MAX_MEM)) {
		fprintf(stderr, "Must have %d <= memory <= %d\n", MIN_MEM, MAX_MEM);
		exit(EXIT_FAILURE);
	}

	if (daemonize)
		fprintf(stderr, "daemon mode not yet implemented, continuing in foreground...\n");

	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
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

	/* Memory is specified in megabytes, but cache_init wants bytes */
	cogent_cache *cache = cache_init(memory << 20);

	while (1) {
		ssize_t sz = recvfrom(sock, recv_buf, RECVBUFSZ, 0, (struct sockaddr *) &from, &from_sz);
		if (sz == -1 ) {
			perror("recvfrom()");
			exit(EXIT_FAILURE);
		}
		void *s = parse_buffer(recv_buf, sz);
		if (s == NULL) {
			fprintf(stderr, "failed to parse input, ignoring message\n");
			continue;
		}

		uint8_t cmd_byte = ((uint8_t *) s)[0];
		switch (CMD_BYTE(s)) {
			case CMD_CLIENT_GET:
				handle_get(cache, sock, &from, (proto_client_get *) s);
				break;
			case CMD_CLIENT_SET:
				handle_set(cache, sock, &from, (proto_client_set *) s);
				break;
			default:
				fprintf(stderr, "unknown\n");
		}
	}

	exit(EXIT_SUCCESS);
}
