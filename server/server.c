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

struct Player *rteam, *bteam;
int port = -1;


void logout(int signum)
{
    s_chat_msg msg;
    bzero(&msg, sizeof(msg));
    msg.type = CHAT_FIN;
    strcpy(msg.from, "Server Info");
    strcpy(msg.msg, "Thank you for using ! :)");
    sendto_all(&msg);
    printf("<"YELLOW"Server Info"NONE"> server closed");
    exit(0);

}

int main(int argc, char **argv) 
{
    int opt, listener;

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
    
    rteam = (struct Player *)calloc(MAX_PLAYER, sizeof(struct Player));
    bteam = (struct Player *)calloc(MAX_PLAYER, sizeof(struct Player));

    int epollfd = epoll_create(MAX_PLAYER * 2);
    repollfd = epoll_create(MAX_PLAYER);
    bepollfd = epoll_create(MAX_PLAYER);

    if (epollfd < 0 || repollfd < 0 || bepollfd < 0) {
        perror("epoll_create()");
        exit(1);
    }

    s_task_queue redQueue;
    s_task_queue blueQueue;
    task_queue_init(&redQueue, MAX_PLAYER, repollfd);
    task_queue_init(&blueQueue, MAX_PLAYER, bepollfd);
    
    pthread_t red_t, blue_t;
    pthread_create(&red_t, NULL, sub_reactor, &redQueue);
    pthread_create(&blue_t, NULL, sub_reactor, &blueQueue);

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
    
    signal(SIGINT, logout);
    for (;;) {
        DBG("<"YELLOW"Main Reactor"NONE"> : waiting for client\n");
        int nfds = epoll_wait(epollfd, events, MAX_PLAYER * 2, -1);
        if (nfds < 0) {
            perror("epoll_wait()");
            exit(1);
        }
        for (int i = 0; i < nfds; i++) {
            s_player player;
            bzero(&player, sizeof(player));
            if (events[i].data.fd == listener) {
                int new_fd = udp_accept(listener, &player);
                if (new_fd > 0) {
                    add_to_sub_reactor(&player);
                }
            }
        }
    }

    return 0;
}


