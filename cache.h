
#ifndef CACHE
#define CACHE

#define _GNU_SOURCE
#include "tree.h"
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<assert.h>
#include<errno.h>
#include<stddef.h>
#include<stdarg.h>
#include<ctype.h>

#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>

#define HOST "127.0.0.1"
#define PORT "12000"

// #define log(x,args...) 
//     printf(x _VA_OPT_(,) args)

// #define log(x, ...) 
//     printf(x, ##__VA_ARGS__)


typedef unsigned int int32;
typedef unsigned short int int16;
typedef unsigned char int8;



typedef struct s_client {
    
    int s;
    char ip[16];
    int16 port;

}Client;

typedef int32 (*Callback)(Client*,int8*,int8*);

struct s_cmdhandler{
    int8* cmd;
    Callback handler;
};
typedef struct s_cmdhandler CmdHandler;



// void zero(int8*,int16);
void childloop(Client*);
void mainloop(int s);
int init_server(int16);
int main(int,char**);



#endif