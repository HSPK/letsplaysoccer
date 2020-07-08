/*************************************************************************
	> File Name: common.h
	> Author: 
	> Mail: 
	> Created Time: Mon 06 Jul 2020 04:37:05 PM CST
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

char conf_ans[50];
char* get_conf_val(char *path, char *key);
int socket_create(int port);
void make_non_block(int fd);
void make_block(int fd);

#ifdef _D
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#endif
