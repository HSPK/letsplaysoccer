/*************************************************************************
	> File Name: udp_epoll.c
	> Author: 
	> Mail: 
	> Created Time: Thu 09 Jul 2020 04:40:40 PM CST
 ************************************************************************/

#include "head.h"

int udp_connect(struct sockaddr_in *client)
{
    int sockfd;
    if ((sockfd = socket_udp()) < 0) {
        perror("socket_udp()");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)client, sizeof(struct sockaddr)) < 0) {
        return -1;
    }
    return sockfd;
}

int udp_accept(int fd, s_player *player)
{
    int new_fd, retval;

    struct sockaddr_in client;
    s_log_request request;
    s_log_response response;
    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));

    socklen_t len = sizeof(client);

    retval = recvfrom(fd, (void *)&request, sizeof(request), 0, (struct sockaddr *)&client, &len);
    if (retval != sizeof(request)) {
        response.type = 1;
        strcpy(response.msg, "login failed with data errors!");
        sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
        return -1;
    }
    
    if (check_online(&request)) {
        response.type = 1;
        strcpy(response.msg, "you are already logged in!");
        sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
        return -1;
    }

    response.type = 0;
    strcpy(response.msg, "Login success! Enjoy your self!\n");
    sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);

    if (request.team) {
        DBG("<"GREEN"Info"NONE" : BLUE %s login on %s:%d <%s>\n", request.name, inet_ntoa(client.sin_addr), ntohs(client.sin_port), request.msg);
    } else {
        DBG("<"GREEN"Info"NONE" : BLUE %s login on %s:%d <%s>\n", request.name, inet_ntoa(client.sin_addr), ntohs(client.sin_port), request.msg);
    }

    strcpy(player->name, request.name);
    player->team = request.team;
    
    new_fd = udp_connect(&client);
    
    player->fd = new_fd;

    return new_fd;
}
