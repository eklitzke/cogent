#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include <stdint.h>

void* parse_buffer(void *buf, size_t len, uint8_t *cmd);

#endif
