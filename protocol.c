/* The binary protocol is documented in doc/protocol.txt */

#include <types.h>

#define CMD_CLIENT_GET   'G'
#define CMD_CLIENT_SET   'S'
#define CMD_CLIENT_DEL   'D'
#define CMD_CLIENT_QUERY 'Q'
#define CMD_CLIENT_PING  'P'

#define CMD_SERVER_GET   'g'
#define CMD_SERVER_SET   's'
#define CMD_SERVER_DEL   'd'
#define CMD_SERVER_QUERY 'q'
#define CMD_SERVER_PING  'p'

/* Need to terminate this macro with a semicolon */
#define PROTO_HEAD \
	uint8_t version; \
	uint16_t padding; \
	uint32_t opaque_id

/* The magic buffer */
uint8_t MAGIC[4] = {'C', 'G', 'N', 'T'}

typedef struct {
	PROTO_HEAD;
} proto_base;

typedef struct {
	PROTO_HEAD;
	uint8_t key_len;
	gconstpointer value;
} proto_client_get;

void *
parse_buffer(void *buf, ssize_t len, uint8_t *cmd)
{
	if ((len < 24) || (memcmp(buf, MAGIC, 4)))
		return NULL;

	/* Parse the common parts of the header */
	proto_base base;
	base.version = buf[5] & 0x0f;
	memcpy(base.padding, buf + 6, 2);
	memcpy(base.opaque_id, buf + 8, 4);

	void *rest = buf + 12;

	*cmd = buf[6];

	/* What kind of command is it? */
	switch (cmd) {
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
