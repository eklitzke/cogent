/* The binary protocol is documented in doc/protocol.txt */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"

#include <glib.h>

static void * parse_client_get(void *buf, proto_base *pb);
static void * parse_client_set(void *buf, proto_base *pb);

static void * parse_server_get(void *buf, proto_base *pb);

static uint8_t *
construct_base(uint8_t cmd, size_t sz)
{
	uint8_t *get = g_slice_alloc(sz);
	memcpy(get, MAGIC, 4);
	get[4] = 0;              /* version */
	get[5] = cmd;            /* cmd */
	get[6] = 0;              /* padding */
	get[7] = 0;              /* padding */
	memset(get + 8, 0, 4);   /* opaque id */
	return get;
}

void *
construct_client_get(const char *key, uint8_t key_len, size_t *buf_len)
{
	*buf_len = 13 + key_len;
	uint8_t *get = construct_base(CMD_CLIENT_GET, *buf_len);
	get[12] = key_len;
	memcpy(get + 13, key, key_len);
	return (void *) get;
}

void *
construct_client_set(const char *key, uint8_t key_len, const void *val, uint16_t val_len, size_t *buf_len)
{
	*buf_len = 15 + key_len + val_len;
	uint8_t *set = construct_base(CMD_CLIENT_SET, *buf_len);
	set[12] = key_len;
	memcpy(set + 13, &val_len, 2);
	memcpy(set + 15, key, key_len);
	memcpy(set + 15 + key_len, val, val_len);
	return (void *) set;
}

void *
construct_server_get(uint8_t flags, uint16_t val_len, const char *val, size_t *buf_len)
{
	*buf_len = 13 + val_len;
	uint8_t *get = construct_base(CMD_SERVER_GET, *buf_len);
	get[12] = val_len;
	memcpy(get + 13, val, val_len);
	return (void *) get;
}

void *
parse_buffer(void *buf, size_t len)
{
	if ((len < 12) || (memcmp(buf, MAGIC, 4)))
		return NULL;

	/* Parse the common parts of the header */
	proto_base base;
	base.version = ((uint8_t *) buf)[4] & 0x0f;
	base.cmd_byte = ((uint8_t *) buf)[5];

	/* FIXME: this is just dumb */
	memcpy(&base.padding, buf + 6, 2);
	memcpy(&base.opaque_id, buf + 8, 4);

	void *rest = buf + 12;

	/* What kind of command is it? */
	switch (base.cmd_byte) {
		case CMD_CLIENT_GET:
			return parse_client_get(rest, &base);
			break;
		case CMD_CLIENT_SET:
			return parse_client_set(rest, &base);
			break;
#if 0
		case CMD_CLIENT_DEL:
			return parse_client_del(rest);
			break;
		case CMD_CLIENT_QUERY:
			return parse_client_query(rest);
			break;
		case CMD_CLIENT_PING:
			return parse_client_ping(rest);
			break;
#endif
		case CMD_SERVER_GET:
			return parse_server_get(rest, &base);
			break;
#if 0
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
	proto_client_get *s = g_slice_alloc(sizeof(proto_client_get));
	memcpy(s, pb, sizeof(proto_base));
	s->key_len = *((uint8_t *) buf);
	s->key = g_slice_copy(s->key_len, buf + 1);
	return s;
}

static void *
parse_client_set(void *buf, proto_base *pb)
{
	proto_client_set *s = g_slice_alloc(sizeof(proto_client_set));
	memcpy(s, pb, sizeof(proto_base));
	s->key_len = *((uint8_t *) buf);
	s->val_len = *((uint8_t *) buf + 1);
	s->key = g_slice_copy(s->key_len, buf + 3);
	s->val = g_slice_copy(s->val_len, buf + 3 + s->key_len);
	return s;
}

static void *
parse_server_get(void *buf, proto_base *pb)
{
	proto_server_get *s = g_slice_alloc(sizeof(proto_server_get));
	memcpy(s, pb, sizeof(proto_base));
	s->val_len = *((uint8_t *) buf);
	s->val = g_slice_copy(s->val_len, buf + 1);
	return s;
}
