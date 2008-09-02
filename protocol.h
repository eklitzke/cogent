#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>
#include <glib.h>

/* Clients will need these to figure out what kind of struct they have */

#define FLAG_MISS 1

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

/* Get the value of the command byte from the struct */
#define CMD_BYTE(x)      (((uint8_t *) x)[0])

/* Like CMD_BYTE, but to be used on a raw buffer */
#define BUF_CHAR(x)      ((char) ((char *)x)[5])

/* Need to terminate this macro with a semicolon */
#define PROTO_HEAD \
	uint8_t cmd_byte; \
	uint8_t version; \
	uint16_t padding; \
	uint32_t opaque_id

static const uint8_t MAGIC[] = { 0x43, 0x47, 0x4e, 0x54 };

typedef struct {
	PROTO_HEAD;
} proto_base;

typedef struct {
	PROTO_HEAD;
	uint8_t key_len;
	gpointer key;
} proto_client_get;

typedef struct {
	PROTO_HEAD;
	uint8_t key_len;
	uint8_t val_len;
	gpointer key;
	gpointer val;
} proto_client_set;

typedef struct {
	PROTO_HEAD;
	uint8_t key_len;
	gpointer key;
} proto_server_del;

typedef struct {
	PROTO_HEAD;
	uint8_t val_len;
	gpointer val;
} proto_server_get;

/* TODO: should probably signal if the item had to be replaced, etc. */
typedef struct {
	PROTO_HEAD;
} proto_server_set;


void* parse_buffer(void *buf, size_t len);

void* construct_client_get(const char *key, uint8_t key_len, size_t *buf_len);
void* construct_client_set(const char *key, uint8_t key_len, const void *val, uint16_t val_len, size_t *buf_len);
void* construct_client_del(const char *key, uint8_t key_len, size_t *buf_len);

void* construct_server_get(uint8_t flags, uint16_t val_len, const char *val, size_t *buf_len);
void* construct_server_set(uint8_t flags, size_t *buf_len);
void* construct_server_del(uint8_t flags, size_t *buf_len);

#endif
