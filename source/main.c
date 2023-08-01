#define _GNU_SOURCE
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "ipv4.h"
#include "scanner.h"

int thread_count = 5;
int timeout = 1;
char * opt_ip0 = NULL;
char * opt_ip1 = NULL;
int port = 80;

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "t:T:0:1:p:")) != -1) {
        switch (opt) {
            case 't':
                thread_count = atoi(optarg);
                break;
            case 'T':
                timeout = atoi(optarg);
                break;
            case '0':
                opt_ip0 = optarg;
                break;
            case '1':
                opt_ip1 = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case '?':
            default:
                fprintf(stderr, "Usage: %s -t <thread_count> -T <timeout> -0 <ip0> -1 <ip1> -p <port>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    ipv4_t * ip0 = NULL;
    ipv4_t * ip1 = NULL;

    ip0 = string_to_ipv4(opt_ip0);
    ip1 = string_to_ipv4(opt_ip1);

    scanner(ip0, ip1, timeout, thread_count, port);
}