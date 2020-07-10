/*************************************************************************
	> File Name: thread_poll.c
	> Author: 
	> Mail: 
	> Created Time: Thu 09 Jul 2020 02:49:21 PM CST
 ************************************************************************/

#include "head.h"
extern int repollfd, bepollfd;
extern s_player *bteam, *rteam;

void sendto_all(s_chat_msg *msg);

void logout(int signum)
{
    s_chat_msg msg;
    bzero(&msg, sizeof(msg));
    msg.type = CHAT_FIN;
    strcpy(msg.from, "Server Info");
    strcpy(msg.msg, "Thank you for using ! :)");
    sendto_all(&msg);
    exit(0);
}

void sendto_all(s_chat_msg *msg)
{
    struct sockaddr_in client;
    socklen_t len;
    for (int i = 0; i < MAX_PLAYER; i++) {
        if (bteam[i].online) {
            bzero(&client, sizeof(client));
            getpeername(bteam[i].fd, (struct sockaddr *)&client, &len);
            sendto(bteam[i].fd, msg, sizeof(s_chat_msg), 0, (struct sockaddr *)&client, len);
        }
        if (rteam[i].online) {
            bzero(&client, sizeof(client));
            getpeername(rteam[i].fd, (struct sockaddr *)&client, &len);
            sendto(rteam[i].fd, msg, sizeof(s_chat_msg), 0, (struct sockaddr *)&client, len);
        }
    }
}

void sendto_single(int fd, s_chat_msg *msg, char *name)
{
    struct sockaddr_in client;
    s_chat_msg buff;
    bzero(&buff, sizeof(buff));
    socklen_t len;

    s_player *team[] = {bteam, rteam};

    for (int j = 0; j < 2; j++) {
    for (int i = 0; i < MAX_PLAYER; i++) {
        if (strcmp(team[j][i].name, name) == 0) {
            if (team[j][i].online) {
                getpeername(team[j][i].fd, (struct sockaddr *)&client, &len);
                sendto(team[j][i].fd, msg, sizeof(s_chat_msg), 0, (struct sockaddr *)&client, len);
                send(fd, msg, sizeof(s_chat_msg), 0);
                return;
            } else {
                buff.type = CHAT_MSG;
                strcpy(buff.from, "Server Info");
                sprintf(buff.msg, "%s is not online !", name);
                send(fd, &buff, sizeof(s_chat_msg), 0);
                return;
            }
        }
    }
    }
    buff.type = CHAT_MSG;
    sprintf(buff.from, "Server Info");
    sprintf(buff.msg, "%s is not logged in !", name);
    send(fd, &buff, sizeof(s_chat_msg), 0);
}

void do_work(struct Player *player) {

    DBG("<"BLUE"In do work"NONE"> %s \n", player->name);
    int fd = player->fd;
    s_chat_msg msg;
    bzero(&msg, sizeof(msg));
    s_chat_msg buff;
    bzero(&buff, sizeof(buff));
    
    if (recv(fd, &msg, sizeof(msg), 0) < 0) {
        perror("recv()");
        exit(1);
    }

    if (msg.type & CHAT_WALL) {
        printf("<"GREEN"%s"NONE"> : %s \n", player->name, msg.msg);
        sendto_all(&msg);
    } else if (msg.type & CHAT_MSG) {
        char name[20] = {0};
        int i = 1;
        
        if (msg.msg[0] != '@') {
            buff.type = CHAT_MSG;
            sprintf(buff.from, "Server Info");
            sprintf(buff.msg, "Check your fomate Please ! ");
            send(fd, &buff, sizeof(buff), 0);
            return;
        }
        
        while (msg.msg[i] != ' ') {
            name[i - 1] = msg.msg[i];
            i++;
        }
        name[i] = '\n';
        while (msg.msg[i] == ' ') i++;
        
        buff.type = CHAT_MSG;
        strcpy(buff.from, msg.from);
        strcpy(buff.msg, msg.msg + i);
        
        printf("<%s> $ %s : %s\n", msg.from, player->name, buff.msg);
        
        sendto_single(fd, &buff, name);
    } else if (msg.type & CHAT_FIN) {
        s_player *team = player->team ? bteam : rteam;
        for (int i = 0; i < MAX_PLAYER; i++) {
            if (team[i].online && strcmp(team[i].name, player->name) == 0) {
                team[i].online = 0;
                break;
            }
        }
        int epollfd = player->team ? bepollfd : repollfd;
        del_event(epollfd, player->fd);
        printf("<"YELLOW"Server Info"NONE"> : %s log out\n", player->name);
    }

    DBG("<"RED"Recv"NONE"> : %s\n", msg.msg);
    //send(fd, msg.msg, strlen(msg.msg), 0);
    DBG("<"RED"Send"NONE"> : %s\n", msg.msg);
}

void task_queue_init(struct task_queue *taskQueue, int sum, int epollfd)
{
    taskQueue->sum = sum;
    taskQueue->epollfd = epollfd;
    taskQueue->team = calloc(sum, sizeof(void *));
    taskQueue->head = taskQueue->tail = 0;    

    pthread_mutex_init(&taskQueue->mutex, NULL);
    pthread_cond_init(&taskQueue->cond, NULL);
}

void task_queue_push(struct task_queue *taskQueue, struct Player *player)
{
    pthread_mutex_lock(&taskQueue->mutex);
    taskQueue->team[taskQueue->tail] = player;
    DBG("<"L_GREEN"Thread Pool"NONE"> : Task push %s \n", player->name);
    if (++taskQueue->tail == taskQueue->sum) {
        taskQueue->tail = 0;
        DBG("<"L_GREEN"Thread Pool"NONE"> : Task Queue End \n");
    }
    pthread_cond_signal(&taskQueue->cond);
    pthread_mutex_unlock(&taskQueue->mutex);
}

struct Player *task_queue_pop(struct task_queue *taskQueue)
{
    pthread_mutex_lock(&taskQueue->mutex);
    while (taskQueue->tail == taskQueue->head) {   
        DBG("<"L_GREEN"Thread Pool"NONE"> : Task Queue Empty, Waiting for Task \n");
        pthread_cond_wait(&taskQueue->cond, &taskQueue->mutex);

    }
    struct Player *player = taskQueue->team[taskQueue->head];
    DBG("<"L_GREEN"Thread Pool"NONE"> : Task Pop %s \n", player->name);
    if (++taskQueue->head == taskQueue->sum) {
        taskQueue->head = 0;
        DBG("<"L_GREEN"Thread Pool"NONE"> : Task Queue End \n");
    }
    pthread_mutex_unlock(&taskQueue->mutex);

    return player;
}

void *thread_run(void *arg)
{
    pthread_detach(pthread_self());
    struct task_queue *taskQueue = (struct task_queue *)arg;
    while (1) {
        struct Player *player = task_queue_pop(taskQueue);
        do_work(player);
    }
}

