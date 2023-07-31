#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/sem.h>
#include <pthread.h>
#include <semaphore.h>

extern bool validate(char* ip_address, int port, int timeout_seconds);

sem_t thread_semaphore;
int max_threads = 100;

typedef struct
{
	uint8_t a, b, c, d;
	uint16_t port;
} address_t;

void remove_prefix(char *string)
{
	int length = strlen(string);
	if(length >= 2)
	{
		memmove(string, string + 2, length - 1);
		string[length - 2] = '\0';
	}
}

void* thread_function(void* arg)
{
    int* thread_args = (int*)arg;
    int a = thread_args[0];
    int b = thread_args[1];
    int c = thread_args[2];
    int d = thread_args[3];
    int port = thread_args[4];
    int timeout = thread_args[5];

    char buffer[64] = {0};
    sprintf(buffer, "%u.%u.%u.%u", a, b, c, d);
    if (validate(buffer, port, timeout))
	{
        printf("CHECKING\t%s:%hu\tFOUND AN AVAILABLE CONNECTION!\n", buffer, port);
        FILE* servers = fopen("servers.txt", "a");
        if (servers != NULL) {
            fprintf(servers, "%s:%hu\n", buffer, port);
            fclose(servers);
        }
    }
	else
	{
        printf("CHECKING\t%s:%hu\tFOUND NOTHING\n", buffer, port);
    }

    sem_post(&thread_semaphore);
    pthread_exit(NULL);
}

int main(int argc, const char *argv[])
{
	address_t address0;
	address_t address1;
	int timeout = 1;

	char saddress0[64];
	char saddress1[64];
	char stimeout[64];
	char sthreads[64];

	FILE *servers = fopen("servers.txt", "a");
	if(servers == NULL)
	{
		printf("Failed to open servers.txt for writing...\n");
		return -1;
	}

	for(int i = 0; i < argc; i++)
	{
		if(argv[i][0] == '-' && argv[i][1] == 'A')
		{
			strcpy(saddress0, argv[i]);
			remove_prefix(saddress0);
			if(sscanf(saddress0, "%hhu.%hhu.%hhu.%hhu:%hu", &address0.a, &address0.b, &address0.c, &address0.d, &address0.port) != 5)
			{
				printf("Invalid address: %s!", saddress0);
				return -1;
			}
		}
		else if(argv[i][0] == '-' && argv[i][1] == 'B')
		{
			strcpy(saddress1, argv[i]);
			remove_prefix(saddress1);
			if(sscanf(saddress1, "%hhu.%hhu.%hhu.%hhu:%hu", &address1.a, &address1.b, &address1.c, &address1.d, &address1.port) != 5)
			{
				printf("Invalid address: %s!", saddress1);
				return -1;
			}
		}
		else if(argv[i][0] == '-' && argv[i][1] == 'T')
		{
			strcpy(stimeout, argv[i]);
			remove_prefix(stimeout);
			timeout = atoi(stimeout);
		}
		else if(argv[i][0] == '-' && argv[i][1] == 't')
		{
			strcpy(sthreads, argv[i]);
			remove_prefix(sthreads);
			max_threads = atoi(sthreads);
		}
	}

	sem_init(&thread_semaphore, 0, max_threads);

	for(unsigned int a = address0.a; a <= address1.a; a++)
	{
		for(unsigned int b = address0.b; b <= address1.b; b++)
		{
			for(unsigned int c = address0.c; c <= address1.c; c++)
			{
				for(unsigned int d = address0.d; d <= address1.d; d++)
				{
					for(unsigned int port = address0.port; port <= address1.port; port++)
					{
						sem_wait(&thread_semaphore);
						pthread_t thread_id;
						int thread_args[6] = {a, b, c, d, port, timeout};
						pthread_create(&thread_id, NULL, thread_function, (void*)thread_args);
					}
				}
			}
		}
	}

	for (int i = 0; i < max_threads; i++)
	{
        sem_wait(&thread_semaphore);
    }

	fclose(servers);

	sem_destroy(&thread_semaphore);

	return 0;
}
