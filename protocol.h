#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>

/* Clients will need these to figure out what kind of struct they have */

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


void* parse_buffer(void *buf, size_t len, uint8_t *cmd);

#endif
