/*************************************************************************
	> File Name: ../common/datatype.h
	> Author: 
	> Mail: 
	> Created Time: Thursday, July 09, 2020 AM10:09:44 CST
 ************************************************************************/

#ifndef _DATATYPE_H
#define _DATATYPE_H


#define CHAT_FIN 0x01
#define CHAT_HEART 0x02
#define CHAT_ACK 0x04
#define CHAT_WALL 0x08    //公聊
#define CHAT_MSG 0x10     //私聊
#define CHAT_FUNC 0x100  //功能

typedef struct ChatMsg {
    int type;
    char from[20];
    char to[20];
    char msg[1024];
}s_chat_msg;

//ball
typedef struct Point{
    double x;
    double y;
}s_point;

typedef struct Speed{
    double vx;
    double vy;
}s_speed;

typedef struct Accel{
    double ax;
    double ay;
}s_accel;

typedef struct Bstatus{
    s_point p;
    s_speed v;
    s_accel a;
    int who;
    char name[20];
}s_Bstatus;

typedef struct Score{
    int team0;
    int team1;
}s_score;

//player

#define MAX_PLAYER 300  //用户数的二分之一
#define NTHREAD 3

typedef struct Player{
    int team;
    int fd;
    char name[20];
    int online;   
    int flag;
    s_point p;
}s_player;

//login

typedef struct LogRequest{
    char name[20];
    int team;
    char msg[512];
}s_log_request;

typedef struct LogResponse{
    int type;
    char msg[512];
}s_log_response;

//during the game
#define MAX_MSG 4096

#define ACTION_KICK 0x01
#define ACTION_CARRY 0x02
#define ACTION_STOP 0x04

typedef struct Ctl{
    int action;
    int dx;
    int dy;
    int strength;    //踢球力度
}s_ctl;

#define FT_HEART 0x01   //心跳
#define FT_WALL 0x02    //公告
#define FT_MSG 0x04
#define FT_ACK 0x08
#define FT_CTL 0x10
#define FT_GAME 0x20    //场景数据
#define FT_SCOER 0x40   //比分变化
#define FT_GAMEOVER 0x80
#define FT_FIN 0x100    //离场

typedef struct FootballMsg{
    int type;
    int size;
    int team;
    char name[20];
    char msg[MAX_MSG];
    s_ctl ctl;
}s_football_msg;

typedef struct Map{
    int width;
    int height;
    s_point start;
    int gate_width;
    int gate_height;
}s_map;

#endif
