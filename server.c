#include "ser.h"


int main()
{
    char A[100];
    conn = mysql_init(conn);    //初始化一个句柄
    mysql_library_init(0,NULL,NULL);   //初始化数据库

    if(conn == NULL)
    {
        printf("mysql_init failed!!!\n");
        exit(1);
    }
    conn = mysql_real_connect(conn,"127.0.0.1","ycl","121819","db1",0,NULL,0);  //连接数据库
    mysql_set_character_set(conn,"utf8");      //调整为中文字符

    if(conn == NULL)
    {
        printf("mysql_real_connect failed!!!\n");
        exit(1);
    }

    sprintf(A, "update student set zt=0");      //服务器重启，所有用户都不在线
    mysql_query(conn,A);
    sprintf(A, "update student set fd=0");      //服务器重启，所有用户都不在线
    mysql_query(conn,A);
    sprintf(A, "update student set sj=0");      //服务器重启，所有用户都不在线
    mysql_query(conn,A);

    //创建epoll线程
    for(int i = 0;i < 5; i++)
    {
        EPOL[i].sum = 0;
        E_LIST *p = &EPOL[i];
        EPOL[i].efd = epoll_create(OPEN_MAX);
        pthread_create(&thid[i], NULL, thread_account, p);
    }



	int cfd, lfd, sfd, ret, n, i, maxi, efd, nready, PD;
	struct sockaddr_in caddr, saddr;
	char buf[50], str[INET_ADDRSTRLEN];           //缓存区
	socklen_t clen = sizeof(struct sockaddr_in);;

	bzero(&saddr, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(9999);               //端口号9999
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);      //TCP

	lfd = socket(AF_INET, SOCK_STREAM, 0);            //创建套接字
    int mw_optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, (char *)&mw_optval,sizeof(mw_optval));
	bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));    //绑定端口
	listen(lfd, 128);                //设置监听套接字


    int j = 0;
    Lock = 0;
    while(1)
    {
                //printf("AAAAAAAAA\n");

                cfd = accept(lfd, (struct sockaddr *)&caddr, &clen);        //连接套接字

                printf("connection--port:%d\tip:%s\n", ntohs(caddr.sin_port), inet_ntop(AF_INET, &caddr.sin_addr.s_addr, str, sizeof(str)));
                //printf("cfd = %d\n", cfd);
                time_t timep_1;
                time(&timep_1);
                struct tm *p;
                p = gmtime(&timep_1);
                sprintf(A, "%4d/%2d/%2d %2d/%2d/%2d\t connection--port：%d\tip:%s\tclient_fd:%d\n",p->tm_year + 1900,p->tm_mon + 1,p->tm_mday
                ,p->tm_hour + 8,p->tm_min,p->tm_sec,ntohs(caddr.sin_port), inet_ntop(AF_INET, &caddr.sin_addr.s_addr, str, sizeof(str)),cfd);
                int fd = open("server_log.txt", O_WRONLY | O_APPEND);
                write(fd, A, strlen(A));
                close(fd);

                if(j == 5)
                {
                    j = 0;
                }
                EPOL[j].tep.events = EPOLLIN;
                EPOL[j].tep.data.fd = cfd;
                EPOL[j].sum++;
                //EPOL[j].efd = cfd;
                //printf("sfd = %d\n", cfd);
                epoll_ctl(EPOL[j].efd, EPOLL_CTL_ADD, cfd, &(EPOL[j].tep));     //将监听套接字加入等待队列
                j++;
    }

    //printf("BBB\n");


    close(lfd);
    return 0;
}


