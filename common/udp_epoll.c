/*************************************************************************
	> File Name: udp_epoll.c
	> Author: 
	> Mail: 
	> Created Time: Thu 09 Jul 2020 04:40:40 PM CST
 ************************************************************************/

#include "head.h"

extern int port, repollfd, bepollfd;
extern s_player *rteam, *bteam;

void add_event_ptr(int epollfd, int fd, int events, s_player *player)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = (void *)player;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

void del_event(int epollfd, int fd) 
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
}

int find_sub(s_player *team)
{
    for (int i = 0; i < MAX_PLAYER; i++) {
        if (!team[i].online) return i;
    }
    return -1;
}

void add_to_sub_reactor(s_player *player)
{
    s_player *team;
    int sub;
    int events = EPOLLIN | EPOLLET;
    if (player->team) 
        team = bteam;
    else
        team = rteam;
    if ((sub = find_sub(team)) < 0) {
        perror("FULL TEAM!\n");
        return;
    }
    //team[sub] = *player;
    memcpy(&team[sub], player, sizeof(s_player));
    //team[sub].online = 1;
    //team[sub].flag = 10;
    DBG("<"L_RED"add to sub reactor"NONE"> sub = %d name = %s\n", sub, player->name);
    if (player->team)
        add_event_ptr(bepollfd, player->fd, events, player);
    else
        add_event_ptr(repollfd, player->fd, events, player);
}    

int udp_connect(struct sockaddr_in *client)
{
    int sockfd;
    if ((sockfd = socket_create_udp(port)) < 0) {
        perror("socket_udp()");
        return -1;
    }

    if (connect(sockfd, (struct sockaddr*)client, sizeof(struct sockaddr)) < 0) {
        perror("connect()");
        return -1;
    }
    return sockfd;
}

int check_online(s_log_request *request)
{
    for (int i = 0; i < MAX_PLAYER; i++) {
        if (bteam[i].online == 1 && strcmp(bteam[i].name, request->name) == 0) {
            return 1;
        }
        if (rteam[i].online == 1 && strcmp(rteam[i].name, request->name) == 0) {
            return 1;
        }
    }
    return 0;
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
    DBG("<"RED"Login Request!"NONE"> <%s:%d>\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
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

    printf("<"YELLOW"Server Info"NONE"> : %s log in\n", request.name);
    DBG("<"RED"Login success!"NONE">\n");
    response.type = 0;
    strcpy(response.msg, "Login success! Enjoy yourself!\n");
    sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);

    if (request.team) {
        DBG("<"GREEN"Info"NONE"> team: BLUE name: %s login on %s:%d <%s>\n", request.name, inet_ntoa(client.sin_addr), ntohs(client.sin_port), request.msg);
    } else {
        DBG("<"GREEN"Info"NONE"> team: RED name: %s login on %s:%d <%s>\n", request.name, inet_ntoa(client.sin_addr), ntohs(client.sin_port), request.msg);
    }

    strcpy(player->name, request.name);
    player->team = request.team;
    
    new_fd = udp_connect(&client);
    
    player->fd = new_fd;
    player->online = 1;
    player->flag = 10;

    return new_fd;
}
