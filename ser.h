#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mariadb/mysql.h>
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
#include <errno.h>
#include <libgen.h>//basename():从路径中获取文件名及后缀

MYSQL *conn;
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

typedef struct Doc{
	int sign; //
	char buf[1024];
}DOC;

typedef struct E_list{
    int index;
    int efd;
    int sum;
    struct epoll_event tep, ep[OPEN_MAX];
    struct E_list *next;
}E_LIST;


E_LIST EPOL[20];
pthread_t thid[20];
int S_ID,E_ID,S_FD,Lock;

int  denglu(XINXI *YY,int sfd);            //登陆

int zhuce(XINXI *YY,int sfd);        //注册

int zhaohui(XINXI *YY,int sfd);     //找回密码

int zhaohui_1(XINXI *YY,int sfd);     //找回密码

void liaotian(DENN *XX, LIAOT *XZ,int sfd);

int xuanzhe_1(DENN *XX, XINXI *YY, int sfd);    //好友管理

int xuanzhe_2(DENN *XX, XINXI *YY, int sfd);    //好友聊天

int group_1(DENN *XX, XINXI *YY, int sfd);      //群管理

int group_2(DENN *XX, XINXI *YY, int sfd);      //群聊天

void G_send(XINXI *YY, int sfd);

void G_get(XINXI *YY, int sfd);

int ADD(XINXI *YY, DENN *XX) ;

int TongZ(XINXI *YY, int sfd);    //添加好友,加群通知

void HY_get(XINXI *YY, int id, int sfd);         //epoll收发信息

void HY_send(XINXI *YY, int sfd);                       //将未读信息发送给客户端

int G_ADD(XINXI *YY, DENN *XX);

void G_ADD_1(XINXI *YY,int sfd);

void getDENN(DENN *XX, int id, int sfd);

void logof(XINXI *YY, int sfd);

void get_TZ(XINXI *YY, int sfd);

int set_1(int id);

int drop_1(int id);

int panduan_1(int id);                  //判断客户是否空闲

int panduan_2(int id);

void TX(XINXI *YY);

void Document(XINXI *YY, int sfd, E_LIST *p);

void login(XINXI *YY, int sfd);

void sort();

void *thread_account(void *arg);

char *getname_from_id(int id);

char *getjl_from_id(int id_I, int id_II);

int getid_from_beizhu(int id,char *beizhu);

char *getbeizhu_from_id(int m_id,int id);

char *getgroupname_from_id(int id);

char *getgroupjl_from_id(int id);