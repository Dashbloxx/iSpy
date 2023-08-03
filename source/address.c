#include <stdlib.h>
#include <stdio.h>

#include "address.h"
#include "ipv4.h"

address_t * string_to_address(char * string)
{
	address_t * address = malloc(sizeof(address_t));
	char * ipv4_string = malloc(sizeof(char) * 128);
	int port;
	sscanf(string, "%s:%d", ipv4_string, &port);
	ipv4_t *ipv4 = string_to_ipv4(ipv4_string);
	address->port = (uint16_t)port;
	address->ipv4 = ipv4;
	free(ipv4_string);
	return address;
}
