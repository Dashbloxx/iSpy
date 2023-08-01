#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "scanner.h"
#include "ipv4.h"
#include "check.h"

typedef struct
{
    char *ip_address;
    int port;
    int timeout;
} thread_args;

void *scanner_thread(void *arg)
{
    thread_args *args = (thread_args *)arg;

    char *ip_address = args->ip_address;
    int port = args->port;
    int timeout = args->timeout;

    int ret = check(ip_address, port, timeout);

    if (ret >= 0)
    {
        printf("%s:%d => \x1b[32;1monline\x1b[0m\n", ip_address, port);
    }
    else if (ret <= 0)
    {
        printf("%s:%d => \x1b[31;1moffline\x1b[0m\n", ip_address, port);
    }

    free(ip_address);
    free(args);
    pthread_exit(NULL);
}

void scanner(ipv4_t *ip0, ipv4_t *ip1, int timeout, int thread_count, uint16_t port)
{
    pthread_t threads[thread_count];
    int current_thread = 0;

    for (int a = (int)ip0->a; a <= (int)ip1->a; a++)
    {
        for (int b = (int)ip0->b; b <= (int)ip1->b; b++)
        {
            for (int c = (int)ip0->c; c <= (int)ip1->c; c++)
            {
                for (int d = (int)ip0->d; d <= (int)ip1->d; d++)
                {
                    ipv4_t *ipv4 = malloc(sizeof(ipv4_t));
                    ipv4->a = a;
                    ipv4->b = b;
                    ipv4->c = c;
                    ipv4->d = d;
                    char *ip_address = ipv4_to_string(ipv4);

                    // Create thread arguments
                    thread_args *args = malloc(sizeof(thread_args));
                    args->ip_address = ip_address;
                    args->port = port;
                    args->timeout = timeout;

                    // Create the thread
                    pthread_create(&threads[current_thread], NULL, scanner_thread, args);
                    current_thread = (current_thread + 1) % thread_count;

                    // Wait for all threads to finish before proceeding to the next iteration
                    if (current_thread == 0)
                    {
                        for (int i = 0; i < thread_count; i++)
                        {
                            pthread_join(threads[i], NULL);
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < current_thread; i++)
    {
        pthread_join(threads[i], NULL);
    }
}