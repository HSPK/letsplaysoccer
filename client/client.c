/*************************************************************************
	> File Name: ./client/client.c
	> Author: 
	> Mail: 
	> Created Time: Wed 08 Jul 2020 03:23:17 PM CST
 ************************************************************************/

#include "head.h"
#include "common.h"
#include "color.h"

int main(int argc, char **argv)
{
    char conf_path[] = "football.conf";
    char name[20] = {0};
    int team = -1;
    char server_ip[20]={0};
    int  server_port = -1;
    char msg[512] = {0};
    

    DBG("<"RED"begin"NONE">\n");
    char opt;
    if (argc != 1) {
    while((opt = getopt(argc, argv, "h:p:n:t:m:")) != -1) {
        //DBG("read opt : %c\n", opt);
         switch (opt) {
            case 'h':
                strcpy(server_ip, optarg);
                break;
            case 'p':
                server_port = atoi(optarg);
                break;
            case 'n':
                strcpy(name, optarg);
                break;
            case 'm':
                strcpy(msg, optarg);
                break;
            case 't':
                team = atoi(optarg);
                break;
            default:
                break;    
        }
    }
    }
    DBG("<"RED"complete info"NONE">\n");
    if (!strlen(server_ip)) strcpy(server_ip, get_conf_val(conf_path, "SERVERIP"));
    if (!strlen(name)) strcpy(name, get_conf_val(conf_path, "NAME"));
    if (!strlen(msg)) strcpy(msg, get_conf_val(conf_path, "MSG"));
    if (team == -1) team = atoi(get_conf_val(conf_path, "TEAM"));
    if (server_port == -1) server_port = atoi(get_conf_val(conf_path, "SERVERPORT"));
    DBG("<"GREEN"ip"NONE">: %s\n", server_ip);
    DBG("<"GREEN"port"NONE">: %d\n", server_port);
    DBG("<"GREEN"name"NONE">: %s\n", name);
    DBG("<"GREEN"team"NONE">: %d\n", team);
    DBG("<"GREEN"msg"NONE">: %s\n", msg);
    return 0;
}
