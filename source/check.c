#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/select.h>

#include "check.h"

int check(char* ip_address, int port, int timeout_seconds)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        return -1;
    }

    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0)
    {
        close(sockfd);
        return -2;
    }

    int connect_result = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (connect_result < 0)
    {
        if (errno == EINPROGRESS)
        {
            fd_set fdset;
            FD_ZERO(&fdset);
            FD_SET(sockfd, &fdset);

            struct timeval timeout;
            timeout.tv_sec = timeout_seconds;
            timeout.tv_usec = 0;

            int select_result = select(sockfd + 1, NULL, &fdset, NULL, &timeout);
            if (select_result == 1)
            {
                int so_error;
                socklen_t len = sizeof(so_error);
                getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_error, &len);
                if (so_error == 0) {
                    close(sockfd);
                    return 0;
                }
            }
            else if (select_result == 0)
            {
                close(sockfd);
                return -3;
            }
            else
            {
                close(sockfd);
                return -4;
            }
        }
        else
        {
            close(sockfd);
            return -5;
        }
    }
    else
    {
        close(sockfd);
        return 1;
    }

    close(sockfd);
    return -5;
}