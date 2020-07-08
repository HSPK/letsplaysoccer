/*************************************************************************
	> File Name: common.c
	> Author: 
	> Mail: 
	> Created Time: Wed 08 Jul 2020 10:04:35 AM CST
 ************************************************************************/

#include "head.h"
#include "common.h"

char *get_conf_val(char *path, char *key) 
{
    FILE *fp;
    size_t nrd, len;
    char *line = (char *)malloc(50);
    char *sub;
    if (path == NULL || key == NULL) {
        fprintf(stderr, "Error in arg!\n");
        return NULL;
    }
    if ((fp = fopen(path, "r")) == NULL) {
        perror("fopen()");
        return NULL;
    }
    while ((nrd = getline(&line, &len, fp)) != -1) {
        if ((sub = strstr(line, key)) == NULL) continue;
        if (line[strlen(key)] == '=') {
            memset(conf_ans, 0, sizeof(conf_ans));
            strncpy(conf_ans, sub + strlen(key) + 1, sizeof(conf_ans));
            free(line);
            return conf_ans;
        }
    }
    free(line);
    return NULL;
}

 int socket_create(int port) {
         int sockfd;
         struct sockaddr_in server;
     if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                 return -1;
             
     }

         server.sin_family = AF_INET;
             server.sin_port = htons(port);
         server.sin_addr.s_addr = htonl(INADDR_ANY);

     int reuse = 1;
     setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,(char *) &reuse, sizeof(int));
     if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
                 return -1;
             
     }

     if (listen(sockfd, 10) < 0) {
                 return -1;
             
     }

         return sockfd;

 }

 void make_non_block(int fd) {
         unsigned long ul = 1;
         ioctl(fd, FIONBIO, &ul);

 }

 void make_block(int fd) {
         unsigned long ul = 0;
         ioctl(fd, FIONBIO, &ul);

 }

