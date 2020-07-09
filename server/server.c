/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Thursday, July 09, 2020 AM10:52:52 CST
 ************************************************************************/

#include "../common/head.h"

#define COURT_STARTX 3
#define COURT_STARTY 3

#define MAX_CLIENTS 250

char conf[] = "./footballd.conf";
s_map court;    //小足球场
s_point ball;    //球的位置
s_Bstatus ball_status;  //球的状态
s_score score;
int repollfd, bepollfd;

int main(int argc, char **argv) 
{
    int opt, port = -1, listener;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                port = atoi(optarg);
                break;
            default:
                fprintf(stderr, "usage : %s port", argv[0]);
                exit(1);
        }
    }

    //assert(get_conf_val(conf, "PORT") != NULL);
    if (port == -1) port = atoi(get_conf_val(conf, "PORT"));
    court.width = atoi(get_conf_val(conf, "COLS"));
    court.height = atoi(get_conf_val(conf, "LINES"));

    court.start.x = COURT_STARTX;
    court.start.y = COURT_STARTY;

    score.team0 = 0;
    score.team1 = 0;

    ball.x = court.width / 2;
    ball.y = court.height / 2;
    ball_status.who - -1;

    if ((listener = socket_create_udp(port)) < 0) {
        perror("socket_create_udp()");
        exit(1);
    }

    DBG("<"GREEN"INFO"NONE"> : server start on port : %d\n", port); 

    int epollfd = epoll_create(MAX_PLAYER * 2);
    repollfd = epoll_create(MAX_PLAYER);
    bepollfd = epoll_create(MAX_PLAYER);

    if (epollfd < 0 || repollfd < 0 || bepollfd < 0) {
        perror("epoll_create()");
        exit(1);
    }

    struct epoll_event ev, events[MAX_PLAYER * 2];

    ev.events = EPOLLIN;
    ev.data.fd = listener;
    
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        perror("epoll_ctl()");
        exit(1);
    }

    struct sockaddr_in client;
    bzero(&client, sizeof(client));
    socklen_t len = sizeof(client);

    for (;;) {
        DBG("<"YELLOW"Main Reactor"NONE"> : waiting for client\n");
        int nfds = epoll_wait(epollfd, events, MAX_PLAYER * 2, -1);
        if (nfds < 0) {
            perror("epoll_wait()");
            exit(1);
        }
        for (int i = 0; i < nfds; i++) {
            s_player player;
            char buff[512];
            if (events[i].data.fd == listener) {
                // recvfrom only
                bzero(buff, sizeof(buff));
                recvfrom(listener, buff, sizeof(buff), 0, (struct sockaddr *)&client, &len);
                DBG("<"RED"Recv From"NONE"> : %s:%d --> %s \n", inet_ntoa(client.sin_addr), htons(client.sin_port), buff);
            }
        }
    }

    return 0;
}

