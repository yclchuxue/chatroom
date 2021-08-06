#include "ser.h"

void Document(XINXI *YY, int sfd, E_LIST *p)
{
    int ret, fd, dfd;
	char A[100];
    char file_len[16], file_name[128], buf[1024],file_new_name[128],sign[10];

    if(YY->ice_2 == 71)        //接收来自客户端的文件
    {   
        strncpy(file_len, YY->buf, sizeof(file_len));
        strncpy(file_name, YY->qu, sizeof(file_name));

        //printf("文件名：%s\n文件大小：%s\n", file_name, file_len);

        sprintf(file_new_name, "./document/recv-%s", file_name);
        //printf("%s", file_new_name);

        fd = open(file_new_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
        
        //printf("fd = %d\n", fd);

        int size = atoi(file_len);

        int write_len = 0, s_size = 1024,r_ice = 1;
        DOC *D = (DOC*)malloc(sizeof(DOC));
        while(1)
        {
            memset(D, 0, sizeof(DOC));
            recv(sfd, D, sizeof(DOC), 0);
            printf("D->sign = %d\n", D->sign);
            printf("r_ice = %d\n", r_ice);
            if(D->sign == r_ice)
            {
                if(write_len + 1024 > size)
                {
                    s_size = size - write_len;
                }
                ret = write(fd, D->buf, s_size);
                write_len += ret;
                ++r_ice;
                sprintf(sign, "ok");
                printf("file_len = %d\nwrite_len = %d\n", size, write_len);
            }
            else
            {
                sprintf(sign, "no");
            }
            send(sfd, sign, sizeof(sign), 0);
            if(write_len >= size)
            {
                close(fd);
                break;
            }
        }

        printf("file_len = %d\nwrite_len = %d\n", size, write_len);
		sprintf(A, "insert into document (from_id, end_id, file_name) values (%d, %d, '%s')", YY->m_id, YY->y_id, file_new_name);
		mysql_query(conn,A);
    }
    else if(YY->ice_2 == 72)   //向客户端发送文件
    {
		int sum = 0;
		sprintf(A, "select file_name from document where  from_id = %d and end_id = %d", YY->y_id, YY->m_id);
		mysql_query(conn,A);
		MYSQL_RES *res_ptr;
    	MYSQL_ROW  res_row;
		res_ptr = mysql_store_result(conn);
		while(res_row = mysql_fetch_row(res_ptr))
		{
			sum++;
		}
		write(sfd, &sum, sizeof(int));

        if(sum <= 0)
        {
            return ;
        }

		sprintf(A, "select file_name from document where  from_id = %d and end_id = %d", YY->y_id, YY->m_id);
		mysql_query(conn,A);
		res_ptr = mysql_store_result(conn);
		while(res_row = mysql_fetch_row(res_ptr))
		{
			LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
			strncpy(file_name, (char *)res_row[0], sizeof(file_name));
			fd = open(file_name, O_RDWR);
			if(fd == -1)
			{
				printf("打开文件失败！！！\n");
				return ;
			}

			int len = lseek(fd, 0, SEEK_END);

			lseek(fd, 0, SEEK_SET);   //文件光标移动到开始位置

			strncpy(file_name, basename(file_name), sizeof(file_name));
			strncpy(XZ->xinxi, file_name, sizeof(file_name));
			sprintf(XZ->beizhu, "%d", len);
			//printf("len = %s\nname = %s\n",XZ->beizhu, XZ->xinxi);
			send(sfd, XZ, sizeof(LIAOT), 0);

			int send_len = 0;       //记录发送的字节数

            int s_ice = 1;

            DOC *D = (DOC*)malloc(sizeof(DOC));

			while(1)
			{
				memset(D, 0, sizeof(DOC));
                memset(sign, 0, sizeof(sign));
                ret = read(fd, D->buf, sizeof(D->buf));
                
                if(ret <= 0)
                {
                    close(fd); 
                    printf("\t\t文件%s发送成功！！！\n", file_name);
                    break;
                }
                D->sign = s_ice;
                
                send(sfd, D, sizeof(DOC), 0);

                recv(sfd, sign, sizeof(sign), 0);
                printf("sign = %s\n", sign);

                if(strcmp(sign, "ok") == 0)
                {
                    printf("\t\tlen = %d\n\t\tsend_len = %d\n", len, send_len);
                    send_len += ret;
                    ++s_ice;
                }
                else if(strcmp(sign, "no") == 0)
                {
                    while(1)
                    {
                        send(sfd, D, sizeof(DOC), 0);

                        recv(sfd, sign, sizeof(sign), 0);

                        if(strcmp(sign, "ok") == 0)
                        {
                            printf("\t\tlen = %d\n\t\tsend_len = %d\n", len, send_len);
                            send_len += ret;
                            ++s_ice;
                            break;
                        }
                    }
                }
                else
                {
                    exit(-1);
                }
                if(send_len == len)
                {
                    close(fd);
                    printf("\t\t文件%s发送成功!!!\n", file_name);
                    break;
                }
			}
            
			sprintf(A, "delete from document where from_id = %d and end_id = %d and file_name = '%s'", YY->y_id, YY->m_id, res_row[0]);
			//printf("A = %s\n", A);
			mysql_query(conn,A);

			free(XZ);
		}
    }

}

void login(XINXI *YY, int sfd)
{
    char A[100];
    time_t timep_1;
    time(&timep_1);
    struct tm *p;
    p = gmtime(&timep_1);
    memset(A, 0, sizeof(A));
    if(YY->ice_1 < 0 || YY->ice_1 > 1000)
    {
        YY->ice_1 = 0;
    }
    if(YY->ice_2 < 0 || YY->ice_2 > 1000)
    {
        YY->ice_2 = 0;
    }
    if(YY->ice_3 < 0 || YY->ice_3 > 1000)
    {
        YY->ice_3 = 0;
    }
    if(YY->ice_4 < 0 || YY->ice_4 > 1000)
    {
        YY->ice_4 = 0;
    }
    if(YY->zt < 0 || YY->zt > 100)
    {
        YY->zt = 0;
    }
    
    if(YY->ice_1 == 0 && YY->ice_2 == 0 && YY->ice_3 == 0 && YY->ice_4 == 0 && YY->m_id == 0 && YY->y_id == 0 && YY->q_id == 0 && YY->zt == 0)
    {
        time_t timep_1;
        time(&timep_1);
        struct tm *p;
        p = gmtime(&timep_1);
        sprintf(A, "%4d/%2d/%2d %2d/%2d/%2d\t client closed client_fd:%d\n",p->tm_year + 1900,p->tm_mon + 1,p->tm_mday
        ,p->tm_hour + 8,p->tm_min,p->tm_sec,sfd);
        int fd = open("server_log.txt", O_WRONLY | O_APPEND);
        write(fd, A, strlen(A));
        close(fd);
        return ;
    }
    
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    sprintf(A, "select * from student  where id = %d", YY->m_id);
    mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    if(res_row == NULL)
    {
        if(YY->ice_1 != 1 || YY->ice_2 != 12)
        {
            YY->m_id = 0;
        }
        
    }
    sprintf(A, "select * from student  where id = %d", YY->y_id);
    mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    if(res_row == NULL)
    {
        YY->y_id = 0;
    }
    sprintf(A, "select * from allgroup where id = %d", YY->q_id);
    mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    if(res_row == NULL)
    {
        if(YY->ice_1 != 3 || YY->ice_2 != 31 || YY->ice_3 != 311)
        {
            YY->q_id = 0;
        }
    }
    //int day = (p->tm_year + 1900)*10000 + (p->tm_mon + 1)*100 + p->tm_mday;
    //int time = (p->tm_hour + 8)*10000 + (p->tm_min)*100 + p->tm_sec;
    sprintf(A, "%4d/%2d/%2d %2d/%2d/%2d\t %3d\t\t%3d\t\t%3d\t\t%3d\t %8d\t%8d\t%8d\t %d\t %d\n",p->tm_year + 1900,p->tm_mon + 1,p->tm_mday
                ,p->tm_hour + 8,p->tm_min,p->tm_sec,YY->ice_1, YY->ice_2, YY->ice_3, YY->ice_4,YY->m_id, YY->y_id, YY->q_id, YY->zt, sfd);
    int fd = open("server_log.txt", O_WRONLY | O_APPEND);
    write(fd, A, strlen(A));
    close(fd);
}

void *thread_account(void *arg)
{
    //printf("YYYYYYYYYYYYYYYYYYY\n");
    char A[100];
    int nready,i, n, ret, sfd;
    DENN *XX = (DENN*)malloc(sizeof(DENN));
    LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
    XINXI YY;
/*
    conn = mysql_init(conn);    //初始化一个句柄
    mysql_library_init(0,NULL,NULL);   //初始化数据库

    if(conn == NULL)
    {
        printf("mysql_init failed!!!\n");
        exit(1);
    }
    conn = mysql_real_connect(conn,"127.0.0.1","root","181219","db1",0,NULL,0);  //连接数据库
    mysql_set_character_set(conn,"utf8");      //调整为中文字符

    if(conn == NULL)
    {
        printf("mysql_real_connect failed!!!\n");
        exit(1);
    }

    //sprintf(A, "update student set zt=0");      //服务器重启，所有用户都不在线
    //mysql_query(conn,A);
*/
    E_LIST *p = (E_LIST *)arg;

    while(p->sum == 0)
    {
           //空语句
    }
    //struct epoll_event tep, ep[OPEN_MAX];   //定义描述符和响应队列
    //p->efd = epoll_create(OPEN_MAX);   
    //p->tep.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP;      
    //p->tep.data.fd = sfd;

    //epoll_ctl(p->efd, EPOLL_CTL_ADD, sfd, &(p->tep));     //将监听套接字加入等待队列

    //printf("AA\n");

    while(1)
    {
        //printf("A\n");
        nready = epoll_wait(p->efd, p->ep, OPEN_MAX, -1);     //等待就绪套接字，将就绪套接字放入响应队列，返回响应套接字的个数
        //printf("nready = %d\n", nready);
        //printf("errno is: %d\n",errno);
        for(i = 0; i < nready; i++)
        {
            //printf("B\n");
            if(!p->ep[i].events & EPOLLIN)     //该套接字描述符不可读
                continue;
            else if(p->ep[i].events & EPOLLIN)
            {
                //printf("C\n");
                sfd = p->ep[i].data.fd;
                memset(&YY, 0, sizeof(XINXI));
                //n = read(sfd, buf, sizeof(buf));         //读取套接字内容
                n = recv(sfd, &YY, sizeof(XINXI), 0);
                //TX(&YY);
                login(&YY, sfd);
                //printf("%d\n", n);
                //TX(&YY);
                if(n == 0)         //客户端关闭
                {
                    //printf("qu\n");
                    ret = epoll_ctl(p->efd, EPOLL_CTL_DEL, sfd, NULL);    //将套接字sfd从等待队列中删除
                    
                    sprintf(A, "update student set zt = 0 where id = %d", YY.m_id);
                    mysql_query(conn,A);        //用户退出，将状态设置为不在线
                    sprintf(A, "update student set fd = 0 where id = %d",YY.m_id);
                    mysql_query(conn,A);        //用户退出后将fd归0

                    close(sfd);        //关闭该套接字
                }
                else
                {
                    //printf("YY->ice = %d\n", YY.ice);
                    if(YY.ice_1 == 1)          //登陆，注册，找回密码
                    {
                        int ret = 0;
                        if(YY.ice_2 == 11)       //登陆
                        {
                            //printf("id = %d\tice = %d\n", XX->id, XX->ice);
                            ret = denglu(&YY, sfd);
                        }
                        else if(YY.ice_2 == 12)      //注册
                        {
                            ret = zhuce(&YY, sfd);
                            //printf("QQQQQQQ\n");
                        }
                        else if(YY.ice_2 == 13)      //找回密码
                        { 
                            if(YY.ice_3 == 131)
                            {
                                zhaohui_1(&YY, sfd);
                            }
                            else
                            {
                                ret = zhaohui(&YY, sfd);
                            }
                        }

                    }
                    else if(YY.ice_1 == 2)             //好友管理
                    {
                        getDENN(XX, YY.m_id, sfd);
                        xuanzhe_1(XX, &YY, sfd);
                    }
                    else if(YY.ice_1 == 3)             //聊天群管理
                    {
                        // TX(&YY);
                        if(YY.ice_4 == 3111)
                        {
                            G_ADD_1(&YY, sfd);
                        }
                        else
                        {
                            getDENN(XX, YY.m_id, sfd);
                            group_1(XX,&YY, sfd);
                        }
                    }
                    else if(YY.ice_1 == 4 && YY.ice_4 == 0)       //好友聊天
                    {
                        //printf("AAAAAAAAAAAAAs\n");
                        getDENN(XX, YY.m_id, sfd);
                        //printf("BBBBBBBBBBBBBBS\n");
                        xuanzhe_2(XX, &YY, sfd);
                    }
                    else if(YY.ice_1 == 5 && YY.ice_4 == 0)       //群聊天
                    {
                        //printf("AAAAAAAAAAAAAs\n");
                        getDENN(XX, YY.m_id, sfd);
                        //printf("BBBBBBBBBBBBBBS\n");
                        group_2(XX, &YY, sfd);
                    }
                    else if(YY.ice_1 == 6)      //查看通知
                    {
                        TongZ(&YY, sfd);
                    }
					else if(YY.ice_1 == 7)
					{
						Document(&YY, sfd, p);
					}
                    else if(YY.ice_4 == 666 && YY.ice_1 == 4)
                    {
                        //TX(&YY);
                        HY_send(&YY, sfd);
                    }
                    else if(YY.ice_4 == 777 && YY.ice_1 == 4)
                    {
                        //TX(&YY);
                        //printf("XX->id = %d\n", XX->id);
                        HY_get(&YY, YY.m_id, sfd);
                    }
                    else if(YY.ice_4 == 888 && YY.ice_1 == 5)
                    {
                        G_get(&YY, sfd);
                    }
                    else if(YY.ice_4 == 999 && YY.ice_1 == 5)
                    {
                        //YY.m_id = XX->id;
                        G_send(&YY, sfd);
                    }
                    else if(YY.ice_1 == 404)
                    {
                        logof(&YY, sfd);
                    }
                    else if(YY.ice_1 == 555)
                    {
                        get_TZ(&YY, sfd);
                    }
                }
                //printf("a\n");
            }
            //printf("b\n");
        }
        //printf("c\n");
    }
    //printf("d\n");
    free(XX);
}

void getDENN(DENN *XX, int id, int sfd)
{
    char A[100];
    int ret;
    memset(XX, 0, sizeof(DENN));
    XX->id = id;              //将m_id存入XX中
    sprintf(A, "update student set fd = %d where id = %d", sfd, XX->id);
    ret = mysql_query(conn,A);           //登陆后将fd存入数据库中
    //printf("A = %s\nret = %d\n", A, ret);
    //获取name
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    sprintf(A, "select name from student where id = %d", id);
    ret = mysql_query(conn,A);
    //printf("A = %s\nret = %d\n", A, ret);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    //printf("%s\n", res_row[0]);
    strncpy(XX->name, res_row[0], sizeof(XX->name));   
    mysql_free_result(res_ptr);
    //获取yhlb
    sprintf(A, "select hylb from student where id = %d", id);
    ret = mysql_query(conn,A);
    //printf("A = %s\nret = %d\n", A, ret);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    strncpy(XX->hylb, res_row[0], sizeof(XX->hylb));
    mysql_free_result(res_ptr);

}

char *getname_from_id(int id)
{
    int ret;
    char A[100], *name = (char *) malloc(sizeof(char)*20);
    //获取name
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    sprintf(A, "select name from student where id = %d", id);
    ret = mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    if(res_row != NULL)
    {
        strncpy(name, res_row[0], sizeof(name));   
    }
    else
    {
        printf("NULL\n");
    }
    mysql_free_result(res_ptr);

    return name;
}

char *getjl_from_id(int id_I, int id_II)
{
    //printf("id_I = %d, id_II = %d\n", id_I, id_II);
    char A[20],B[20], *jl = (char *)malloc(sizeof(char)*20);
    strncpy(A, getname_from_id(id_I), sizeof(A));
    strncpy(B, getname_from_id(id_II), sizeof(B));
    //printf("A: %s\n", A);
    //printf("B: %s\n", B);
    if(strcmp(A,B) > 0)
    {
        strncpy(jl, A, sizeof(A));
        strcat(jl,B);
    }
    else
    {
        strncpy(jl, B, sizeof(B));
        strcat(jl,A);
    }

    //printf("jl = %s\n", jl);

    return jl;
}

int getid_from_beizhu(int id,char *beizhu)
{
    char A[100];
    int ID;
    int ret;
    sprintf(A, "select id from %shylb where beizhu = '%s'", getname_from_id(id), beizhu);
    ret = mysql_query(conn,A);
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    mysql_free_result(res_ptr);
    if(res_row != NULL)
    {
        ID = atoi(res_row[0]);
        return ID;
    }
    else
    {
        return 0;
    }
}

char *getbeizhu_from_id(int m_id,int id)
{
    char A[100],*beizhu = (char *) malloc(sizeof(char)*20);
    int ret;
    sprintf(A, "select beizhu from %shylb where id = %d", getname_from_id(m_id), id);
    ret = mysql_query(conn,A);
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    mysql_free_result(res_ptr);
    //printf("YYYYYYYYYY\n");
    if(res_row != NULL)
    {
        strncpy(beizhu, res_row[0], sizeof(beizhu));
    }
    //printf("XXXXXXXXXXXX\n");
    return beizhu;
}

char *getgroupname_from_id(int id)
{
    char A[100], *name = (char *) malloc(sizeof(char)*20);
    int ret;
    sprintf(A, "select name from allgroup where id = %d", id);
    ret = mysql_query(conn,A);
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    if(res_row != NULL)
    {
        strncpy(name, res_row[0], sizeof(name));
    }
    mysql_free_result(res_ptr);

    return name;
}

char *getgroupjl_from_id(int id)
{
    char A[100], *jl = (char *) malloc(sizeof(char)*20);
    int ret;
    sprintf(A, "select jl from allgroup where id = %d", id);
    ret = mysql_query(conn,A);
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    if(res_row != NULL)
    {
        strncpy(jl, res_row[0], sizeof(jl));
    }
    mysql_free_result(res_ptr);

    return jl;
}

void get_TZ(XINXI *YY, int sfd)
{
    char A[100],B[50], ch;
    int ret, field, sum = 0, m = 0;
    LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
    sprintf(A, "select message from box where end_id = %d", YY->m_id);
    ret = mysql_query(conn,A);
    if(ret)
    {
        printf("wrong!\n");
    }
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res_ptr = mysql_store_result(conn);
    field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
    while(res_row=mysql_fetch_row(res_ptr))    
    {
        sum++;
        //printf("sum1 = %d\n", sum);
    }
    mysql_free_result(res_ptr);

    if(sum > 0)
    {
        sprintf(B, "----------有%d条加好友通知----------", sum);
        send(sfd, B, sizeof(B), 0);
    }

    sprintf(A, "select id from %sgrouptable where sf > 0",getname_from_id(YY->m_id));
    //printf("A = %s\n", A);
    ret = mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    while(res_row = mysql_fetch_row(res_ptr))
    {
        //printf("AAAA\n");
        int ID = atoi(res_row[0]);
        sprintf(A, "select message from box where end_id = %d", ID);
        //printf("A = %s\n", A);
        ret = mysql_query(conn,A);
        MYSQL_RES *res_ptr_1;
        MYSQL_ROW  res_row_1;
        res_ptr_1 = mysql_store_result(conn);
        do
        {
            res_row_1 = mysql_fetch_row(res_ptr_1);
            if(res_row_1 != NULL)
            {
                m++;
            }
            else
            {
                break;
            }
        }while(1);
        
        mysql_free_result(res_ptr_1);
    }
    mysql_free_result(res_ptr);

    if(m > 0)
    {
        sprintf(B, "----------有%d条加群通知------------", m);
        send(sfd, B, sizeof(B), 0);
    }

    sprintf(A, "select id from %shylb", getname_from_id(YY->m_id));
    mysql_query(conn, A);
    res_ptr = mysql_store_result(conn);
    while(res_row=mysql_fetch_row(res_ptr))    
    {
        int ID = atoi(res_row[0]);
        int s = 0;
        sprintf(A, "select * from %s where zt = 1 and end_id = %d", getjl_from_id(YY->m_id, ID), YY->m_id);
        mysql_query(conn, A);
        MYSQL_RES *res_ptr_1;
        MYSQL_ROW  res_row_1;
        res_ptr_1 = mysql_store_result(conn);        
        while(res_row_1 = mysql_fetch_row(res_ptr_1))
        {
            s++;
        }
        if(s > 0)
        {
            sprintf(B, "----------有来自%s的%d条信息-------------", getbeizhu_from_id(YY->m_id, ID), s);
            send(sfd, B, sizeof(B), 0);
        }
    }
//q_id需要循环

    int day, time_1;
    sprintf(A, "select id from %sgrouptable", getname_from_id(YY->m_id));
    mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    while(res_row = mysql_fetch_row(res_ptr))
    {
        sum = 0;
        YY->q_id = atoi(res_row[0]);
        sprintf(A, "select day from %scylb where id = %d", getgroupname_from_id(YY->q_id), YY->m_id);
        ret = mysql_query(conn,A);
        //printf("A = %s\nret = %d\nYY->m_id = %d\nYY->q_id = %d\n", A, ret, YY->m_id, YY->q_id);
        MYSQL_RES *res_ptr_1;
        MYSQL_ROW  res_row_1;
        res_ptr_1 = mysql_store_result(conn);
        res_row_1 = mysql_fetch_row(res_ptr_1);
        if(res_row_1 != NULL)
        {
            day  = atoi(res_row_1[0]);
        }
        mysql_free_result(res_ptr_1);

        sprintf(A, "select time from %scylb where id = %d", getgroupname_from_id(YY->q_id), YY->m_id);
        ret = mysql_query(conn,A);
        //printf("A = %s\nret = %d\nYY->m_id = %d\nYY->q_id = %d\n", A, ret, YY->m_id, YY->q_id);
        res_ptr_1 = mysql_store_result(conn);
        res_row_1 = mysql_fetch_row(res_ptr_1);
        if(res_row_1 != NULL)
        {
            time_1 = atoi(res_row_1[0]);
        }
        mysql_free_result(res_ptr_1);


        LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
        sprintf(A, "select * from %sjl where day = %d and time > %d and id != %d", getgroupname_from_id(YY->q_id), day, time_1, YY->m_id);
        ret = mysql_query(conn,A);
        //printf("A = %s\nret = %d\n", A, ret);
        res_ptr_1 = mysql_store_result(conn);
        while(res_row_1 = mysql_fetch_row(res_ptr_1))
        {
            sum++;
        }
        mysql_free_result(res_ptr_1);

        sprintf(A, "select * from %sjl where day > %d and id != %d", getgroupname_from_id(YY->q_id),day,YY->m_id);
        ret = mysql_query(conn,A);
        //printf("A = %s\nret = %d\n", A, ret);
        res_ptr_1 = mysql_store_result(conn);
        while(res_row_1 = mysql_fetch_row(res_ptr_1))
        {
            sum++;
        }
        mysql_free_result(res_ptr_1);
        if(sum > 0)
        {
            sprintf(B, "----------有来自群%s的%d条信息-----------", getgroupname_from_id(YY->q_id), sum);
            send(sfd, B, sizeof(B), 0);
        }
    }
    mysql_free_result(res_ptr);

    sprintf(A, "select from_id from document where end_id = %d", YY->m_id);
    mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    while(res_row = mysql_fetch_row(res_ptr))
    {
        int id = atoi(res_row[0]);
        sprintf(B, "----------有来自好友%d的文件传输----------",id);
        send(sfd, B, sizeof(B), 0);
    }
    mysql_free_result(res_ptr);

    sprintf(B, "over");
    send(sfd, B, sizeof(B), 0);
}

int G_ADD(XINXI *YY, DENN *XX)
{
    int field, fd, ret;

    char B[50], name[20], A[100];
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    strncpy(name, getgroupname_from_id(YY->q_id), sizeof(name));
    sprintf(A, "select * from box where from_id = %d and end_id = %d", YY->m_id, YY->q_id);
    ret = mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    if(res_row == NULL)
    {
        sprintf(B, "用户%s请求加入群聊%s！", XX->name, name);
        //printf("id = %d\n", XZ->id); 
        sprintf(A,"select * from %sgrouptable where id = %d", getname_from_id(YY->m_id), YY->q_id);
        ret = mysql_query(conn,A);
        res_ptr = mysql_store_result(conn);
        res_row = mysql_fetch_row(res_ptr);
        mysql_free_result(res_ptr);
        if(res_row != NULL)
        {
            ret = -1;            //该群聊已经加入
        }
        else
        {
            sprintf(A, "insert into box (from_id,end_id,message,ZT,ice,beizhu) values (%d,%d,'%s',0,1,'%s')", XX->id, YY->q_id, B, XX->name);
            ret = mysql_query(conn,A);
            if(ret)
            {
                printf("wrong!\n");
                ret = 0;              //信息发送失败
            }
            else
            {
                ret =  1;              //信息发送成功
            }
        }
    }
    else
    {
        ret = -2;
    }
    return ret;
}

void G_ADD_1(XINXI *YY, int sfd)        //问题回答
{
    char A[100],name[20],B[50];
    if(YY->zt == 1)     
    {
        int ret_1, ret_2, ret_3;
        strncpy(name, getgroupname_from_id(YY->q_id), sizeof(name));
        sprintf(A, "insert into %sgrouptable (id, name, sf) values (%d, '%s', 0)", getname_from_id(YY->m_id), YY->q_id, name);
        ret_1 = mysql_query(conn,A);        //添加到已加群表
        sprintf(A, "insert into %scylb (id, name, sf) values (%d, '%s', 0)", name, YY->m_id, getname_from_id(YY->m_id));
        ret_2 = mysql_query(conn,A);        //添加到成员列表
        if(ret_1 == 0 && ret_2 == 0)
            sprintf(B, "加入成功!\n");
        else
            sprintf(B, "加入失败!\n");
        send(sfd, B, sizeof(B), 0);
    }
}

int group_1(DENN *XX, XINXI *YY, int sfd)      //群管理
{
    char A[100],B[50],name[20],ch;
    int ret, id, ret_1, ret_2, ret_3, ret_4, field;
    LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
    YY->m_id = XX->id;
    if(YY->ice_2 == 31)                        //群的创建与解散
    {
        time_t timep_1;
        struct tm *p;
        time(&timep_1);
        if(YY->ice_3 == 311)          //群的创建
        {
            if(YY->zt == 1 || YY->zt == 0)        //需要管理员同意或无要求
            {
                sprintf(A, "select * from allgroup where id = %d", YY->q_id);
                ret = mysql_query(conn,A);
                MYSQL_RES *res_ptr;
                MYSQL_ROW  res_row;
                res_ptr = mysql_store_result(conn);
                res_row = mysql_fetch_row(res_ptr);
                mysql_free_result(res_ptr);
                if(res_row != NULL)     //该ID已存在
                {
                    sprintf(B, "该ID已存在，请更换！");
                }
                else
                {
                    sprintf(A, "select * from allgroup where name = %s", YY->name);
                    ret = mysql_query(conn,A);
                    MYSQL_RES *res_ptr;
                    MYSQL_ROW  res_row;
                    res_ptr = mysql_store_result(conn);
                    res_row = mysql_fetch_row(res_ptr);
                    mysql_free_result(res_ptr);
                    if(res_row != NULL)   //该name已存在
                    {
                        sprintf(B, "该name已存在，请更换！");
                    }
                    else
                    {
                        p = gmtime(&timep_1);
                        int day = (p->tm_year + 1900)*10000 + (p->tm_mon + 1)*100 + p->tm_mday;
                        int time = (p->tm_hour + 8)*10000 + (p->tm_min)*100 + p->tm_sec;
                        //printf("m_id = %d,id = %d\n",YY->m_id, XX->id);
                        sprintf(A, "insert into %sgrouptable (id, name, sf) values (%d, '%s', 2)", getname_from_id(XX->id), YY->q_id, YY->name);
                        //printf("A = %s\n", A);
                        ret = mysql_query(conn,A);
                        sprintf(A, "insert into allgroup (id, name, cylb, jl, qu, an, zt) values (%d, '%s', '%scylb', '%sjl', NULL, NULL, %d)", YY->q_id, YY->name, YY->name, YY->name, YY->zt);
                        ret_1 = mysql_query(conn,A);        //添加至群信息列表
                        sprintf(A, "create table %scylb (id int, name varchar(20), sf int, day int, time int)",YY->name);
                        ret_2 = mysql_query(conn,A);        //创建群成员列表
                        sprintf(A, "create table %sjl (id int, name varchar(20), xinxi varchar(100), day int, time int)", YY->name);
                        ret_3 = mysql_query(conn,A);        //创建群聊天记录列表
                        sprintf(A, "insert into %scylb (id, name, sf, day, time) values (%d, '%s', 2, %d, %d)", YY->name, YY->m_id, getname_from_id(XX->id), day, time);
                        ret_4 = mysql_query(conn,A);
                        //printf("ret_1 = %d\tret_2 = %d\tret_3 = %d\n", ret_1, ret_2, ret_3);
                        if(ret_1 == 0 && ret_2 == 0 && ret_3 == 0 && ret_4 == 0 && ret == 0)
                        {
                            sprintf(B, "创建成功！！！");
                        }
                        else
                        {
                            sprintf(B, "创建失败！！！");
                        }
                    }
                }
                //mysql_free_result(res_ptr);
                send(sfd, B, sizeof(B), 0);
            }
            else if(YY->zt == 2)   //需要回答问题
            {
                time_t timep_1;
                sprintf(A, "select * from allgroup where id = %d", YY->q_id);
                ret = mysql_query(conn,A);
                MYSQL_RES *res_ptr;
                MYSQL_ROW  res_row;
                res_ptr = mysql_store_result(conn);
                res_row = mysql_fetch_row(res_ptr);
                mysql_free_result(res_ptr);
                if(res_row != NULL)     //该ID已存在
                {
                    sprintf(B, "该ID已存在，请更换！");
                }
                else
                {
                    sprintf(A, "select * from allgroup where name = %s", YY->name);
                    ret = mysql_query(conn,A);
                    MYSQL_RES *res_ptr;
                    MYSQL_ROW  res_row;
                    res_ptr = mysql_store_result(conn);
                    res_row = mysql_fetch_row(res_ptr);
                    mysql_free_result(res_ptr);
                    if(res_row != NULL)   //该name已存在
                    {
                        sprintf(B, "该name已存在，请更换！");
                    }
                    
                    else
                    {
                        p = gmtime(&timep_1);
                        int day = (p->tm_year + 1900)*10000 + (p->tm_mon + 1)*100 + p->tm_mday;
                        int time = (p->tm_hour + 8)*10000 + (p->tm_min)*100 + p->tm_sec;
                        //printf("id = %d\n", XX->id);
                        sprintf(A, "insert into %sgrouptable (id, name, sf) values (%d, '%s', 2)", getname_from_id(XX->id), YY->q_id, YY->name);
                        //printf("A = %s\n", A);
                        ret = mysql_query(conn,A);
                        sprintf(A, "insert into allgroup (id, name, cylb, jl, qu, an, zt) values (%d, '%s', '%scylb', '%sjl', '%s', '%s', %d)", YY->q_id, YY->name, YY->name, YY->name, YY->qu, YY->an, YY->zt);
                        //printf("A = %s\n", A);
                        ret_1 = mysql_query(conn,A);        //添加至群信息列表
                        sprintf(A, "create table %scylb (id int, name varchar(20), sf int,day int, time int)",YY->name);
                        //printf("A = %s\n", A);
                        ret_2 = mysql_query(conn,A);        //创建群成员列表
                        sprintf(A, "create table %sjl (id int, name, xinxi varchar(100), day int, time int)", YY->name);
                        //printf("A = %s\n", A);
                        ret_3 = mysql_query(conn,A);        //创建群聊天记录列表
                        sprintf(A, "insert into %scylb (id, name, sf, day, time) values (%d, '%s', 2, %d, %d)", YY->name, YY->m_id, getname_from_id(XX->id), day, time);
                        //printf("A = %s\n", A);
                        ret_4 = mysql_query(conn,A);
                        if(ret_1 == 0 && ret_2 == 0 && ret_3 == 0 && ret_4 == 0 && ret == 0) 
                        {
                            sprintf(B, "创建成功！！！");
                        }
                    }
                }
                //mysql_free_result(res_ptr);
                send(sfd, B, sizeof(B), 0);
            }
        }
        else if(YY->ice_3 == 312)        //群的解散
        {
            //TX(YY);
            sprintf(A, "select * from allgroup where id = %d", YY->q_id);
            ret = mysql_query(conn,A);
            MYSQL_RES *res_ptr;
            MYSQL_ROW  res_row;
            res_ptr = mysql_store_result(conn);
            res_row = mysql_fetch_row(res_ptr);
            mysql_free_result(res_ptr);
            if(res_row != NULL)
            {
                int I;
                strncpy(name, getgroupname_from_id(YY->q_id), sizeof(name));

                sprintf(A, "select id from %scylb", name);
                ret = mysql_query(conn,A);
                res_ptr = mysql_store_result(conn);
                while(res_row = mysql_fetch_row(res_ptr))
                {
                    I = atoi(res_row[0]);
                    sprintf(A, "delete from %sgrouptable where id = %d", getname_from_id(I), YY->q_id);
                    ret = mysql_query(conn,A);    //将已加群列表中该群的信息删除
                    if(ret != 0)
                    {
                        printf("群解散失败！！！\n");
                    }
                }
                mysql_free_result(res_ptr);

                sprintf(A, "delete from allgroup where id = %d", YY->q_id);
                ret_1 = mysql_query(conn,A);
                sprintf(A, "drop table %scylb", name);
                ret_2 = mysql_query(conn,A);
                sprintf(A, "drop table %sjl", name);
                ret_3 = mysql_query(conn,A);
                //printf("ret_1 = %d\tret_2 = %d\tret_3 = %d\n", ret_1, ret_2, ret_3);
                if(ret_1 == 0 && ret_2 == 0 && ret_3 == 0)
                {
                    sprintf(B, "解散成功！！!");
                }
                else
                {
                    sprintf(B, "解散失败！！!");
                }
            }
            else
            {
                sprintf(B, "该群ID不存在！");
                //printf("QQQ\n");
            }
            //printf("YYY\n");
            send(sfd, B, sizeof(B), 0);
        }
    }
    else if(YY->ice_2 == 32)                   //申请加群退群*
    {
        if(YY->ice_3 == 321)       //加群
        {
            int ZT;
            sprintf(A, "select zt from allgroup where id = %d", YY->q_id);
            //printf("%s\n", A);
            ret = mysql_query(conn,A);
            MYSQL_RES *res_ptr;
            MYSQL_ROW  res_row;
            res_ptr = mysql_store_result(conn);
            res_row = mysql_fetch_row(res_ptr);
            mysql_free_result(res_ptr);
            if(res_row == NULL)
            {
                sprintf(XZ->buf, "无此ID的群！！！");
            }
            else
            {   
                //printf("res_row = %s\n",res_row[0]);
                ZT = atoi(res_row[0]);
            }
            //printf("ZT = %d\n", ZT);
            if(ZT == 0)              //直接加入
            {
                sprintf(A, "select * from %sgrouptable where id = %d", getname_from_id(YY->m_id), YY->q_id);
                res_ptr = mysql_store_result(conn);
                res_row = mysql_fetch_row(res_ptr);
                if(res_row != NULL)
                {
                    sprintf(XZ->buf, "你已经加入了此群！！!\n");
                }
                else
                {
                    int ret_1, ret_2, ret_3;
                    strncpy(name, getgroupname_from_id(YY->q_id), sizeof(name));
                    sprintf(A, "insert into %sgrouptable (id, name, sf) values (%d, '%s', 0)", getname_from_id(YY->m_id), YY->q_id, name);
                    ret_1 = mysql_query(conn,A);        //添加到已加群表
                    sprintf(A, "insert into %scylb (id, name, sf) values (%d, '%s', 0)", name, YY->m_id, getname_from_id(YY->m_id));
                    ret_2 = mysql_query(conn,A);        //添加到成员列表
                    XZ->zt = 0;
                    if(ret_1 == 0 && ret_2 == 0)
                        sprintf(XZ->buf, "加入成功!\n");
                    else
                        sprintf(XZ->buf, "加入失败!\n");
                }
            }
            else if(ZT == 1)        //需要管理员同意才能加入
            {
                ret = G_ADD(YY, XX);
                XZ->zt = 1;
                if(ret == 0)
                {
                    sprintf(XZ->buf, "请求发送失败!");
                }
                else if(ret == 1)
                {
                    sprintf(XZ->buf, "请求发送成功，等待同意中！");
                }
                else if(ret == -1)
                {
                    sprintf(XZ->buf, "你已经加入此群！");
                }
                else if(ret == -2)
                {
                    sprintf(XZ->buf, "请求已经发送过！");
                }
            }
            else if(ZT == 2)        //回答问题加入
            {
                sprintf(A, "select qu and an from allgroup where id = %d", YY->q_id);
                ret = mysql_query(conn,A);
                res_ptr = mysql_store_result(conn);
                res_row = mysql_fetch_row(res_ptr);
                XZ->zt = 100;
                strncpy(XZ->qu, res_row[0], sizeof(XZ->qu));
                strncpy(XZ->an, res_row[1], sizeof(XZ->an));
                mysql_free_result(res_ptr);
            }
            send(sfd, XZ, sizeof(LIAOT), 0);
        }
        else if(YY->ice_3 == 322)    //退群
        {
            sprintf(A, "select * from %sgrouptable where id = %d", getname_from_id(YY->m_id),YY->q_id);
            ret = mysql_query(conn,A);
            MYSQL_RES *res_ptr;
            MYSQL_ROW  res_row;
            res_ptr = mysql_store_result(conn);
            res_row = mysql_fetch_row(res_ptr);
            mysql_free_result(res_ptr);
            if(res_row != NULL)
            {
                int ret_1, ret_2;
                sprintf(A, "delete * from %sgrouptable where id = %d", getname_from_id(YY->m_id), YY->q_id);
                ret_1 = mysql_query(conn,A);         //从已加列表中删除
                sprintf(A, "delete from %scylb where id = %d", getgroupname_from_id(YY->q_id), YY->m_id);
                ret_2 = mysql_query(conn,A);         //从群成员列表中删除
                if(ret_1 == 0 && ret_2 == 0)
                {
                    sprintf(B, "退出成功！");
                }
            }
            else
            {
                sprintf(B, "你没有加入此群！！！");
            }
            send(sfd, B, sizeof(B), 0);
        }
    }
    else if(YY->ice_2 == 33)                   //查看已加群和群成员
    {
        if(YY->ice_3 == 331)          //查看已加群
        {
            sprintf(A, "select * from %sgrouptable", getname_from_id(YY->m_id));
            //printf("A = %s\n", A);
            ret = mysql_query(conn,A);
            MYSQL_RES *res_ptr;
            MYSQL_ROW  res_row;
            res_ptr = mysql_store_result(conn);
            field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
            if(ret == 0) 
            {
                sprintf(B, "ok");
                send(sfd, B, sizeof(B), 0);
            }
            while(res_row = mysql_fetch_row(res_ptr))
            {
                int i, sf;
                memset(B, 0, sizeof(B));
                for(i = 0; i < field-1; i++)
                {
                    strcat(B, res_row[i]);
                    strcat(B, "\t");
                }
                sf = atoi(res_row[i]);
                if(sf == 1)
                {
                    strcat(B, "管理员");
                }
                else if(sf == 2)
                {
                    strcat(B, "群主");
                }
                else if(sf == 0)
                {
                    strcat(B, "普通群成员");
                }
                //printf("B = %s\n", B);
                send(sfd, B, sizeof(B), 0);
            }
            sprintf(B, "over");
            send(sfd, B, sizeof(B), 0);
            mysql_free_result(res_ptr);
        }
        else if(YY->ice_3 == 332)     //查看群成员
        {
            sprintf(A, "select * from allgroup where id = %d", YY->q_id);
            ret = mysql_query(conn,A);
            MYSQL_RES *res_ptr;
            MYSQL_ROW  res_row;
            res_ptr = mysql_store_result(conn);
            res_row = mysql_fetch_row(res_ptr);
            mysql_free_result(res_ptr);
            if(res_row != NULL)       //该群存在
            {
                sprintf(A, "select * from %sgrouptable where id = %d", getname_from_id(YY->m_id), YY->q_id);
                ret = mysql_query(conn,A);
                res_ptr = mysql_store_result(conn);
                res_row = mysql_fetch_row(res_ptr);
                mysql_free_result(res_ptr);
                if(res_row != NULL)   //用户在该群中
                {
                    sprintf(A, "select * from %scylb", getgroupname_from_id(YY->q_id));
                    ret = mysql_query(conn,A);
                    res_ptr = mysql_store_result(conn);
                    field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
                    while(res_row = mysql_fetch_row(res_ptr))
                    {
                        int i, sf;
                        memset(B, 0, sizeof(B));
                        for(i = 0; i < 2; i++)
                        {
                            strcat(B, res_row[i]);
                            strcat(B, "\t");
                        }
                        sf = atoi(res_row[i]);
                        if(sf == 1)
                        {
                            strcat(B, "管理员");
                        }
                        else if(sf == 2)
                        {
                            strcat(B, "群主");
                        }
                        else if(sf == 0)
                        {
                            strcat(B, "普通群成员");
                        }
                        //printf("B = %s\n", B);
                        send(sfd, B, sizeof(B), 0);
                    }
                    mysql_free_result(res_ptr);
                }
                else
                {
                    sprintf(B, "你不是该群的成员，无权查询！");
                    send(sfd, B, sizeof(B), 0);
                }
            }
            else
            {
                sprintf(B, "该群不存在!");
                send(sfd, B, sizeof(B), 0);
            }
            sprintf(B, "over");
            send(sfd, B, sizeof(B), 0);
        }
    }
    else if(YY->ice_2 == 34)                   //查看聊天记录
    {
        sprintf(A, "select * from %sgrouptable where id = %d", getname_from_id(YY->m_id), YY->q_id);
        ret = mysql_query(conn, A);
        //printf("A = %s\nret = %d\n",A, ret);
        MYSQL_RES *res_ptr;
        MYSQL_ROW  res_row;
        res_ptr = mysql_store_result(conn);
        res_row = mysql_fetch_row(res_ptr);
        mysql_free_result(res_ptr);
        if(res_row != NULL)
        {
            strncpy(XZ->buf, "ok", sizeof(XZ->buf));
            send(sfd, XZ, sizeof(LIAOT), 0);

            sprintf(A, "select * from %sjl", getgroupname_from_id(YY->q_id));
            ret = mysql_query(conn, A);
            res_ptr = mysql_store_result(conn);
            field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
            while(res_row = mysql_fetch_row(res_ptr))
            {
                XZ->id = atoi(res_row[0]);
                strncpy(XZ->name, res_row[1], sizeof(XZ->name));
                strncpy(XZ->xinxi, res_row[2], sizeof(XZ->xinxi));
                XZ->zt = atoi(res_row[3]);
                XZ->ice = atoi(res_row[4]);
                send(sfd,XZ, sizeof(LIAOT), 0);
            }
            mysql_free_result(res_ptr);
            memset(XZ, 0, sizeof(LIAOT));
            strncpy(XZ->buf, "over", sizeof(XZ->buf));
            send(sfd, XZ, sizeof(LIAOT), 0);
        }
    }
    else if(YY->ice_2 == 35)                    //设置管理员
    {
        int pow,ret_1, ret_2;
        memset(B, 0, sizeof(B));
        strncpy(name, getgroupname_from_id(YY->q_id), sizeof(name));
        sprintf(A, "select sf from %scylb where id = %d", name, YY->m_id);
        ret = mysql_query(conn, A);
        MYSQL_RES *res_ptr;
        MYSQL_ROW  res_row;
        res_ptr = mysql_store_result(conn);
        res_row = mysql_fetch_row(res_ptr);
        if(res_row != NULL)
        {
            pow = atoi(res_row[0]);
            if(pow == 2)     //群主
            {
                sprintf(A, "update %scylb set sf = 1 where id = %d", name, YY->y_id);
                ret_1 = mysql_query(conn,A);
                sprintf(A, "update %sgrouptable set sf = 1 where id = %d", getname_from_id(YY->y_id), YY->q_id);
                //printf("A = %s\n", A);
                ret_2 = mysql_query(conn,A);
                if(ret_1 == 0 && ret_2 == 0)
                {
                    sprintf(B, "设置成功!");
                    //printf("B = %s\n", B);
                }
                //printf("ret_1 = %d\nret_2 = %d\n", ret_1, ret_2);
            }
            else
            {
                sprintf(B, "你不是群主，没有权限设置管理员！");
            }
        }
        else
        {
            sprintf(B, "你不是该群的成员!");
        }
        send(sfd, B, sizeof(B), 0);
        //printf("B = %s\n", B);
        mysql_free_result(res_ptr);
    }
    else if(YY->ice_2 == 36)                   //踢人
    {
        //printf("AAA\n");
        int pow_1, pow_2;
        strncpy(name, getgroupname_from_id(YY->q_id), sizeof(name));
        sprintf(A, "select sf from %scylb where id = %d", name,YY->m_id);
        //printf("A = %s\n", A);
        ret = mysql_query(conn,A);
        MYSQL_RES *res_ptr;
        MYSQL_ROW  res_row;
        res_ptr = mysql_store_result(conn);
        res_row = mysql_fetch_row(res_ptr);
        mysql_free_result(res_ptr);
        if(res_row != NULL)
        {
            pow_1 = atoi(res_row[0]);
            
            if(pow_1 > 0)
            {
                sprintf(A, "select sf from %scylb where id = %d", name, YY->y_id);
                //printf("A = %s\n", A);
                ret = mysql_query(conn, A);
                res_ptr = mysql_store_result(conn);
                res_row = mysql_fetch_row(res_ptr);
                if(res_row != NULL)
                {
                    pow_2 = atoi(res_row[0]);
                    if(pow_1 > pow_2)
                    {
                        int ret_1, ret_2;
                        sprintf(A, "delete from %scylb where id = %d", name, YY->y_id);
                        //printf("A = %s\n", A);
                        ret_1 = mysql_query(conn, A);
                        sprintf(A, "delete from %sgrouptable where id = %d", getname_from_id(YY->y_id), YY->q_id);
                        //printf("A = %s\n", A);
                        ret_2 = mysql_query(conn,A);
                        //printf("ret_1 = %d\nret_2 = %d\n", ret_1,ret_2);
                        if(ret_1 == 0 && ret_2 == 0)
                        {
                            sprintf(B, "已踢出此群！\n");
                        }
                    }
                    else 
                    {
                        sprintf(B, "你没有权限踢人\n");
                    }
                }
                mysql_free_result(res_ptr);
            }
        }
        else
        {
            sprintf(B, "你没有权限踢人\n");
        }
        //printf("B = %s\n", B);
        send(sfd, B, sizeof(B), 0);
    }

    free(XZ);
}

void G_send(XINXI *YY, int sfd)
{
    //printf("AAA\n");
    char A[100];
    int day, time_1, ret, sum = 0, i;
    sprintf(A, "select day from %scylb where id = %d", getgroupname_from_id(YY->q_id), YY->m_id);
    ret = mysql_query(conn,A);
    //printf("A = %s\nret = %d\nYY->m_id = %d\nYY->q_id = %d\n", A, ret, YY->m_id, YY->q_id);
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    if(res_row != NULL)
    {
        day  = atoi(res_row[0]);
    }
    mysql_free_result(res_ptr);

    sprintf(A, "select time from %scylb where id = %d", getgroupname_from_id(YY->q_id), YY->m_id);
    ret = mysql_query(conn,A);
    //printf("A = %s\nret = %d\nYY->m_id = %d\nYY->q_id = %d\n", A, ret, YY->m_id, YY->q_id);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    if(res_row != NULL)
    {
        time_1 = atoi(res_row[0]);
    }
    mysql_free_result(res_ptr);

    sum = 0;

    sprintf(A, "select * from %sjl where day = %d and time > %d and id != %d", getgroupname_from_id(YY->q_id), day, time_1, YY->m_id);
    ret = mysql_query(conn,A);
    //printf("A = %s\nret = %d\n", A, ret);
    res_ptr = mysql_store_result(conn);
    while(res_row = mysql_fetch_row(res_ptr))
    {
        sum++;
    }
    mysql_free_result(res_ptr);

    sprintf(A, "select * from %sjl where day > %d and id != %d", getgroupname_from_id(YY->q_id),day,YY->m_id);
    ret = mysql_query(conn,A);
    //printf("A = %s\nret = %d\n", A, ret);
    res_ptr = mysql_store_result(conn);
    while(res_row = mysql_fetch_row(res_ptr))
    {
        sum++;
    }
    mysql_free_result(res_ptr);
    //printf("sum = %d\n",sum);
    send(sfd, &sum, sizeof(sum), 0);


    LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
    sprintf(A, "select * from %sjl where day = %d and time > %d and id != %d", getgroupname_from_id(YY->q_id), day, time_1, YY->m_id);
    ret = mysql_query(conn,A);
    //printf("A = %s\nret = %d\n", A, ret);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    mysql_free_result(res_ptr);
    if(res_row != NULL)
    {
            //有未读信息
        sprintf(A, "select * from %sjl where day = %d and time > %d and id != %d", getgroupname_from_id(YY->q_id), day, time_1, YY->m_id);
        ret = mysql_query(conn,A);
        //printf("A = %s\nret = %d\n", A, ret);
        res_ptr = mysql_store_result(conn);
        //int field = mysql_num_fields(res_ptr);
        while(res_row = mysql_fetch_row(res_ptr))
        {
            int ret_1, ret_2;
            XZ->zt = 1;
            strncpy(XZ->name, res_row[1], sizeof(XZ->name));      //名字
            strncpy(XZ->xinxi, res_row[2], sizeof(XZ->xinxi));        //信息
            day  = atoi(res_row[3]);     //该信息时间
            time_1 = atoi(res_row[4]);    
            sprintf(A, "update %scylb set day = %d where id = %d", getgroupname_from_id(YY->q_id), day, YY->m_id);
            ret_1 = mysql_query(conn,A);
            //printf("A = %s\nret = %d\n", A, ret);
            sprintf(A, "update %scylb set time = %d where id = %d", getgroupname_from_id(YY->q_id), time_1, YY->m_id);
            ret_2 = mysql_query(conn,A);
            //printf("A = %s\nret = %d\n", A, ret);
            if(ret_1 == 0 && ret_2 == 0)
            {
                //printf("%s : %s\n", XZ->name, XZ->xinxi);
                send(sfd, XZ, sizeof(LIAOT), 0);
            }
        }
        mysql_free_result(res_ptr);
    }
    else
    {
        sum = 0;         //无未读信息
    }
    sprintf(A, "select * from %sjl where day > %d and id != %d", getgroupname_from_id(YY->q_id),day,YY->m_id);
    ret = mysql_query(conn,A);
    //printf("A = %s\nret = %d\n", A, ret);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    mysql_free_result(res_ptr);
    if(res_row != NULL)
    {
        
        XZ->zt = 1; 
        //有未读信息
        sprintf(A, "select * from %sjl where day > %d and id != %d", getgroupname_from_id(YY->q_id),day,YY->m_id);
        ret = mysql_query(conn,A);
        //printf("A = %s\nret = %d\n", A, ret);
        res_ptr = mysql_store_result(conn);
        while(res_row = mysql_fetch_row(res_ptr))
        {
            int ret_1, ret_2;
            XZ->zt = 1;
            strncpy(XZ->name, res_row[1], sizeof(XZ->name));      //名字
            strncpy(XZ->xinxi, res_row[2], sizeof(XZ->xinxi));        //信息
            day  = atoi(res_row[3]);     //该信息时间
            time_1 = atoi(res_row[4]);    
            sprintf(A, "update %scylb set day = %d where id = %d", getgroupname_from_id(YY->q_id), day, YY->m_id);
            ret_1 = mysql_query(conn,A);
            //printf("A = %s\nret = %d\n", A, ret);
            sprintf(A, "update %scylb set time = %d where id = %d", getgroupname_from_id(YY->q_id), time_1, YY->m_id);
            ret_2 = mysql_query(conn,A);
            //printf("A = %s\nret = %d\n", A, ret);
            if(ret_1 == 0 && ret_2 == 0)
            {
                send(sfd, XZ, sizeof(LIAOT), 0);
            }
        }
    }
    //XZ->zt = 0;
    //send(sfd, XZ, sizeof(LIAOT), 0);
}

void G_get(XINXI *YY, int sfd)
{
    char A[100];
    time_t timep_1;
    time(&timep_1);
    struct tm *p;
    p = gmtime(&timep_1);
    int day = (p->tm_year + 1900)*10000 + (p->tm_mon + 1)*100 + p->tm_mday;
    int time = (p->tm_hour + 8)*10000 + (p->tm_min)*100 + p->tm_sec;
    int ret;
    if(strcmp(YY->buf, "exit") == 0) 
    {
        return;
    }
    sprintf(A, "insert into %sjl (id, name, xinxi, day, time) values (%d, '%s', '%s', %d, %d)",getgroupname_from_id(YY->q_id), YY->m_id, getname_from_id(YY->m_id), YY->buf, day, time);
    ret = mysql_query(conn,A);
    //printf("A = %s\nret = %d\n", A, ret);
}

int group_2(DENN *XX, XINXI *YY, int sfd)      //群聊天
{
    //printf("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY\n");
    int ret;
    char A[100], B[50];
    YY->m_id = XX->id;
    if(YY->ice_2 == 41)
    {
        //printf("YY->m_id = %d\n", YY->m_id);
        sprintf(A, "select * from %sgrouptable where id = %d", getname_from_id(YY->m_id), YY->q_id);
        ret = mysql_query(conn,A);
        //printf("A = %s\nret = %d\n", A, ret);
        MYSQL_RES *res_ptr;
        MYSQL_ROW  res_row;
        res_ptr = mysql_store_result(conn);
        res_row = mysql_fetch_row(res_ptr);
        mysql_free_result(res_ptr);
        if(res_row != NULL)
        {
            sprintf(B, "ok");
            write(sfd, B, sizeof(B));
        }
        else
        {
            //用户未加入此群
            sprintf(B, "你未加入该群！");
            write(sfd, B, sizeof(B));
        }
    }

}

int TongZ(XINXI *YY, int sfd)
{
    char A[100],B[50], ch;
    int ret, field, sum = 0, q[20], m = 0;
    //printf("id = %d\n", YY->id);
    //printf("m_id = %d\n", YY->m_id);
    sprintf(A, "select message from box where end_id = %d", YY->m_id);
    ret = mysql_query(conn,A);
    if(ret)
    {
        printf("wrong!\n");
    }
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res_ptr = mysql_store_result(conn);
    field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
    //printf("field = %d\n", field);
    while(res_row=mysql_fetch_row(res_ptr))    
    {
        sum++;
        //printf("sum1 = %d\n", sum);
    }
    mysql_free_result(res_ptr);

    sprintf(A, "select id from %sgrouptable where sf > 0",getname_from_id(YY->m_id));
    //printf("A = %s\n", A);
    ret = mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    while(res_row = mysql_fetch_row(res_ptr))
    {
        //printf("AAAA\n");
        int ID = atoi(res_row[0]);
        sprintf(A, "select message from box where end_id = %d", ID);
        //printf("A = %s\n", A);
        ret = mysql_query(conn,A);
        MYSQL_RES *res_ptr_1;
        MYSQL_ROW  res_row_1;
        res_ptr_1 = mysql_store_result(conn);
        /*
        res_row_1 = mysql_fetch_row(res_ptr_1);
        if(res_row_1 == NULL)
        {
            printf("AAAA\n");
        }
        else
        {
            printf("BBB\n");
        }
        */
        do
        {
            res_row_1 = mysql_fetch_row(res_ptr_1);
            if(res_row_1 != NULL)
            {
                sum++;
                q[m] = ID;
                m++;
            }
            else
            {
                break;
            }
        }while(1);
        
        mysql_free_result(res_ptr_1);
    }
    mysql_free_result(res_ptr);

    //printf("sum2 = %d\n", sum);
    //printf("m = %d\n", m);
    write(sfd, &sum, sizeof(int));

        read(sfd, B, sizeof(B));           //接收客户端选择
        if(strcmp(B, "A") == 0)
        {
            sprintf(A, "select message from box where end_id = %d and ice = 0", YY->m_id);
            ret = mysql_query(conn,A);
            MYSQL_RES *res_ptr;
            MYSQL_ROW  res_row;
            res_ptr = mysql_store_result(conn);
            field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
            //printf("field = %d\n", field);
            while(res_row=mysql_fetch_row(res_ptr)) 
            {
                sprintf(B, "%s", res_row[0]);
                write(sfd, B, sizeof(B));
            }
            mysql_free_result(res_ptr);
            for(int i=0; i < m;i++)
            {
                sprintf(A, "select message from box where end_id = %d and ice = 1", q[i]);
                ret = mysql_query(conn,A);
                res_ptr = mysql_store_result(conn);
                field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
                //printf("field = %d\n", field);
                while(res_row=mysql_fetch_row(res_ptr)) 
                {
                    sprintf(B, "%s", res_row[0]);
                    write(sfd, B, sizeof(B));
                }
                mysql_free_result(res_ptr);
            }
            sprintf(B, "over");
            write(sfd, B, sizeof(B));
        }
        else if(strcmp(B, "B") == 0)
        {
            //printf("AAA\n");
            sprintf(A, "select * from box where end_id = %d and ice = 0", YY->m_id);
            ret = mysql_query(conn,A);
            MYSQL_RES *res_ptr;
            MYSQL_ROW  res_row;
            LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
            res_ptr = mysql_store_result(conn);
            field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
            //printf("field = %d\n", field);
            while(res_row=mysql_fetch_row(res_ptr)) 
            {
                //printf("CCC\n");
                int i,s_id, e_id;
                for(i = 0; i < field; i++)
                {
                    if(i == 0)
                    {
                        s_id = atoi(res_row[i]);
                    }
                    else if(i == 1)
                    {
                        e_id = atoi(res_row[i]);
                    }
                }
                sprintf(XZ->buf, "%s", res_row[i]);
                XZ->zt = 0;     //加好友
                send(sfd, XZ, sizeof(LIAOT), 0);
                recv(sfd, XZ, sizeof(LIAOT), 0);
                //printf("buf = %s\n",XZ->buf);
                if(strcmp(XZ->buf, "Y") == 0)                           //群人分开
                {
                    
                        //人  
                        int ret_1,ret_2,ret_3;
                        //printf("s_id = %d\te_id = %d\n",s_id,e_id);
                        sprintf(A, "insert into %shylb (id, beizhu, jl, zt) values (%d, '%s', '%s', 0)", getname_from_id(e_id),s_id,XZ->beizhu,getjl_from_id(s_id,e_id));
                        ret_1 = mysql_query(conn,A);        //添加到好友列表
                        sprintf(A, "insert into %shylb (id, beizhu, jl, zt) values (%d, '%s', '%s', 0)", getname_from_id(s_id),e_id,res_row[field-1],getjl_from_id(s_id,e_id));
                        ret_2 = mysql_query(conn,A);        //添加到好友列表
                        sprintf(A, "create table %s (from_id int, xinxi varchar(200), end_id int, zt int)", getjl_from_id(s_id,e_id));
                        ret_3 = mysql_query(conn,A);        //创建信息表
                        sprintf(A, "delete from box where from_id = %d and end_id = %d and zt = 0", s_id, e_id);
                        ret = mysql_query(conn,A);        //删除该通知
                        if(ret == 0 && ret_1 == 0 && ret_2 == 0 && ret_3 == 0)
                            sprintf(B, "添加成功!\n");
                        else
                            sprintf(B, "添加失败!\n");

                    strncpy(XZ->buf, B, sizeof(B));
                    send(sfd, XZ, sizeof(LIAOT), 0);
                    
                }
                else if(strcmp(XZ->buf, "N") == 0)
                {
                    sprintf(A, "delete from box where from_id = %d and end_id = %d", s_id, e_id);
                    mysql_query(conn,A);        //删除该通知
                }   
                
            }
            for(int i=0; i < m;i++)
            {
                //printf("BBB\n");
                sprintf(A, "select * from box where end_id = %d and ice = 1", q[i]);
                //printf("A = %s\n", A);
                ret = mysql_query(conn,A);
                res_ptr = mysql_store_result(conn);
                field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
                //printf("field = %d\n", field);
                while(res_row=mysql_fetch_row(res_ptr)) 
                {
                    //printf("YYY\n");
                    int i,s_id, e_id,I;
                    for(i = 0; i < 2; i++)
                    {
                        if(i == 0)
                        {
                            s_id = atoi(res_row[i]);
                        }
                        else if(i == 1)
                        {
                            e_id = atoi(res_row[i]);
                        }
                    }
                    sprintf(XZ->buf, "%s", res_row[i]);
                    XZ->zt = 1;     //加群
                    //ice++;
                    //XZ->ice = ice;
                    ///printf("ice = %d\n", ice);
                    send(sfd, XZ, sizeof(LIAOT), 0);
                    //printf("XZ->buf = %s\n", XZ->buf);
                    memset(XZ, 0, sizeof(LIAOT));
                    recv(sfd, XZ, sizeof(LIAOT), 0);
                    //mysql_free_result(res_ptr);
                    if(strcmp(XZ->buf, "Y") == 0)
                    {
                        time_t  timep_1;
                        time (&timep_1);
                        struct tm *p;
                        p = gmtime(&timep_1);
                        int day = (p->tm_year + 1900)*10000 + (p->tm_mon + 1)*100 + p->tm_mday;
                        int time = (p->tm_hour + 8)*10000 + (p->tm_min)*100 + p->tm_sec;
                        int ret_1, ret_2, ret_3;
                        sprintf(A, "insert into %sgrouptable (id, name, sf) values (%d, '%s', 0)", getname_from_id(s_id), e_id, getgroupname_from_id(e_id));
                        //printf("A = %s\n", A);
                        ret_1 = mysql_query(conn,A);        //添加到已加群表
                        sprintf(A, "insert into %scylb (id, name, sf, day, time) values (%d, '%s', 0, %d, %d)", getgroupname_from_id(e_id), s_id, getname_from_id(s_id), day, time);
                        //printf("A = %s\n", A);
                        ret_2 = mysql_query(conn,A);
                        sprintf(A, "delete from box where from_id = %d and end_id = %d and ice = 1", s_id, e_id);
                        //printf("A = %s\n", A);
                        ret = mysql_query(conn,A);        //删除该通知
                        if(ret_1 == 0 && ret_2 == 0 && ret == 0)
                            sprintf(B, "添加成功!\n");
                        else
                            sprintf(B, "添加失败!\n");
                        strncpy(XZ->buf, B, sizeof(B));
                        send(sfd, XZ, sizeof(LIAOT), 0);
                    }
                    else if(strcmp(XZ->buf, "N") == 0)
                    {
                        sprintf(A, "delete from box where from_id = %d and end_id = %d", s_id, e_id);
                        mysql_query(conn,A);        //删除该通知
                    }
                }
                //printf("x = %d\n", x);
            }
            memset(XZ, 0, sizeof(LIAOT));
            XZ->ice = 100;
            sprintf(XZ->buf, "over");
            send(sfd,XZ, sizeof(XZ), 0);
            mysql_free_result(res_ptr);
            free(XZ);
            return 0;                      //结束循环
        }
        else if(strcmp(B, "R") == 0)
        {
            return 0;                     //结束循环
        }

}

int set_1(int id)            //设置为忙碌
{
    char A[100];
    sprintf(A, "update student set sj = 1 where id = %d", id);
    int ret = mysql_query(conn,A);
    if(ret == 0)
    {
        return 0;
    }
    return 1;
}

int drop_1(int id)           //设置为空闲
{
    char A[100];
    sprintf(A, "update student set sj = 0 where id = %d", id);
    int ret = mysql_query(conn,A);
    if(ret == 0)
    {
        return 0;
    }
    return 1;
}

int panduan_1(int id)                  //判断客户是否空闲
{
    char A[100];
    int sj, res, field;
    //printf("HHH %d HHH\n", id);
    sprintf(A, "select sj from student where id = %d", id);
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res = mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
    if(res_row == NULL)
    {
        printf("NULL\n");
    }
    
    sj = atoi(res_row[0]);

    return sj;
}

int panduan_2();

int ADD(XINXI *YY, DENN *XX)        //添加好友
{
    int field, fd, ret;

    if(YY->y_id == XX->id)
    {
        //printf("YYY\n");
        ret = -2;
        return ret;
    }

    char B[50];
    sprintf(B, "用户%s请求添加你为好友！", XX->name);
    char A[100];
    //printf("id = %d\n", XZ->id); 
    sprintf(A,"select * from %shylb where id = %d", getname_from_id(XX->id), YY->y_id);
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    ret = mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    mysql_free_result(res_ptr);
    if(res_row != NULL)
    {
        ret = -1;            //该用户已经是好友
        return ret;
    }

    sprintf(A, "insert into box (from_id,end_id,message,ZT,ice,beizhu) values (%d,%d,'%s',0,0,'%s')", XX->id, YY->y_id, B, YY->beizhu);
    ret = mysql_query(conn,A);
    if(ret)
    {
        printf("wrong!\n");
        return 0;              //信息发送失败
    }
    else
    {
        return 1;              //信息发送成功
    }


    /*  
    sprintf(A, "select fd from student where id = %d", XZ->id);      //XZ->id出错

    int res = mysql_query(conn,A);
    if(res)
    {
        printf("wrong!!!\n");
    }
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res_ptr = mysql_store_result(conn);
    field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
    res_row=mysql_fetch_row(res_ptr);
    printf("B\n");
    if(res_row == NULL)
    {
        printf("over\n");
    }
    else
    {
        fd = atoi(res_row[0]);            //获取目标好友的套接字码
        printf("fd  = %d\n", fd);
    }
    */
}

  
int xuanzhe_1(DENN *XX, XINXI *YY, int sfd)
{   
    int ret;
    char buf[50],A[100];
    LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
    

        if(YY->ice_2 == 21)           //好友的添加，删除，查询
        {
            char B[50];
            if(YY->ice_3 == 211)                //添加好友
            {
                //printf("sfd = %d\n", sfd);
                ret = ADD(YY, XX);
                if(ret == 1)
                {
                    sprintf(B, "信息发送成功！\n");
                }
                else if(ret == 0)
                {
                    sprintf(B, "信息发送失败！\n");
                }
                else if(ret == -1)
                {
                    sprintf(B, "该用户已经是好友，无需再添加!\n");
                }
                else if(ret == -2)
                {
                    sprintf(B, "该ID为当前帐号！\n");
                }
                write(sfd, B, sizeof(B));
            }
            else if(YY->ice_3 == 212)
            {
                    if(YY->zt == 1)   //根据ID删除好友
                    {
                        int ret_1,ret_2,ret_3;
                        sprintf(A, "delete from %shylb where id = %d", getname_from_id(XX->id), YY->y_id);
                        ret_1 = mysql_query(conn,A);
                        sprintf(A, "delete from %shylb where id = %d", getname_from_id(YY->y_id), XX->id);    
                        ret_2 = mysql_query(conn,A);
                        sprintf(A, "drop table %s", getjl_from_id(XX->id, YY->y_id));
                        ret_3 = mysql_query(conn,A);
                        if(ret_1 == 0 && ret_2 == 0 && ret_3 == 0) 
                        {
                            sprintf(buf, "删除成功!\n");
                            write(sfd, buf, sizeof(buf));
                        }
                        else
                        {
                            sprintf(buf, "删除失败!\n");
                            write(sfd, buf, sizeof(buf));
                        }
                    }
                    else if(YY->zt == 2)   //根据备注删除好友
                    {
                        int ID = getid_from_beizhu(XX->id, YY->beizhu);
                        YY->y_id = ID;
                        sprintf(A, "delete from %shylb where id = %d", getname_from_id(XX->id), YY->y_id);
                        ret = mysql_query(conn,A);
                        sprintf(A, "delete from %shylb where id = %d", getname_from_id(YY->y_id), XX->id);
                        if(ret)
                            ret = mysql_query(conn,A);
                        sprintf(A, "drop table %s", getjl_from_id(XX->id, YY->y_id));
                        if(ret)
                            ret = mysql_query(conn,A);

                        if(ret == 0)
                        {
                            sprintf(buf, "删除成功!\n");
                            write(sfd, buf, sizeof(buf));
                        }
                        else
                        {
                            sprintf(buf, "删除失败!\n");
                            write(sfd, buf, sizeof(buf));
                        }
                    }
            }
            else if(YY->ice_3 == 213)
            {
                    if(YY->zt == 1) //根据ID查询备注
                    {
                        sprintf(A, "select beizhu from %shylb where id = %d", getname_from_id(XX->id), YY->y_id);
                        ret = mysql_query(conn,A);
                        MYSQL_RES *res_ptr;
                        MYSQL_ROW  res_row;
                        res_ptr = mysql_store_result(conn);
                        res_row = mysql_fetch_row(res_ptr);
                        if(res_row != NULL)
                        {
                            sprintf(buf, "ID为%d的好友备注为：%s\n", YY->y_id, res_row[0]);
                            write(sfd,buf, sizeof(buf));
                        }
                        mysql_free_result(res_ptr);
                    }
                    else if(YY->zt == 2) //根据备注查询ID
                    {
                        sprintf(A, "select id from %shylb where beizhu = '%s'", getname_from_id(XX->id), YY->beizhu);
                        ret = mysql_query(conn,A);
                        MYSQL_RES *res_ptr;
                        MYSQL_ROW  res_row;
                        res_ptr = mysql_store_result(conn);
                        res_row = mysql_fetch_row(res_ptr);
                        if(res_row != NULL)
                        {
                            sprintf(buf,"备注为%s的好友ID为：%s\n", YY->beizhu, res_row[0]);
                            write(sfd,buf, sizeof(buf));
                        }
                        mysql_free_result(res_ptr);
                    }
            }
        }
        else if(YY->ice_2 == 22)          //查看好友列表
        {
            int field;
            char B[50], A[100];
            sprintf(A, "select * from %shylb", getname_from_id(XX->id));
            int res = mysql_query(conn,A);
            if(res)
            {
                printf("wrong!!!\n");
            }
            MYSQL_RES *res_ptr;
            MYSQL_ROW  res_row;
            res_ptr = mysql_store_result(conn);
            field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
            //printf("field = %d\n", field);
            memset(B, 0, sizeof(B));
            while(res_row=mysql_fetch_row(res_ptr) )
	        {
		        for(int i=0;i<2;i++)
    		    {
                    if(i != 2)
                    {
                        //printf("%s\n", res_row[i]);
    	    		    strncat(B, res_row[i],strlen(res_row[i]));
                        strcat(B, "\t");
                    }
                }
    		    write(sfd, B, sizeof(B));
                memset(B, 0, sizeof(B));
	        }
            //printf("CCC\n");
            sprintf(B, "over");
            write(sfd, B, sizeof(B));
            mysql_free_result(res_ptr);
        }
        else if(YY->ice_2 == 23)             //查看好友状态
        {
            int field;
            char A[100];
            MYSQL_RES *res_ptr;
            MYSQL_ROW  res_row;
            sprintf(A, "select zt from student where id = %d", YY->y_id);
            int res = mysql_query(conn,A);
            if(res)
            {
                printf("wrong!!!\n");
            }
            res_ptr = mysql_store_result(conn);
            field = mysql_num_fields(res_ptr);
            res_row=mysql_fetch_row(res_ptr);
            if(res_row != NULL)
            {
                XZ->zt = atoi(res_row[0]);
                //printf("XZ->zt = %d\n", XZ->zt);
            }
            mysql_free_result(res_ptr);
            sprintf(A, "select * from %shylb where id = %d", getname_from_id(XX->id), YY->y_id);
            //printf("A = %s\n", A);
            ret = mysql_query(conn,A);
            res_ptr = mysql_store_result(conn);
            res_row = mysql_fetch_row(res_ptr);
            if(res_row == NULL)
            {
                XZ->zt = -1;            //没有此ID的好友
            }
            mysql_free_result(res_ptr);
            
            send(sfd, XZ, sizeof(LIAOT), 0);
        }
        else if(YY->ice_2 == 24)             //查看聊天记录
        {
            //printf("AAAAAAAAAAAAAAAAAA\n");
            int field;
            char A[100];
            MYSQL_RES *res_ptr;
            MYSQL_ROW  res_row;
            //printf("XX->id = %d\tYY->y_id = %d\n",XX->id,YY->y_id);
            sprintf(A, "select * from %s", getjl_from_id(XX->id, YY->y_id));
            int res = mysql_query(conn,A);
            if(res)
            {
                printf("wrong!!!\n");
            }
            res_ptr = mysql_store_result(conn);
            field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
            //printf("field = %d\n", field);
            while(res_row=mysql_fetch_row(res_ptr))
            {
                for(int i = 0;i<field;i++)
                {
                    if(i == 0)
                    {
                        //printf("11111111111\n");
                        XZ->id = atoi(res_row[i]);
                        //printf("2222222222222\n");
                        strncpy(XZ->beizhu, getbeizhu_from_id(XX->id,XZ->id),sizeof(XZ->beizhu));
                        //printf("33333333333\n");
                    }
                    if(i == 1)
                    {
                        //printf("444444444444\n");
                        strncpy(XZ->xinxi, res_row[i], sizeof(XZ->xinxi));
                        //printf("5555555555555\n");
                    }
                }
                send(sfd, XZ, sizeof(LIAOT), 0);
            }
            //printf("over\n");
            sprintf(XZ->xinxi, "over");
            send(sfd, XZ, sizeof(LIAOT), 0);
            mysql_free_result(res_ptr);
        }
        else if(YY->ice_2 == 25)              //屏蔽好友信息 或取消屏蔽
        {
            int field;
            sprintf(A, "select * from %shylb where id = %d",getname_from_id(XX->id), YY->y_id);
            MYSQL_RES *res_ptr;
            MYSQL_ROW  res_row;
            ret = mysql_query(conn,A);
            if(ret)
            {
                printf("wrong!!!\n");
            }
            res_ptr = mysql_store_result(conn);
            field = mysql_num_fields(res_ptr); 
            res_row=mysql_fetch_row(res_ptr);
            if(res_row != NULL)
            {
                if(YY->ice_3 == 251)
                {
                    sprintf(A, "update %shylb set zt = -1 where id = %d", getname_from_id(XX->id), YY->y_id);
                    ret = mysql_query(conn,A);
                    sprintf(buf, "屏蔽成功!\n");
                }
                if(YY->ice_3 == 252)
                {
                    sprintf(A, "update %shylb set zt = 0 where id = %d", getname_from_id(XX->id), YY->y_id);
                    ret = mysql_query(conn,A);
                    sprintf(buf, "取消屏蔽成功!\n");
                }
            }
            else
            {
                sprintf(buf, "你无此ID的好友!\n");
            }   
            write(sfd, buf, sizeof(buf));
            mysql_free_result(res_ptr);  
        }
    
    free(XZ);
    
    return 0;
}


void HY_send(XINXI *YY,int sfd)                       //将未读信息发送给客户端
{
    //printf("BBBA\n");
    int ret,sum = 0;
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
    char A[100],B[50],beizhu[20];
    strncpy(YY->jl, getjl_from_id(YY->m_id, YY->y_id), sizeof(YY->jl));
    //printf("YY->jl = %s\tYY->m_id = %d\n",YY->jl,YY->m_id);
    sprintf(A, "select xinxi from %s where end_id = %d and zt = 1", YY->jl, YY->m_id);
    ret = mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    while(res_row = mysql_fetch_row(res_ptr))
    {
        sum++;
    }
    XZ->zt = sum;    
    send(sfd, XZ, sizeof(LIAOT), 0);
    if(sum > 0)
    {
        mysql_free_result(res_ptr);
        ret = mysql_query(conn,A);
        res_ptr = mysql_store_result(conn);
        while(res_row = mysql_fetch_row(res_ptr))
        {
            //printf("res_row: %s\n", res_row[0]);
            strncpy(XZ->beizhu, getbeizhu_from_id(YY->m_id,YY->y_id), sizeof(XZ->beizhu));
            strncpy(XZ->xinxi, res_row[0], sizeof(XZ->xinxi));
            send(sfd, XZ, sizeof(LIAOT), 0);
            sprintf(A, "update %s set zt = 0 where zt = 1 and end_id = %d and xinxi = '%s'", YY->jl, YY->m_id, XZ->xinxi);
            ret = mysql_query(conn,A);      //将未读更新为已读
            //printf("ret = %d", ret);
            //printf("%s : %s\n", XZ->beizhu, XZ->xinxi);
        }
        mysql_free_result(res_ptr);
    }
    /*
    if(sum > 0)
    {
        sprintf(A, "update %s set zt = 0 where zt = 1", YY->jl);
        ret = mysql_query(conn,A);      //将未读更新为已读
        //printf("ret = %d", ret);
    }
    */
    
    free(XZ);
    //printf("AAA\n");
}

void HY_get(XINXI *YY, int id, int sfd)
{
    //printf("CCCCCCCCCCCcCCCCC\n");
    char A[100];//B[50],buf[50],beizhu[20];
    int ret;//,field,zt;   
    //TX(YY);
    //printf("YY->buf: %s\n", YY->buf);
    if(strcmp(YY->buf, "exit") == 0) 
    {
        //pthread_join(thid, NULL);
        return;
    }
    sprintf(A, "select * from %shylb where id = %d and zt = 0", getname_from_id(YY->m_id), YY->y_id);
    mysql_query(conn,A);
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    mysql_free_result(res_ptr);
    if(res_row != NULL)
    {
        sprintf(A, "select * from %shylb where id = %d and zt = 0", getname_from_id(YY->y_id), YY->m_id);
        mysql_query(conn,A);
        MYSQL_RES *res_ptr;
        MYSQL_ROW  res_row;
        res_ptr = mysql_store_result(conn);
        res_row = mysql_fetch_row(res_ptr);
        mysql_free_result(res_ptr);
        if(res_row != NULL)
        {
            sprintf(A, "insert into %s (from_id, xinxi, end_id, zt) values (%d, '%s', %d, 1)",getjl_from_id(id,YY->y_id), id, YY->buf, YY->y_id);
            //printf("%s\n",A);
            
            ret = mysql_query(conn,A);
            //printf("ret = %d\n\n\n", ret);
            //printf("DDDDDDDDDDDDDDDDDDDDDDDD\n");
        }
    }

}


int xuanzhe_2(DENN *XX, XINXI *YY, int sfd)
{
    int id, ret;
    char buf[50], A[100];
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    //获取未浏览信息数量

    YY->m_id = XX->id;

    //ret = recv(sfd, XZ, sizeof(LIAOT), 0);
        if(YY->ice_2 == 31)
        {
            //判断是否存在该好友
            //printf("name: %s\tYY->y_id = %d\n", getname_from_id(XX->id), YY->y_id);
            strncpy(YY->name, getname_from_id(XX->id), sizeof(YY->name));
            sprintf(A, "select * from %shylb where id = %d", YY->name, YY->y_id);
            ret = mysql_query(conn,A);
            //printf("rett = %d\n", ret);
            if(!ret)
            {
                res_ptr = mysql_store_result(conn);
                res_row = mysql_fetch_row(res_ptr);
                if(res_row != NULL)         //存在该好友
                {
                    mysql_free_result(res_ptr);
                    //printf("YY->m_id = %d\n", YY->m_id);
                    //printf("YY->y_id = %d\n", YY->y_id);
                    strncpy(YY->jl, getjl_from_id(YY->m_id, YY->y_id), sizeof(YY->jl));
                    strncpy(YY->beizhu, getbeizhu_from_id(YY->m_id, YY->y_id), sizeof(YY->beizhu));

                    sprintf(buf, "OK");
                    write(sfd, buf, sizeof(buf));
                    sleep(1);
                    //HY_send(YY,sfd);     //将未读信息发送个客户端
                    S_FD = sfd;
                    S_ID = YY->y_id;
                    E_ID = XX->id;
                    //pthread_create(&thid, NULL, thread, NULL);
                    return 0;                 //回到epoll，让epoll来随时接受信息
                }
                else if(res_row == NULL)                       //不存在该好友
                {
                    sprintf(buf, "不存在该好友!\n");
                    write(sfd, buf, sizeof(buf));
                }
                mysql_free_result(res_ptr);
            }
        }
}

int  denglu(XINXI *YY,int sfd)            //登陆
{
    int row, res, ret;
    char A[100],B[50];
    //printf("ID = %d\n", XX->id);
    sprintf(A, "select zt from student where id = %d", YY->m_id);
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res = mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    mysql_free_result(res_ptr);
    if(res_row == NULL)
    {
        //id错误
        strncpy(B,"无此id",50);
        ret = 0;
    }
    else if(atoi(res_row[0]) == 1)
    {
        sprintf(B, "该帐号已在其他地方登陆！");
    }
    else
    {
        sprintf(A, "select id from student where id = %d and password = %s", YY->m_id, YY->password);
        res = mysql_query(conn,A);
        res_ptr = mysql_store_result(conn);
        res_row = mysql_fetch_row(res_ptr);                   
        if(res_row == NULL)
        {
        //password错误
            strncpy(B,"密码错误",50);
            ret = 0;
        }
        else
        {
            sprintf(A, "update student set zt=1 where id = %d", YY->m_id);        //在线状态
            mysql_query(conn, A);
            sprintf(A, "update student set fd=%d where id = %d", sfd, YY->m_id);        //在线状态
            mysql_query(conn, A);
            strncpy(B,"登陆成功",50);
            ret = 1;
        }
        mysql_free_result(res_ptr);
    }
    write(sfd, B, sizeof(B));

    return ret;
}

void logof(XINXI *YY, int sfd)
{
    int ret_1, ret_2;
    char A[100];
    
    sprintf(A, "update student set zt = 0 where id = %d", YY->m_id);
    ret_1 = mysql_query(conn,A);        //用户退出，将状态设置为不在线
    sprintf(A, "update student set fd = 0 where id = %d",YY->m_id);
    ret_2 = mysql_query(conn,A);        //用户退出后将fd归

    if(ret_1 != 0 || ret_2 != 0)
    {
        printf("退出登陆信息更新失败!!!\n");
    }
}

int zhuce(XINXI *YY,int sfd)        //注册
{
    int res, ret, ret_1, ret_2,ret_3;
    char A[100],B[50];
    //printf("YY->m_id = %d\n", YY->m_id);
    sprintf(A, "select id from student where id = %d", YY->m_id);
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res = mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    mysql_free_result(res_ptr);
    if(res_row)
    {
        //id已经存在
        ret = 0;
        strncpy(B,"id已经被注册，请更换一个id",50);
    }
    else
    {
        sprintf(A, "select id from student where id = %d and name = %s", YY->m_id, YY->name);
        res = mysql_query(conn,A);
        res_ptr = mysql_store_result(conn);
        res_row = mysql_fetch_row(res_ptr);
        mysql_free_result(res_ptr);
        if(res_row)                      //name已存在
        {
            ret = 0;
            strncpy(B, "name已经被注册，请更换一个name", 50);
        }
        else
        {
            //strncpy(YY->hylb, YY->name, sizeof(YY->hylb));
            //printf("AAA\n");
            sprintf(A, "insert into student (id,password,name,qu,an,hylb,zt,sj,grouptable) values (%d, '%s', '%s', '%s', '%s', '%shylb', 0, 0, '%sgrouptable')",YY->m_id, YY->password,YY->name,YY->qu,YY->an, YY->name, YY->name);
            //printf("%s\n",A);
            //return 0;
            ret_1 = mysql_query(conn,A);
            //printf("ret_1 = %dBBB\n", ret_1);
            sprintf(A, "create table %s (id int, beizhu varchar(20), jl varchar(20), zt int)", YY->hylb);
            ret_2 = mysql_query(conn,A);           //创建好友列表
            //printf("ret_2 = %dCCC\n", ret_2);
            sprintf(A, "create table %sgrouptable (id int, name varchar(20),sf int)", YY->name);
            ret_3 = mysql_query(conn,A);           //创建已加群列表
            //printf("ret_3 = %dDDD\n", ret_3);
            if(ret_1 == 0 && ret_2 == 0 && ret_3 == 0)
            {
                ret = 1;
                sprintf(B,"注册成功");
            }
            else
            {
                sprintf(B, "注册失败");
                ret = 0;
            }
        }  
    }
    //printf("A\n");
    write(sfd, B, sizeof(B));

    return ret;
}

int zhaohui(XINXI *YY,int sfd)           //找回密码
{
    int res, field, ret;
    char A[200],B[50],C[100];
    sprintf(A, "select qu from student where id = %d", YY->m_id);
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    res = mysql_query(conn,A);
    res_ptr = mysql_store_result(conn);
    res_row = mysql_fetch_row(res_ptr);
    field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
    //printf("AA%dAA\n", field);
    //printf("%s\n",res_row[0]);
    strncpy(A,res_row[0],200);
    write(sfd, A, sizeof(A));               //将密保问题发送到客户端
    mysql_free_result(res_ptr);

    return 1;
}

int zhaohui_1(XINXI *YY,int sfd)           //找回密码
{
    int ret,field;
    char A[100],B[50];
    MYSQL_RES *res_ptr;
    MYSQL_ROW  res_row;
    sprintf(A,"select an from student where id = %d AND an = '%s'",YY->m_id, YY->an);
    //printf("%d\t%s\n",XX->id,XX->an);
    mysql_query(conn, A);
    res_ptr = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res_ptr);
    mysql_free_result(res_ptr);

    if(row == NULL)
    {
        strncpy(B,"答案错误",50);
        ret = 0;
    }
    else
    {
        sprintf(A, "select password from student where an = '%s'", YY->an);
        ret = mysql_query(conn,A);
        res_ptr = mysql_store_result(conn);
        res_row = mysql_fetch_row(res_ptr);
        field = mysql_num_fields(res_ptr);      //返回你这张表有多少列
        sprintf(B,"密码为：%s",res_row[0]);
        mysql_free_result(res_ptr);
        ret = 1;
    }
    write(sfd, B, sizeof(B));        //将密码发送到客户端

    return ret;
}

void TX(XINXI *YY)
{
    printf("ice_1 = %d\n", YY->ice_1);
    printf("ice_2 = %d\n", YY->ice_2);
    printf("ice_3 = %d\n", YY->ice_3);
    printf("ice_4 = %d\n", YY->ice_4);
    printf("zt    = %d\n", YY->zt);
    printf("m_id  = %d\n", YY->m_id);
    printf("y_id  = %d\n", YY->y_id);
    printf("q_id  = %d\n", YY->q_id);
    //printf("name = %s\n", YY->name);
    printf("password = %s\n", YY->password);
    //printf("buf = %s\n", YY->buf);
    printf("*****************\n");
}