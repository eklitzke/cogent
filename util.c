#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "util.h"


/* ONLY WORKS FOR DOTTED QUAD IPS, NOT HOSTNAMES */
in_addr_t
parse_hostname(const char *str)
{
	assert(str != NULL);

	/* If this is part of a string like ip:hostname we need to remove the colon
	 * and terminating portion */
	const char *colon = strchr(str, ':');

	/* This fails for 255.255.255.255 but that's OK for our purposes */

	if (colon != NULL) {
		char *new_string = strndup(str, (size_t) (colon - str));
		in_addr_t result = inet_addr(new_string);
		free(new_string);
		return result;
	}
	else
		return inet_addr(str);
}

/* Returns 0 on failure */
uint16_t
parse_port(const char *str)
{
	assert(str != NULL);

	char *colon = strchr(str, ':');
	if (colon == NULL)
		return 0;

	if (++colon == '\0')
		return 0;

	return (uint16_t) atoi(colon);
}
