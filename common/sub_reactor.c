/*************************************************************************
    > File Name: sub_reactor.c
    > Author: 
    > Mail: 
    > Created Time: Thu 09 Jul 2020 03:44:09 PM CST
 ************************************************************************/

#include "head.h"

void *sub_reactor(void *arg)
{
    struct task_queue *taskQueue = (struct task_queue *)arg;

    pthread_t *tid = (pthread_t *)calloc(NTHREAD, sizeof(pthread_t));

    for (int i = 0; i < NTHREAD; i++) {
        pthread_create(&tid[i], NULL, thread_run, (void *)taskQueue);
    }

    struct epoll_event ev, events[MAX_PLAYER];
    while (1) {
        DBG("<"L_RED"Sub Reactor"NONE"> : epoll waiting...\n");
        int nfds = epoll_wait(taskQueue->epollfd, events, MAX_PLAYER, -1);
        if (nfds < 0) {
            perror("epoll_wait()");
            exit(1);
        }
        for (int i = 0; i < nfds; i++) {
            struct Player *player = (struct Player *)events[i].data.ptr;
            DBG("<"L_RED"Sub Reactor"NONE"> : %s Ready\n", player->name);
            if (events[i].events & EPOLLIN) {
                task_queue_push(taskQueue, player);
            }
        }
    }

    return NULL;
}
