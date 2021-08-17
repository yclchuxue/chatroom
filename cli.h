#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <assert.h>
#include <libgen.h>//basename():从路径中获取文件名及后缀


#define OPEN_MAX 1024

typedef struct xinxi{
	int ice_1;              //功能选择
	int ice_2;
	int ice_3;
    int ice_4;
	int m_id;               //客户id
	int y_id;               //好友id
	int q_id;              //群id
    int fd;                //套接字码
	int zt;                //状态
	char name[20];         //name
	char password[16];     //密码
	char qu[200];          //问题
	char an[100];          //答案
	char beizhu[20];      //备注	
    char hylb[20];         //好友列表
    char cylb[20];         //成员列表
    char jl[20];           //聊天记录
	char buf[200];       //信息内容
}XINXI;

typedef struct denn{
	int ice;               //功能选择
	int id;                //id
	int zt;                //状态
	char name[20];         //name
	char password[16];     //密码
	char qu[200];          //问题
	char an[100];          //答案
	char hylb[20];         //好友列表
}DENN;

typedef struct liaot{
	int ice;
	int id;
	int zt;
    char name[20];
	char qu[200];
	char an[100];
	char buf[50];
	char beizhu[20];
    char xinxi[200];
}LIAOT;

typedef struct E_list{
    int index;
    int efd;
    int sum;
    struct epoll_event tep, ep[OPEN_MAX];
    struct E_list *next;
}E_LIST;

typedef struct Doc{
	int sign; //
	char buf[1024];
}DOC;

int Socket_fd, Y_ID, M_ID, Q_ID, M;

void C_denn(XINXI *YY, int socket_fd);

void C_zhuce(XINXI *YY, int socket_fd);

void C_zhaohui(XINXI *YY, int socket_fd);

void C_haoy(XINXI *YY, DENN *XX, int socket_fd);

void C_haoyouliaot(XINXI *YY, DENN *XX, int socket_fd);

void C_group(XINXI *YY, DENN *XX, int socket_fd);

void C_group_com(XINXI *YY, DENN *XX, int socket_fd);

int C_TongZ(XINXI *YY, DENN *XX, int socket_fd);

void C_get(int socket_fd);

void C_document(XINXI *YY, int socket_fd);

void get_XX(int socket_fd);

void *thread(void *arg);

void *thread_g(void *arg);

void face(XINXI *YY);