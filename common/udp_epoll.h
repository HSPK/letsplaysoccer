/*************************************************************************
	> File Name: udp_epoll.h
	> Author: 
	> Mail: 
	> Created Time: Thu 09 Jul 2020 04:40:50 PM CST
 ************************************************************************/

#ifndef _UDP_EPOLL_H
#define _UDP_EPOLL_H

int udp_connect(struct sockaddr_in *client);
int udp_accept(int fd, s_player *player);
void del_event(int epollfd, int fd);
void add_to_sub_reactor(s_player *player);

#endif
