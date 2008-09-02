#ifndef _UTIL_H_
#define _UTIL_H_

#include <sys/socket.h>
#include <netinet/in.h>

/* Random utilities that are not really specific to cogent stuff */

in_addr_t parse_hostname(const char *str);

uint16_t parse_port(const char *str);

#endif /* _UTIL_H_ */
