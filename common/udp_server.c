/*************************************************************************
	> File Name: ../common/udp_server.c
	> Author: 
	> Mail: 
	> Created Time: Thursday, July 09, 2020 AM11:14:30 CST
 ************************************************************************/

#include "head.h"

int socket_create_udp(int port)
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    make_non_block(sockfd);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind()");
        return -1;
    }
    return sockfd;
}
