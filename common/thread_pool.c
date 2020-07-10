/*************************************************************************
	> File Name: thread_poll.c
	> Author: 
	> Mail: 
	> Created Time: Thu 09 Jul 2020 02:49:21 PM CST
 ************************************************************************/

#include "head.h"
extern int repollfd, bepollfd;
extern s_player *bteam, *rteam;

void do_work(struct Player *player) {
    DBG("<"BLUE"In do work"NONE"> %s \n", player->name);
    int fd = player->fd;
    s_chat_msg msg;
    bzero(&msg, sizeof(msg));
    if (recv(fd, &msg, sizeof(msg), 0) < 0) {
        perror("recv()");
        exit(1);
    }
    if (msg.type & CHAT_WALL) {
        printf("<"GREEN"%s"NONE"> : %s \n", player->name, msg.msg);
    } else if (msg.type & CHAT_MSG) {
        printf("<%s> $ %s \n", player->name, msg.msg);
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
    send(fd, msg.msg, strlen(msg.msg), 0);
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

