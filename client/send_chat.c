/*************************************************************************
 
	> File Name: send_chat.c
	> Author: 
	> Mail: 
	> Created Time: Saturday, July 11, 2020 AM09:46:45 CST
 ************************************************************************/

#include "../common/head.h"
#include "send_chat.h"
#include "chat_ui.h"

extern int sockfd;
extern WINDOW *input_sub, *input_win, *message_sub;

void send_chat()
{
    s_chat_msg msg;
    echo();
    nocbreak();
    bzero(&msg, sizeof(msg));
    msg.type = CHAT_WALL;
    w_gotoxy_puts(input_win, 1, 1, "Input Message :");

    //show_message(message_sub, &msg, 1);
    wrefresh(input_win);
    mvwscanw(input_win, 2, 1, "%[^\n]s", msg.msg);
    //show_message(message_sub, &msg, 1);
    if (strlen(msg.msg)) {
        if (msg.msg[0] == '@') msg.type = CHAT_MSG;
        if (msg.msg[0] == '#') msg.type = CHAT_FUNC;
        send(sockfd, (void *)&msg, sizeof(msg), 0);
    }
    wclear(input_win);
    box(input_win, 0 , 0);
    wrefresh(input_win);
    noecho();
    cbreak();
}

