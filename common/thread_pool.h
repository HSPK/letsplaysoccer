/*************************************************************************
	> File Name: thread_poll.h
	> Author: 
	> Mail: 
	> Created Time: Thu 09 Jul 2020 02:49:10 PM CST
 ************************************************************************/

#ifndef _THREAD_POLL_H
#define _THREAD_POLL_H

typedef struct task_queue {
    int sum;
    int epollfd; //从反应堆
    struct Player **team;
    int head;
    int tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}s_task_queue;

void task_queue_init(struct task_queue *taskQueue, int sum, int epollfd);
void task_queue_push(struct task_queue *taskQueue, struct Player *player);
struct Player *task_queue_pop(struct task_queue *taskQueue);

void *thread_run(void *arg);

#endif
