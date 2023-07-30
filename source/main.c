#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

typedef struct
{
	uint8_t a, b, c, d;
	uint16_t port;
} address_t;


int validate(char *address, uint16_t port, char *response, int _timeout)
{
	int sockfd;
	struct sockaddr_in server;
	char buffer[2048];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1)
	{
		return -1;
	}

	struct timeval timeout;
	timeout.tv_sec = _timeout;
	timeout.tv_usec = 0;

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		return -2;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		return -2;
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(address);

	if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		return -3;
	}

	if (send(sockfd, "\x00\x00\x00\x00\x00\x00\x04\x01\x02\x00", sizeof("\x00\x00\x00\x00\x00\x00\x04\x01\x02\x00") - 1, 0) < 0)
	{
		return -4;
	}

	ssize_t bytes_read = recv(sockfd, buffer, sizeof(buffer) - 1, 0);

	if (bytes_read < 0)
	{
		return -5;
	}

	buffer[bytes_read] = '\0';

	response = &buffer[0];

	close(sockfd);

	return 0;
}


void remove_prefix(char *string)
{
	int length = strlen(string);
	if(length >= 2)
	{
		memmove(string, string + 2, length - 1);
		string[length - 2] = '\0';
	}
}

int main(int argc, const char *argv[])
{
	address_t address0;
	address_t address1;
	int timeout = 1;

	char saddress0[64];
	char saddress1[64];
	char stimeout[64];

	FILE *servers = fopen("servers.txt", "a");
	if(servers == NULL)
	{
		printf("Failed to open servers.txt for writing...\n");
		return -1;
	}

	for(unsigned int i = 0; i < argc; i++)
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
	}

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
						char buffer0[64] = {0};
						char *buffer1;
						sprintf(buffer0, "%u.%u.%u.%u", a, b, c, d);
						if(validate(buffer0, port, buffer1, timeout) == 0)
						{
							printf("CHECKING\t%s:%hu\tFOUND AN AVAILABLE CONNECTIIN!\n", buffer0, port);
							fprintf(servers, "%s:%hu\n", buffer0, port);
						}
						else
						{
							printf("CHECKING\t%s:%hu\tFOUND NOTHING\n", buffer0, port);
						}
					}
				}
			}
		}
	}

	fclose(servers);
}
