/* The binary protocol is documented in doc/protocol.txt */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"

/* Need to terminate this macro with a semicolon */
#define PROTO_HEAD \
	uint8_t cmd_byte; \
	uint8_t version; \
	uint16_t padding; \
	uint32_t opaque_id

static const uint8_t MAGIC[] = { (uint8_t) 'C', (uint8_t) 'G', (uint8_t) 'N', (uint8_t) 'T' };

typedef struct {
	PROTO_HEAD;
} proto_base;

typedef struct {
	PROTO_HEAD;
	uint8_t key_len;
	const void *value;
} proto_client_get;

static void * parse_client_get(void *buf, proto_base *pb);

void *
parse_buffer(void *buf, size_t len, uint8_t *cmd)
{
	if ((len < 24) || (memcmp(buf, MAGIC, 4)))
		return NULL;

	/* Parse the common parts of the header */
	proto_base base;
	base.version = ((uint8_t *) buf)[5] & 0x0f;
	base.cmd_byte = ((uint8_t *) buf)[6];

	/* FIXME: this is just dumb */
	memcpy(&base.padding, buf + 6, 2);
	memcpy(&base.opaque_id, buf + 8, 4);

	void *rest = buf + 12;

	/* What kind of command is it? */
	switch (base.cmd_byte) {
		case CMD_CLIENT_GET:
			return parse_client_get(rest, &base);
			break;
#if 0
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
#endif
		default:
			return NULL;
	}
}

static void *
parse_client_get(void *buf, proto_base *pb)
{
	proto_client_get *s = malloc(sizeof(proto_client_get));
	memcpy(s, pb, sizeof(proto_base));
	s->key_len = *((uint8_t *) pb);
	memcpy(s, pb + 1, s->key_len);
	return s;
}
