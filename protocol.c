/* The binary protocol is documented in doc/protocol.txt */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"

#define CMD_CLIENT_GET   ((uint8_t) 'G')
#define CMD_CLIENT_SET   ((uint8_t) 'S')
#define CMD_CLIENT_DEL   ((uint8_t) 'D')
#define CMD_CLIENT_QUERY ((uint8_t) 'Q')
#define CMD_CLIENT_PING  ((uint8_t) 'P')

#define CMD_SERVER_GET   ((uint8_t) 'g')
#define CMD_SERVER_SET   ((uint8_t) 's')
#define CMD_SERVER_DEL   ((uint8_t) 'd')
#define CMD_SERVER_QUERY ((uint8_t) 'q')
#define CMD_SERVER_PING  ((uint8_t) 'p')

/* Need to terminate this macro with a semicolon */
#define PROTO_HEAD \
	uint8_t version; \
	uint16_t padding; \
	uint32_t opaque_id

static uint8_t MAGIC[] = { (uint8_t) 'C', (uint8_t) 'G', (uint8_t) 'N', (uint8_t) 'T' };

typedef struct {
	PROTO_HEAD;
} proto_base;

typedef struct {
	PROTO_HEAD;
	uint8_t key_len;
	const void *value;
} proto_client_get;

void *
parse_buffer(void *buf, size_t len, uint8_t *cmd)
{
	if ((len < 24) || (memcmp(buf, MAGIC, 4)))
		return NULL;

	/* Parse the common parts of the header */
	proto_base base;
	base.version = ((uint8_t *) buf)[5] & 0x0f;

	/* FIXME: this is just dumb */
	memcpy(&base.padding, buf + 6, 2);
	memcpy(&base.opaque_id, buf + 8, 4);

	void *rest = buf + 12;

	*cmd = ((uint8_t *) buf)[6];

	/* What kind of command is it? */
	switch (*cmd) {
		case CMD_CLIENT_GET:
			return parse_client_get(rest);
			break;
		case CMD_CLIENT_SET:
			return parse_client_set(rest);
			break;
		case CMD_CLIENT_DEL:
			return parse_client_del(rest);
			break;
		case CMD_CLIENT_QUERY:
			return parse_client_query(rest);
			break;
		case CMD_CLIENT_PING:
			return parse_client_ping(rest);
			break;
		case CMD_SERVER_GET:
			return parse_server_get(rest);
			break;
		case CMD_SERVER_SET:
			return parse_server_set(rest);
			break;
		case CMD_SERVER_DEL:
			return parse_server_del(rest);
			break;
		case CMD_SERVER_QUERY:
			return parse_server_query(rest);
			break;
		case CMD_SERVER_PING:
			return parse_server_ping(rest);
			break;
		default:
			return NULL;
	}
}
