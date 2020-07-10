/*************************************************************************
	> File Name: ./client/client.c
	> Author: 
	> Mail: 
	> Created Time: Wed 08 Jul 2020 03:23:17 PM CST
 ************************************************************************/

#include "../common/head.h"

int sockfd;
s_log_response response;
s_log_request request;

void *work(void *arg)
{
    int fd;
    fd = *(int *)arg;
    s_chat_msg msg;
    while (1) {
        bzero(&msg, sizeof(msg));
        if (recv(fd, &msg, sizeof(msg), 0) < 0) {
            perror("recv()");
            exit(1);
        }
        if (msg.type & CHAT_WALL) {
            if (strcmp(msg.from, request.name) == 0) {
                printf("<"GREEN"%s"NONE"> : %s \n", msg.from, msg.msg);
            } else {
                printf("<"BLUE"%s"NONE"> : %s \n", msg.from, msg.msg);
            }  
        } else if (msg.type & CHAT_MSG) {

        } else {
            printf("<"RED"Error"NONE"> : unsopported msg type ! \n");
        }
    }
}

void logout(int signum) 
{
    s_chat_msg msg;
    msg.type = CHAT_FIN;
    send(sockfd, &msg, sizeof(msg), 0);
    close(sockfd);
    exit(0);
}

int main(int argc, char **argv)
{
    char conf_path[] = "football.conf";
    char server_ip[20]={0};
    int  server_port = -1;

    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));
    request.team = -1;

    DBG("<"RED"begin"NONE">\n");
    char opt;
    while((opt = getopt(argc, argv, "h:p:n:t:m:")) != -1) {
        //DBG("read opt : %c\n", opt);
         switch (opt) {
            case 'h':
                strcpy(server_ip, optarg);
                break;
            case 'p':
                server_port = atoi(optarg);
                break;
            case 'n':
                strcpy(request.name, optarg);
                break;
            case 'm':
                strcpy(request.msg, optarg);
                break;
            case 't':
                request.team = atoi(optarg);
                break;
            default:
                fprintf(stderr, "usage : %s [-hpnmt]", argv[0]);
                exit(1);
                break;    
        }
    }
    DBG("<"RED"complete info"NONE">\n");
    if (!strlen(server_ip)) strcpy(server_ip, get_conf_val(conf_path, "SERVERIP"));
    if (!strlen(request.name)) strcpy(request.name, get_conf_val(conf_path, "NAME"));
    if (!strlen(request.msg)) strcpy(request.msg, get_conf_val(conf_path, "MSG"));
    if (request.team == -1) request.team = atoi(get_conf_val(conf_path, "TEAM"));
    if (server_port == -1) server_port = atoi(get_conf_val(conf_path, "SERVERPORT"));
    int slen = strlen(request.name);
    if (request.name[slen - 1] == '\n') request.name[slen - 1] = '\0';
    slen = strlen(request.msg);
    if (request.msg[slen - 1] == '\n') request.msg[slen - 1] = '\0';
    DBG("<"GREEN"ip"NONE">: %s\n", server_ip);
    DBG("<"GREEN"port"NONE">: %d\n", server_port);
    DBG("<"GREEN"name"NONE">: %s\n", request.name);
    DBG("<"GREEN"team"NONE">: %d\n", request.team);
    DBG("<"GREEN"msg"NONE">: %s\n", request.msg);

    struct sockaddr_in server;
    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip);

    socklen_t len = sizeof(server);

    if ((sockfd = socket_udp()) < 0) {
        perror("socket_udp()");
        exit(1);
    }
    
    if (connect(sockfd, (struct sockaddr *)&server, len) < 0) {
        perror("connect()");
        exit(1);
    }
 
    send(sockfd, (void *)&request, sizeof(request), 0);
    int nrcv;
    if ((nrcv = recv(sockfd, (void *)&response, sizeof(response), 0)) != sizeof(response)) {
        perror("recv()");
        exit(1);
    }
    if (response.type == 1) {
        fprintf(stderr, "login failed : %s\n", response.msg);
        exit(1);
    }
    printf("%s\n", response.msg);

    pthread_t tid;
    if (pthread_create(&tid, NULL, work, &sockfd) < 0) {
        perror("pthread_create()");
        exit(1);
    }

    signal(SIGINT, logout);
    while (1) {
        s_chat_msg msg;
        bzero(&msg, sizeof(msg));
        msg.type = CHAT_WALL;
        strcpy(msg.from, request.name);
        printf("Input msg:\n");
        scanf("%[^\n]s", msg.msg);
        getchar();
        send(sockfd, &msg, sizeof(msg), 0);
        printf("send: %s\n", msg.msg);
    }
    return 0;
}
