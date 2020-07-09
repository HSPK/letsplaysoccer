/*************************************************************************
	> File Name: ../common/udp_client.c
	> Author: 
	> Mail: 
	> Created Time: Thursday, July 09, 2020 AM11:57:24 CST
 ************************************************************************/

#include "head.h"

int socket_udp()
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }
    return sockfd;
}
