/*************************************************************************
	> File Name: thread_poll.c
	> Author: 
	> Mail: 
	> Created Time: Thu 09 Jul 2020 02:49:21 PM CST
 ************************************************************************/

#include "head.h"
extern int repollfd, bepollfd;
extern s_player *bteam, *rteam;


void sendto_all(s_chat_msg *msg)
{
    //struct sockaddr_in client;
    //socklen_t len;
    for (int i = 0; i < MAX_PLAYER; i++) {
        if (bteam[i].online) {
            //bzero(&client, sizeof(client));
            //getpeername(bteam[i].fd, (struct sockaddr *)&client, &len);
            //sendto(bteam[i].fd, msg, sizeof(s_chat_msg), 0, (struct sockaddr *)&client, len);
            send(bteam[i].fd, msg, sizeof(s_chat_msg), 0);
        }
        if (rteam[i].online) {
            //bzero(&client, sizeof(client));
            //getpeername(rteam[i].fd, (struct sockaddr *)&client, &len);
            //sendto(rteam[i].fd, msg, sizeof(s_chat_msg), 0, (struct sockaddr *)&client, len);
            send(rteam[i].fd, msg, sizeof(s_chat_msg), 0);
        }
    }
}
void find_online(int fd) {
    s_chat_msg buff;
    sprintf(buff.msg, "online people is:\n");
    buff.type = CHAT_FUNC;
    strcpy(buff.from, "Server Info");
    //send(fd, &buff, sizeof(s_chat_msg), 0);
    s_player *team[] = {bteam, rteam};3;
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < MAX_PLAYER; i++) {
             if (team[j][i].online) {
                //buff.type = CHAT_SYS;
                strcat(buff.msg,team[j][i].name);
                strcat(buff.msg,"\t");
               // sprintf(buff.msg, "%s",team[j][i].name);
            }
        }
    }

    strcat(buff.msg,"\n");
    
    send(fd, &buff, sizeof(s_chat_msg), 0);
    return ;
}
void sendto_single(int fd, s_chat_msg *msg, char *name)
{
    //struct sockaddr_in client;
    s_chat_msg buff;
    bzero(&buff, sizeof(buff));
    //socklen_t len;

    s_player *team[] = {bteam, rteam};3;

    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < MAX_PLAYER; i++) {
            if (strcmp(team[j][i].name, name) == 0) {
                if (team[j][i].online) {
                    //getpeername(team[j][i].fd, (struct sockaddr *)&client, &len);
                    //sendto(team[j][i].fd, msg, sizeof(s_chat_msg), 0, (struct sockaddr *)&client, len);
                    send(team[j][i].fd, msg, sizeof(s_chat_msg), 0);
                    send(fd, msg, sizeof(s_chat_msg), 0);
                    return;
                } else {
                    buff.type = CHAT_MSG;
                    //strcpy(buff.from, "Server Info");
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

void display(const char *path)
{
    FILE *fp = fopen(path, "r");
    
    if (fp == NULL) {
        perror("fopen()");
        exit(1);
    }

    s_chat_msg buff;
    bzero(&buff, sizeof(buff));
    buff.type = CHAT_FUNC;
    while (fgets(buff.msg, sizeof(buff.msg), fp) != NULL) {
        sendto_all(&buff);
        bzero(buff.msg, sizeof(buff.msg));
    }
}

void do_work(struct Player *player) {

    DBG("<"BLUE"In do work"NONE"> %s \n", player->name);
    int fd = player->fd;
    s_chat_msg msg;
    bzero(&msg, sizeof(msg));
    s_chat_msg buff;
    bzero(&buff, sizeof(buff));
    const char suantou_path[] = "../common/suantou.txt";


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
        
        while (msg.msg[i] != ' ' && i < 20) {
            name[i - 1] = msg.msg[i];
            i++;
        }
        name[i] = '\n';
        while (msg.msg[i] == ' ' && i < 1024) i++;
        
        buff.type = CHAT_MSG;
        strcpy(buff.from, msg.from);
        strcpy(buff.msg, msg.msg + i);
        
        printf("<%s - %s > ~ %s\n", msg.from, name, buff.msg);
        
        sendto_single(fd, &buff, name);
    } else if (msg.type & CHAT_FIN) {
        //s_player *team = player->team ? bteam : rteam;
        //for (int i = 0; i < MAX_PLAYER; i++) {
        //    if (team[i].online && strcmp(team[i].name, player->name) == 0) {
        //        team[i].online = 0;
        //        break;
        //    }
        //}
        player->online = 0;
        int epollfd = player->team ? bepollfd : repollfd;
        del_event(epollfd, player->fd);
        printf("<"YELLOW"Server Info"NONE"> : %s log out\n", player->name);
        buff.type = CHAT_SYS;
        sprintf(buff.from, "Server Info");
        sprintf(buff.msg, "%s log out\n", player->name);
        sendto_all(&buff);
    } else if (msg.type & CHAT_FUNC) {
        //#1
        int cmd = atoi(msg.msg + 1);
        switch (cmd) {
            case 1:
                buff.type = CHAT_FUNC;
                sprintf(buff.from, "Server Info");
                find_online(fd);
                break;
            case 2:
                buff.type = CHAT_WALL;
                strcpy(buff.from, player->name);
                sprintf(buff.msg, "little suan");
                sendto_all(&buff);
                display(suantou_path);
                break;
            default:
                buff.type = CHAT_MSG;
                sprintf(buff.from, "Server Info");
                sprintf(buff.msg, "Invaild Command");
                send(fd, &buff, sizeof(buff), 0);
                break;
        }
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

