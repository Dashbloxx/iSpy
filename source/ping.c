#include <stdarg.h>
#include <stdlib.h>

#include "ping.h"
#include "ipv4.h"

bool ping(ipv4_t ipv4)
{
    char * command = malloc(sizeof(char) * 128);
    sprintf(command, "ping -c 1 %s.%s.%s.%s", ipv4->a, ipv4->b, ipv4->c, ipv4->d);
    if(system(command) == 0)
    {
        free(command);
        return true;
    }
    free(command);
    return false;
}