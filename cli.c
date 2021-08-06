#include "cli.h"

void get_XX(int socket_fd)
{
	XINXI YY;
	char buf[50];
	YY.m_id = M_ID;
	YY.ice_1 = 555;
	send(socket_fd, &YY, sizeof(XINXI), 0);

	do
	{
	
		recv(socket_fd, buf, sizeof(buf), 0);
		if(strcmp(buf, "over") == 0)
		{
			break;
		}
		printf("\t\t%s\n", buf);
	}while(1);
	//printf("SSSSSS\n");
}

void C_document(XINXI *YY, int socket_fd)
{
	LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
	int n, fd, ret, id;
	char file_len[16], file_name[128],file_path[128],sign[10];
	char buf[1024];
	setbuf(stdin, NULL);
	printf("\t\t********************1 文件发送 *********************\n");
	printf("\t\t********************2 文件接收 *********************\n");
	printf("\t\t********************0 退出     *********************\n");
	printf("\t\t请输入你的选择：");
	scanf("%d", &n);
	printf("\t\t请输入好友ID：");
	scanf("%d", &id);
	YY->y_id = id;
	if(n == 0)
	{
		return ;
	}
	else if(n == 1)     //文件发送
	{

		setbuf(stdin, NULL);
		printf("\t\t请输入文件：");
		scanf("%s", &file_path);
		memset(file_name, 0, sizeof(file_name));
		strncpy(file_name, basename(file_path), sizeof(file_name));

		fd = open(file_path, O_RDWR);
		if(fd == -1)
		{
			printf("\t\t打开文件失败！！！\n");
			return ;
		}

		int len = lseek(fd, 0, SEEK_END);

		lseek(fd, 0, SEEK_SET);   //文件光标移动到开始位置

		YY->ice_2 = 71;
		sprintf(YY->buf, "%d", len);
		strncpy(YY->qu, file_name, sizeof(file_name));

		send(socket_fd, YY, sizeof(XINXI),0);

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
			
			send(socket_fd, D, sizeof(DOC), 0);

			recv(socket_fd, sign, sizeof(sign), 0);
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
					send(socket_fd, D, sizeof(DOC), 0);

					recv(socket_fd, sign, sizeof(sign), 0);

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
			//sleep(1);
		}
		printf("\t\tlen = %d\n\t\tsend_len = %d\n", len, send_len);
	}
	else if(n == 2)     //文件接收
	{
		YY->ice_2 = 72;
		send(socket_fd, YY, sizeof(XINXI),0);

		int ret, fd, sum,i;
    	char file_len[16], file_name[128], buf[1024],file_new_name[128],sign[10];

		read(socket_fd, &sum, sizeof(int));
		printf("\t\t有%d份文件需要接收！！！\n", sum);

		if(sum == 0)
		{
			return ;
		}
		
		for(i = 0; i < sum; i++)
		{

			printf("\t\t等待文件传入！\n");

			recv(socket_fd, XZ, sizeof(LIAOT), 0);

			strncpy(file_len, XZ->beizhu, sizeof(file_len));
			strncpy(file_name, XZ->xinxi, sizeof(file_name));

			printf("\t\t文件名：%s\n文件大小：%s\n", file_name, file_len);

			sprintf(file_new_name, "c-%s", file_name);
			//printf("\t\t%s", file_new_name);

			fd = open(file_new_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
			
			//printf("fd = %d\n", fd);

			int size = atoi(file_len), r_ice = 1;

			int write_len = 0, s_size = 1024;

			DOC *D = (DOC*)malloc(sizeof(DOC));
			
			while(1)
			{
				memset(D, 0, sizeof(DOC));
				recv(socket_fd, D, sizeof(DOC), 0);
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
				send(socket_fd, sign, sizeof(sign), 0);
				if(write_len >= size)
				{
					close(fd);
					break;
				}
				
			}

			printf("\t\tsize = %d\nwrite_len = %d\n", size, write_len);
		}
	}
	free(XZ);
	//return;
	printf("\t\t请输入enter继续!!!\n");
	setbuf(stdin, NULL);
	getchar();
	setbuf(stdin, NULL);
	//printf("AAAAAAA\n");
}

void *thread_g(void *arg)
{
	int n;
	LIAOT XZ;
	XINXI YY;
	do
	{
		sleep(5);
		//printf("AAAAAA\n");
		//pthread_mutex_lock(&lock);              //加锁
		YY.ice_4 = 999;
		YY.ice_1 = 5;
		YY.m_id = M_ID;
		//printf("mid = %d\tyid = %d\n",M_ID,YY->y_id);
		YY.q_id = Q_ID;
		send(Socket_fd, &YY, sizeof(XINXI), 0);

		recv(Socket_fd, &n, sizeof(n), 0);
		//printf("n = %d\n", n);
		while(n > 0)
		{
			recv(Socket_fd, &XZ, sizeof(LIAOT), 0);
			//printf("XZ->zt = %d\n",XZ.zt);
			if(XZ.zt > 0)
			{
				printf("\b\b\b\b\b\b                          \n");
				printf("\t\t\033[34m%s : %s\033[0m", XZ.name, XZ.xinxi);
			}
			--n;
			if(n == 0)
			{
				printf("\t\t\033[31mMINE :");
			}
		}
		
	}while(M == 1);
}

void C_group_com(XINXI *YY, DENN *XX, int socket_fd)      //群聊天
{
	pthread_t thid;
	int id,ret,ic,N = 0;
	char beizhu[20],buf[50];

		LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
		get_XX(socket_fd);
		printf("\t\t**************************************\n");
		printf("\t\t*************0 退出 *******************\n");
		printf("\t\t*************1 群聊天 **************\n");
		printf("\t\t请输入你的选择：");
		scanf("%d", &ic);
		printf("\033c");
		if(ic == 0)
		{
			return;
		}
		else if(ic == 1)
		{
			YY->ice_2 = 41;
			YY->ice_4 = 0;
			printf("\t\t请输入群ID:");
			scanf("%d",&id);
			YY->q_id = id;
			YY->m_id = M_ID;
			send(socket_fd,YY, sizeof(XINXI), 0);

			read(socket_fd,buf, sizeof(buf));
			if(strcmp(buf, "ok") != 0)
			{
				printf("\t\t%s",buf);
				return ;
			}
			//fflush(stdin);
			Q_ID = id;
			
			
			//pthread_mutex_init(&lock, NULL);                   //初始化锁
    		//pthread_cond_init(&cond, NULL);                    //初始化条件变量

			M = 1;
			if(pthread_create(&thid, NULL, thread_g, NULL) != 0)
			{
				//创建失败
				printf("\t\t创建线程失败！\n");
				return ;
			}
			else
			{
				//printf("\t\t创建成功！\n");
			}

			printf("\t\t\033[31mMINE :");
			setbuf(stdin, NULL);
			do{
				//要监视的描述符集合
				fd_set fds;
				FD_ZERO(&fds);                  //清空文件描述符集合
		
				FD_SET(0,&fds);                 //把标准输入设备加入到集合中 
		
				//FD_SET(socket_fd,&fds);         //把网络通信文件描述符加入到集合中 

				ret = select(socket_fd+1,&fds,NULL,NULL,NULL);
		 		if(ret < 0)//错误
				{
					perror("select fail:");
					return ;
				}
				else if(ret > 0) //有活跃的
				{
					//判断是否 标准输入设备活跃 假设是则发送数据
					if(FD_ISSET(0,&fds))
					{
						N = 1;
						int i = 0;
						char Buf[200] = {0},ch;
						setbuf(stdin, NULL);
						fgets(Buf,200,stdin);
						setbuf(stdin, NULL);
				
						YY->ice_4 = 888;
						YY->m_id  = M_ID;
						strncpy(YY->buf, Buf, sizeof(Buf));
						//printf("buf: %s\n", YY->buf);
						//printf("ice_1 = %d\n",YY->ice_1);
						if(strcmp(YY->buf, "exit\n") == 0)
						{
							//printf("杀死线程\n");
							printf("\033[0m\n");
							M = 0;
							pthread_join(thid, NULL);   //销毁线程
							return ;
						}
						if(strcmp(YY->buf, "\n") != 0)
							ret = send(socket_fd, YY, sizeof(XINXI), 0);
						//printf("buf = %sA\n", buf);
						setbuf(stdin, NULL);
						printf("\t\t\033[31mMINE: ");
						N = 0;
					}
				}
				//pthread_mutex_unlock(&lock);
			}while(1);
		}
}

void C_group(XINXI *YY, DENN *XX, int socket_fd)
{
	int ret,ic,id;
	char ch;
	char A[100],name[20], buf[50],qu[200],an[100];
	LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
	do
	{
		setbuf(stdin, NULL);
		printf("\033c");
		get_XX(socket_fd);
		printf("\t\t*******************************************\n");
		printf("\t\t***************0 退出 **********************\n");
		printf("\t\t***************1 群的创建，解散***************\n");
		printf("\t\t***************2 申请加群，退群***************\n");
		printf("\t\t***************3 查看已加群和群成员************\n");
		printf("\t\t***************4 查看聊天记录*****************\n");
		printf("\t\t***************5 设置管理员*******************\n");
		printf("\t\t***************6 踢人 ***********************\n");
		printf("\t\t请输入你的选择：");
		setbuf(stdin, NULL);
		scanf("%d", &ic);
		printf("\033c");
		if(ic == 0)         //退出循环
		{
			break;
		}
		else if(ic == 1)    //群的创建，解散
		{
			YY->ice_2 = 31;
			printf("\t\t*******************************************\n");
			printf("\t\t***************R 退出 **********************\n");
			printf("\t\t***************A 创建***********************\n");
			printf("\t\t***************B 解散***********************\n");
			printf("\t\t请输入你的选择：");
			setbuf(stdin, NULL);
			scanf("%s", &buf);
			printf("\033c");
			if(strcmp(buf, "A") == 0)
			{
				YY->ice_3 = 311;
				YY->m_id  = M_ID;
				printf("\t\t群ID：");
				scanf("%d", &id);
				YY->q_id = id;
				printf("\t\t群名：");
				scanf("%s", &name);
				//printf("name = %s\n", name);
				strncpy(YY->name, name, sizeof(YY->name));
				//printf("YY->name = %s\n",YY->name);
				printf("\t\t群类型：[A] 加群需要管理员同意加群\n");
				printf("\t\t       \t[B] 加群需要回答问题加群\n");
				printf("\t\t       \t[C] 加群无需管理员同意\n");
				printf("\t\t       \t[R] 退出\n");
				printf("\t\t群类型：");
				//fflush(stdin);
				scanf("%s", &buf);
				if(strcmp(buf, "A") == 0)
				{
					YY->zt = 1;
				}
				else if(strcmp(buf, "B") == 0)
				{
					YY->zt = 2;
					printf("\t\t问题：");
					scanf("%s", &qu);
					strncpy(YY->qu, qu, sizeof(qu));
					printf("\t\t答案：");
					scanf("%s",&an);
					strncpy(YY->an, an, sizeof(an));
				}
				else if(strcmp(buf, "C") == 0)
				{
					YY->zt = 0;
				}
				else if(strcmp(buf, "R") == 0)
				{
					continue;
				}
				send(socket_fd, YY, sizeof(XINXI), 0);
				read(socket_fd, buf, sizeof(buf));
				printf("%s\n", buf);
				printf("\t\t请输入enter继续!!!\n");
				setbuf(stdin, NULL);
				getchar();
				setbuf(stdin, NULL);
			}
			else if(strcmp(buf, "B") == 0)
			{
				YY->ice_3 = 312;
				YY->m_id  = M_ID;
				printf("\t\t群ID：");
				scanf("%d", &id);
				YY->q_id = id;
				send(socket_fd, YY, sizeof(XINXI),0);
				//printf("AAAA\n");
				read(socket_fd,buf, sizeof(buf));
				printf("\t\t%s\n",buf);
				printf("\t\t请输入enter继续!!!\n");
				setbuf(stdin, NULL);
				getchar();
				setbuf(stdin, NULL);
			}
			else if(strcmp(buf, "R") == 0)
			{
				continue;
			}
		}
		else if(ic == 2)        //申请加群退群
		{
			YY->ice_2 = 32;
			YY->m_id  = M_ID;
			printf("\t\t[A] 加群\n");
			printf("\t\t[B] 退群\n");
			printf("\t\t[R] 退出\n");
			printf("\t\t请输入你的选择：");
			setbuf(stdin, NULL);
			scanf("%s", &buf);
			printf("\033c");
			if(strcmp(buf, "A") == 0)
			{
				YY->ice_3 = 321;
				printf("\t\t请输入群ID：");
				scanf("%d", &id);
				YY->q_id = id;
				send(socket_fd, YY, sizeof(XINXI),0);
				recv(socket_fd, XZ, sizeof(LIAOT), 0);
				if(XZ->zt == 100)     //回答问题加入
				{
					printf("\t\t回答正确问题才能加入！！！\n");
					printf("\t\t问题：%s\n", XZ->qu);
					printf("\t\t答案：");
					scanf("%s", &an);
					if(strcmp(an, XZ->an) == 0)
					{
						YY->ice_1 = 3;
						YY->ice_4 = 3211;
						YY->zt = 1;          //1为回答正确
					}
					else
					{
						printf("\t\t答案错误！！！\n");
					}
				}
				else
				{
					printf("\t\t%s\n", XZ->buf);
				}
				printf("\t\t请输入enter继续!!!\n");
				setbuf(stdin, NULL);
				getchar();
				setbuf(stdin, NULL);
			}
			else if(strcmp(buf, "B") == 0)
			{
				YY->ice_3 = 322;
				printf("\t\t请输入群ID：");
				scanf("%d", &id);
				YY->q_id = id;
				send(socket_fd, YY, sizeof(XINXI),0);
				recv(socket_fd, buf, sizeof(buf), 0);
				printf("\t\t%s\n", buf);
				printf("\t\t请输入enter继续!!!\n");
				setbuf(stdin, NULL);
				getchar();
				setbuf(stdin, NULL);
			}
			else if(strcmp(buf, "R") == 0)
			{
				continue;
			}
		}
		else if(ic == 3)        //查看已加群和群成员
		{
			YY->ice_2 = 33;
			YY->m_id  = M_ID;
			do
			{
				printf("\033c");
				printf("\t\t[A] 查看已加群\n");
				printf("\t\t[B] 查询群成员\n");
				printf("\t\t[R] 退出\n");
				printf("\t\t请输入你的选择：");
				setbuf(stdin, NULL);
				scanf("%s", &buf);
				printf("\033c");
				if(strcmp(buf, "A") == 0)
				{
					YY->ice_3 = 331;
					send(socket_fd, YY, sizeof(XINXI),0);
					recv(socket_fd, buf, sizeof(buf), 0);
					if(strcmp(buf, "ok") == 0)
					{
						do
						{
							recv(socket_fd, buf, sizeof(buf), 0);
							if(strcmp(buf, "over") != 0)
							{
								printf("\t\t%s\n", buf);
							}
							else
							{
								break;
							}
						}while(1);
					}
				}
				else if(strcmp(buf, "B") == 0)
				{
					YY->ice_3 = 332;
					printf("\t\t群ID：");
					scanf("%d", &id);
					YY->q_id = id;
					send(socket_fd, YY, sizeof(XINXI),0);
					//recv(socket_fd, buf, sizeof(buf), 0);
					do
					{
						recv(socket_fd, buf, sizeof(buf), 0);
						if(strcmp(buf, "over") != 0)
						{
							printf("\t\t%s\n", buf);
						}
						else
						{
							break;
						}
					}while(strcmp(buf, "over") != 0);
				}
				else if(strcmp(buf, "R") == 0)
				{
					break;
				}
				printf("\t\t请输入enter继续!!!\n");
				setbuf(stdin, NULL);
				getchar();
				setbuf(stdin, NULL);
			} while (1);
		}
		else if(ic == 4)        //查看聊天记录
		{
			YY->ice_2 = 34;
			YY->m_id  = M_ID;
			printf("\t\t群ID：");
			scanf("%d",&id);
			YY->q_id = id;
			send(socket_fd, YY, sizeof(XINXI),0);
			int DAY = 0, year, month, day;
			recv(socket_fd, XZ, sizeof(LIAOT), 0);
			if(strcmp(XZ->buf, "ok") == 0)
			{
				do
				{
					recv(socket_fd, XZ, sizeof(LIAOT), 0);
					if(XZ->zt == 0)
					{
						break;
					}
					if(strcmp(XZ->xinxi,"over") != 0)
					{
						year  = XZ->zt/10000;
						month = (XZ->zt - year*10000)/100;
						day   = XZ->zt - year*10000 - month*100;
						if(DAY == 0)
						{
							DAY = XZ->zt;
							printf("\t\t\t\033[31m%d年%d月%d日\n\033[0m", year, month, day);
						}
						else if(DAY != XZ->zt)
						{
							DAY = XZ->zt;
							printf("\t\t\t\033[31m%d年%d月%d日\n\033[0m", year, month, day);
						}
						printf("\t\t%s :%s\n\n",XZ->name,XZ->xinxi);
					}
				}while(strcmp(XZ->xinxi,"over") != 0);
			}
			printf("\t\t请输入enter继续!!!\n");
			setbuf(stdin, NULL);
			getchar();
			setbuf(stdin, NULL);
		}
		else if(ic == 5)        //设置管理员
		{
			YY->ice_2 = 35;
			YY->m_id  = M_ID;
			printf("\t\t群ID：");
			scanf("%d", &id);
			YY->q_id = id;
			printf("\t\t群成员ID：");
			scanf("%d", &id);
			YY->y_id = id;
			send(socket_fd, YY, sizeof(XINXI),0);
			recv(socket_fd, buf, sizeof(buf), 0);
			printf("\t\t%s\n", buf);
			printf("\t\t请输入enter继续!!!\n");
			setbuf(stdin, NULL);
			getchar();
			setbuf(stdin, NULL);
		}
		else if(ic == 6)        //踢人
		{
			YY->ice_2 = 36;
			YY->m_id  = M_ID;
			printf("\t\t群ID：");
			scanf("%d", &id);
			YY->q_id = id;
			printf("\t\t要删除的群成员ID：");
			setbuf(stdin, NULL);
			scanf("%d", &id);
			YY->y_id = id;
			send(socket_fd, YY, sizeof(XINXI), 0);
			recv(socket_fd, buf, sizeof(buf), 0);
			printf("\t\t%s\n", buf);
			printf("\t\t请输入enter继续!!!\n");
			setbuf(stdin, NULL);
			getchar();
			setbuf(stdin, NULL);
		}
		else
		{
			printf("\t\t无此功能请重新输入！\n");
		}
	} while (1);

	free(XZ);
}

int C_TongZ(XINXI *YY, DENN *XX, int socket_fd)
{
	int sum, ret;
	char buf[50];
	read(socket_fd, &sum, sizeof(int));

	do{
		printf("\033c");
		setbuf(stdin, NULL);
		printf("\t\t你一共有%d条通知！！！\n", sum);
		printf("\t\t[A] 查看所有通知  [B] 逐个处理所有通知 [R] 退出\n");
		printf("\t\t请选择功能:");
		setbuf(stdin, NULL);
		scanf("%s", buf);
		if (strcmp(buf, "A") == 0)
		{
			write(socket_fd, buf, sizeof(buf));
			do
			{
				read(socket_fd, buf, sizeof(buf));
				if(strcmp(buf, "over") != 0)
				{
					printf("\t\t%s\n", buf);
				}
			} while (strcmp(buf, "over") != 0);
			break;
		}
		else if (strcmp(buf, "B") == 0)
		{
			write(socket_fd, buf, sizeof(buf));
			LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
			while(1)
			{
				printf("\033c");
				setbuf(stdin, NULL);
				recv(socket_fd, XZ, sizeof(LIAOT), 0);
				//printf("%s\n", XZ->buf);
				if(strcmp(XZ->buf, "over") == 0)
				{
					break;
				}
				else if(XZ->ice == 100)
				{
					break;
				}
				else
				{
					//printf("A%sA\n", XZ->buf);
					//printf("YYYYY%dYYYY\n", XZ->zt);
						char ch, B[50],beizhu[20];
						printf("\t\t%s\n", XZ->buf);
						printf("\t\t[Y] 同意  [N] 忽略\n");
						printf("\t\t请输入你的选择:");
						setbuf(stdin, NULL);
						scanf("%s", &B);
						setbuf(stdin, NULL);
						//write(socket_fd, ch, sizeof(char));
						if(strcmp(B, "Y") == 0)
						{
							if(XZ->zt == 0)
							{
								printf("\n\t\t请输入备注:");
								scanf("%s", beizhu);
								strncpy(XZ->buf, B, sizeof(B));
								strncpy(XZ->beizhu, beizhu, sizeof(beizhu));
								send(socket_fd, XZ, sizeof(LIAOT), 0);
								memset(XZ, 0, sizeof(LIAOT));
								recv(socket_fd, XZ, sizeof(LIAOT), 0);
								if(strcmp(XZ->buf, "over") == 0)
								{
									break;
								}
								printf("\t\t%s",XZ->buf);
								memset(XZ, 0, sizeof(LIAOT));
							}
							else if(XZ->zt == 1)
							{
								strncpy(XZ->buf, B, sizeof(B));
								send(socket_fd, XZ, sizeof(LIAOT), 0);
								recv(socket_fd, XZ, sizeof(LIAOT), 0);
								if(strcmp(XZ->buf, "over") == 0)
								{
									break;
								}
								printf("\t\t%s",XZ->buf);
								memset(XZ, 0, sizeof(LIAOT));
							}
						}
						else if(strcmp(B, "N") == 0)
						{
							strncpy(XZ->buf, B, sizeof(B));
							send(socket_fd, XZ, sizeof(LIAOT), 0);
						}
				}
			}
			break;
		}
		else if (strcmp(buf, "R") == 0)
		{
			write(socket_fd, buf, sizeof(buf));
			return 0;
		}
		else
		{
			printf("\t\t无此选项请重新输入！\n");
		}
		printf("\t\t请输入enter继续!!!\n");
		setbuf(stdin, NULL);
		getchar();
		setbuf(stdout, NULL);
		return 0;
	
	}while(1);
	printf("\t\t请输入enter继续!!!\n");
	setbuf(stdin, NULL);
	getchar();
	setbuf(stdout, NULL);
	return 0;
}

void C_haoy(XINXI *YY, DENN *XX, int socket_fd)
{
	int id,ret;
	char beizhu[20];
	char buf[50];
	do
	{
		//printf("%d\n", ret);
		printf("\033c");
		int ic;
		setbuf(stdin, NULL);
		LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
		memset(XZ, 0, sizeof(LIAOT));
		get_XX(socket_fd);
		printf("\t\t**********************************************\n");
		printf("\t\t******************0 退出***********************\n");
		printf("\t\t**************1 好友的添加，删除，查询************\n");
		printf("\t\t******************2 查看好友列表****************\n");
		printf("\t\t*****************3 查看好友状态*****************\n");
		printf("\t\t******************4 查看聊天记录****************\n");
		printf("\t\t****************5 屏蔽好友消息******************\n");
		printf("\t\t请输入要选择的功能：");
		scanf("%d", &ic);
		printf("\033c");
		if(ic == 1)      //添加,删除，查询好友
		{
			YY->ice_2 = 21;
			YY->m_id  = M_ID;
			//send(socket_fd, XZ, sizeof(LIAOT), 0);
			char buf[50];
			char ch;
			printf("\t\tA 添加好友\n");
			printf("\t\tB 删除好友\n");
			printf("\t\tC 查询好友\n");
			printf("\t\t请输入你的选择:");
			scanf("%s", buf);
			//int ret = write(socket_fd,buf, sizeof(buf));
			//strncpy(XZ->buf, buf, sizeof(XZ->buf));
			//printf("%d\n", ret);
			printf("\033c");
			if(strcmp(buf, "A") == 0)
        	{
				YY->ice_3 = 211;
				printf("\t\t请输入要添加好友的ID：");
				scanf("%d",&id);
				YY->y_id = id;
				printf("\t\t请输入备注：");
				scanf("%s",&beizhu);
				strncpy(YY->beizhu, beizhu, sizeof(beizhu));
				//printf("size = %d\nid = %d\n", sizeof(LIAOT), XZ->id);
				ret = send(socket_fd, YY, sizeof(XINXI), 0);  //将添加好友信息发送
				//printf("%d\n", ret);
				ret = read(socket_fd,buf, sizeof(buf));
				//printf("%d\n", ret);
				printf("\t\t%s",buf);
				
        	}
    	    else if(strcmp(buf, "B") == 0)
        	{
				YY->ice_3 = 212;
				do
				{
					int i;
					setbuf(stdin, NULL);
					printf("\t\t************************************\n");
					printf("\t\t**********0 退出 ********************\n");
					printf("\t\t**********1 根据ID 删除 **************\n");
					printf("\t\t**********2 根据备注删除 **************\n");
					printf("\t\t请输入你的选择：");
					scanf("%d", &i);
					printf("\033c");		
					YY->zt = i;
					if(i == 0)
					{
						//send(socket_fd, XZ, sizeof(LIAOT), 0);
						break;
					}
					else if(i == 1)
					{
						printf("\t\t请输入你要删除的好友的ID：");
						scanf("%d",&id);
						YY->y_id = id;
						ret = send(socket_fd, YY, sizeof(XINXI),0);
						ret = read(socket_fd, buf, sizeof(buf));
						printf("\t\t%s",buf);
					}
					else if(i == 2)
					{
						printf("\t\t请输入你要删除好友的备注：");
						scanf("%s",beizhu);
						strncpy(XZ->beizhu,beizhu,sizeof(beizhu));
						ret = send(socket_fd, YY, sizeof(XINXI),0);
						ret = read(socket_fd, buf, sizeof(buf));
						printf("\t\t%s",buf);
					}
					printf("\t\t请输入enter继续!!!\n");
					setbuf(stdin, NULL);
					getchar();
					setbuf(stdin, NULL);
				}while(1);
    	    }
    	    else if(strcmp(buf, "C") == 0)
    	    {
				YY->ice_3 = 213;
				do
				{
					int i;
					setbuf(stdin, NULL);
					printf("\t\t************************************\n");
					printf("\t\t**********0 退出 ********************\n");
					printf("\t\t**********1 根据ID查询备注 **************\n");
					printf("\t\t**********2 根据备注查询ID **************\n");
					printf("\t\t请输入你的选择：");
					scanf("%d", &i);
					printf("\033c");
					YY->zt = i;
					if(i == 0)
					{
						//ret = send(socket_fd, XZ, sizeof(LIAOT), 0);
						break;
					}		
					else if(i == 1)
					{
						printf("\t\t请输入好友的ID：");
						scanf("%d", &id);
						YY->y_id = id;
						ret = send(socket_fd, YY, sizeof(XINXI),0);
						ret = read(socket_fd, buf, sizeof(buf));
						printf("\t\t%s",buf);
					}
					else if(i == 2)
					{
						printf("\t\t请输入好友备注：");
						scanf("%s", beizhu);
						strncpy(YY->beizhu, beizhu, sizeof(beizhu));
						ret = send(socket_fd, YY, sizeof(XINXI), 0);
						ret = read(socket_fd, buf, sizeof(buf));
						printf("\t\t%s",buf);
					}
					printf("\t\t请输入enter继续!!!\n");
					setbuf(stdin, NULL);
					getchar();
					setbuf(stdin, NULL);
				} while (1);
				

    	    }
			/*
			int I;
			char B[20];
			printf("请输入好友ID:");
			scanf("%d", &I);
			printf("请输入备注:");
			scanf("%s", B);
			XZ->id = I;               //好友ID
			XZ->ice = ic;
			strncpy(XZ->beizhu, B, sizeof(B));          //备注名
			send(socket_fd, XZ, sizeof(LIAOT),0);

			read(socket_fd, buf, sizeof(buf));
			printf("%s\n",buf);
			*/
		}
		else if (ic == 2)      //查看好友列表
		{
			YY->ice_2 = 22;
			YY->m_id  = M_ID;
			send(socket_fd, YY, sizeof(XINXI),0);
			do
			{
				read(socket_fd,buf, sizeof(buf));
				if(strcmp(buf,"over") != 0)
				{
					printf("\t\t%s\n",buf);
				}
			} while (strcmp(buf, "over") != 0);

		}
		else if(ic == 3)        //查看好友状态
		{
			YY->ice_2 = 23;
			YY->m_id  = M_ID;
			printf("\t\t请输入要查询好友的ID：");
			scanf("%d", &id);
			YY->y_id = id;
			send(socket_fd, YY, sizeof(XINXI), 0);
			recv(socket_fd, XZ, sizeof(LIAOT), 0);
			//printf("XZ->zt = %d\n", XZ->zt);
			if(XZ->zt == 1)
			{
				printf("\t\t该好友在线！\n");
			}
			else if(XZ->zt == 0)
			{
				printf("\t\t该好友不在线!\n");
			}
			else if(XZ->zt == -1)
			{
				printf("\t\t你无此ID的好友！\n");
			}
		}
		else if(ic == 4)        //查看聊天记录
		{
			YY->ice_2 = 24;
			YY->m_id  = M_ID;
			printf("\t\t请输入你要查询好友的ID：");
			scanf("%d", &id);
			YY->y_id = id;
			send(socket_fd, YY, sizeof(XINXI), 0);
			do
			{
				setbuf(stdin, NULL);
				recv(socket_fd,XZ, sizeof(LIAOT), 0);
				if(strcmp(XZ->xinxi, "over") != 0)
				{
					if(XZ->id == id)      //从好友处发来的信息
					{
						printf("\t\033[34m%s : %s\033[0m\n\n", XZ->beizhu, XZ->xinxi);
					}
					else
					{
						printf("\t\033[31mMINE ：%s\033[0m\n\n", XZ->xinxi);
					}
				}
			} while (strcmp(XZ->xinxi, "over") != 0);
			//printf("end\n");
		}
		else if(ic == 5)        //屏蔽好友消息
		{
			YY->ice_2 = 25;
			YY->m_id  = M_ID;
			int i;
			while(1)
			{
				setbuf(stdin, NULL);
				printf("\033c");
				printf("\t\t**************1 屏蔽好友信息***************\n");
				printf("\t\t**************2 取消屏蔽 ******************\n");
				printf("\t\t**************0 退出 **********************\n");
				printf("\t\t请输入你的选择：");
				scanf("%d", &i);
				if(i == 0)
				{
					break;
				}
				else if(i == 1)
				{
					YY->ice_3 = 251;
					printf("\t\t请输入你要屏蔽的好友ID:");
					scanf("%d",&id);
					YY->y_id = id;
					send(socket_fd, YY, sizeof(XINXI),0);
					read(socket_fd, buf, sizeof(buf));
					printf("\t\t%s\n",buf);
				}
				else if(i == 2)
				{
					YY->ice_3 = 252;
					printf("\t\t请输入你要取消屏蔽的好友ID:");
					scanf("%d",&id);
					YY->y_id = id;
					send(socket_fd, YY, sizeof(XINXI),0);
					read(socket_fd, buf, sizeof(buf));
					printf("\t\t%s\n",buf);
				}
				printf("\t\t请输入任意键继续！！！\n");
				setbuf(stdin, NULL);
				getchar();
				setbuf(stdout, NULL);
			}
		}
		else if(ic == 0)
		{
			YY->ice_2 = 20; 
			return ;     //退出循环
		}
		else
		{
			printf("\t\t无此功能请重新选择!!!!!!!!!!!!\n");
		}
		printf("\t\t请输入enter继续!!!\n");
		setbuf(stdin, NULL);
		getchar();
		setbuf(stdin, NULL);
	} while (1);
} 

void C_denn(XINXI *YY, int socket_fd)
{
	int I;
	char *P,buf[50];
	do
	{
		setbuf(stdin, NULL);
		printf("\033c");
		printf("\t\tID:");
		scanf("%d",&I);
		P = getpass("\t\t密码：");
		YY->m_id = I;
		YY->zt = 1;
		strncpy(YY->password, P, 16);
		//printf("id = %d,password = %s\n",XX->id,XX->password);
		int ret = send(socket_fd, YY, sizeof(XINXI),0);
		ret = read(socket_fd,buf, sizeof(buf));
		if(ret)
		{
			printf("\t\t%s\n",buf);
		}
		printf("\t\t请输入enter继续!!!\n");
		setbuf(stdin, NULL);
		getchar();
		setbuf(stdin, NULL);
	}while(strcmp(buf,"登陆成功") != 0);
	//printf("AAAA\n");
}

void C_get(int socket_fd)
{
	int n;
	LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
	XINXI *YY = (XINXI*)malloc(sizeof(XINXI));
		//pthread_mutex_lock(&lock);              //加锁
		YY->ice_4 = 666;
		YY->ice_1 = 4;
		YY->m_id = M_ID;
		//printf("mid = %d\tyid = %d\n",M_ID,YY->y_id);
		YY->y_id = Y_ID;
		send(Socket_fd, YY, sizeof(XINXI), 0);
		//printf("JJJJJJJJJ\n");
		recv(Socket_fd, XZ, sizeof(LIAOT), 0);
		//printf("SSSSSSSSSSSS\n");
		n = XZ->zt;
		//printf("XZ->zt = %d\n", XZ->zt);
		if(XZ->zt > 0)
		{
			for(int i = 0;i<n;i++)
			{
				printf("\b\b\b\b\b\b\n");
				char buf[1024]={0};
				//read(socket_fd,buf,sizeof(buf));
				recv(Socket_fd, XZ, sizeof(LIAOT), 0);
				printf("\t\t%s : %s\n", XZ->beizhu, XZ->xinxi);
				//printf("MINE: ");
			}
		}
	free(YY);
	free(XZ);
	//printf("HHHHHHHHHHHHHHH\n");
	return ;
}

void *thread(void *arg)
{
	int n;
	char A[300];
	LIAOT XZ;
	XINXI YY;
	do
	{
		sleep(3);
		//pthread_mutex_lock(&lock);              //加锁
		YY.ice_4 = 666;
		YY.ice_1 = 4;
		YY.m_id = M_ID;
		//printf("mid = %d\tyid = %d\n",M_ID,YY->y_id);
		YY.y_id = Y_ID;
		send(Socket_fd, &YY, sizeof(XINXI), 0);
		//printf("JJJJJJJJJ\n");
		recv(Socket_fd, &XZ, sizeof(LIAOT), 0);
		//printf("SSSSSSSSSSSS\n");
		n = XZ.zt;
		//printf("XZ->zt = %d\n", XZ->zt);
		if(XZ.zt > 0)
		{
			for(int i = 0;i<n;i++)
			{
				printf("\b\b\b\b\b\b                 \n");
				char buf[1024]={0};
				//read(socket_fd,buf,sizeof(buf));
				recv(Socket_fd, &XZ, sizeof(LIAOT), 0);
				sprintf(A, "\t\033[34m%s : %s\033[0m", XZ.beizhu, XZ.xinxi);
				printf("%s", A);
			}
			printf("\t\033[31mMINE :");
		}
	} while (M == 1);       //退出线程
}

void C_haoyouliaot(XINXI *YY, DENN *XX, int socket_fd)
{
	pthread_t thid;
	int id,ret,ic,N = 0;
	char beizhu[20],buf[50];

		LIAOT *XZ = (LIAOT*)malloc(sizeof(LIAOT));
		get_XX(socket_fd);
		printf("\t\t**************************************\n");
		printf("\t\t*************0 退出 *******************\n");
		printf("\t\t*************1 与好友聊天 **************\n");
		printf("\t\t请输入你的选择：");
		scanf("%d", &ic);
		printf("\033c");
		if(ic == 0)
		{
			return;
		}
		else if(ic == 1)
		{
			YY->ice_2 = 31;
			YY->ice_4 = 0;
			YY->m_id  = M_ID;
			printf("\t\t请输入好友的ID:");
			scanf("%d",&id);
			YY->y_id = id;
			send(socket_fd,YY, sizeof(XINXI), 0);
			read(socket_fd,buf, sizeof(buf));
			if(strcmp(buf, "OK") != 0)
			{
				printf("\t\t%s",buf);
			}
			//fflush(stdin);
			Y_ID = id;
			
			//pthread_mutex_init(&lock, NULL);                   //初始化锁
    		//pthread_cond_init(&cond, NULL);                    //初始化条件变量
			M = 1;
			if(pthread_create(&thid, NULL, thread, NULL) != 0)
			{
				//创建失败
				printf("\t\t创建线程失败！\n");
				return ;
			}
			else
			{
				//printf("\t\t创建成功！\n");
			}

			printf("\t\033[31mMINE :");

			do{

				//pthread_mutex_lock(&lock);              //加锁
				
				
    			//pthread_cond_wait(&cond, &lock);        //将锁释放（其他地方可以抢到锁），并睡眠等待唤醒      
				/*
				printf("BBBBB\n");
				i++;
				printf("i = %d\n", i);
				if(i == 5)
				{
					i = 0;
					printf("YYYYYYYYYYYYYYYY\n");
					C_get(socket_fd);
				}
				*/
				//要监视的描述符集合
				fd_set fds;
				FD_ZERO(&fds);                  //清空文件描述符集合
		
				FD_SET(0,&fds);                 //把标准输入设备加入到集合中 
		
				//FD_SET(socket_fd,&fds);         //把网络通信文件描述符加入到集合中 


				ret = select(socket_fd+1,&fds,NULL,NULL,NULL);
		 		if(ret < 0)//错误
				{
					perror("select fail:");
					return ;
				}
				else if(ret > 0) //有活跃的
				{
					//判断是否 标准输入设备活跃 假设是则发送数据
					if(FD_ISSET(0,&fds))
					{
						N = 1;
						int i = 0;
						char Buf[200] = {0},ch;
						setbuf(stdin, NULL);
						fgets(Buf,200,stdin);
						setbuf(stdin, NULL);
						/*
						do
						{
							scanf("%c", &ch);
							if(ch != '\n')
							{
								Buf[i] = ch;
								i++;
							}
						}while(ch != '\n');
						*/
						YY->ice_4 = 777;
						YY->m_id  = M_ID;
						strncpy(YY->buf, Buf, sizeof(Buf));
						//printf("buf: %s\n", YY->buf);
						//printf("ice_1 = %d\n",YY->ice_1);
						if(strcmp(YY->buf, "exit\n") == 0)
						{
							printf("\033[0m\n");
							M = 0;
							pthread_join(thid, NULL);   //销毁线程
							return ;
						}
						if(strcmp(Buf, "\n") != 0)
							ret = send(socket_fd, YY, sizeof(XINXI), 0);
						//printf("buf = %sA\n", buf);


						printf("\t\033[31mMINE: ");
						N = 0;
					}
				}
				//pthread_mutex_unlock(&lock);
			}while(1);
		}

}

void C_zhuce(XINXI *YY, int socket_fd)
{
	int I;
	char P[16],buf[50],name[20],qu[200],an[100],hylb[20];
	
		setbuf(stdin, NULL);
		printf("\033c");
		printf("\t\tID:");
		scanf("%d",&I);
		printf("\t\tPassword:");
		scanf("%s",P);
		printf("\t\tName: ");
		scanf("%s",name);
		printf("\t\t请设置密保问题：");
		scanf("%s",qu);
		printf("\t\t请输入答案：");
		scanf("%s",an);
		YY->m_id = I;
		sprintf(hylb, "%shylb", name);
		strncpy(YY->password, P, sizeof(P));
		strncpy(YY->name, name, sizeof(name));
		strncpy(YY->qu, qu, sizeof(qu));
		strncpy(YY->an, an, sizeof(an));
		strncpy(YY->hylb, hylb, sizeof(hylb));          //好友列表，以name为mysql—table名
		send(socket_fd, YY, sizeof(XINXI),0);
		//printf("YY->m_id = %d\n",YY->m_id);
		read(socket_fd,buf, sizeof(buf));
		printf("\t\t%s\n",buf);
		printf("\t\t请输入enter继续!!!\n");
		setbuf(stdin, NULL);
		getchar();
		setbuf(stdin, NULL);
	
}

void C_zhaohui(XINXI *YY, int socket_fd)
{
	int I;
	char an[100],qu[200],buf[50];
	//do
	//{
		setbuf(stdin, NULL);
		printf("\033c");
		printf("\t\t请输入你的id：");
		scanf("%d",&I);
		YY->m_id = I;
		send(socket_fd, YY, sizeof(XINXI),0);      //将ID发送到服务器
		read(socket_fd, qu, sizeof(qu));          //读取密保问题
		printf("\t\t%s\n", qu);
		printf("\t\t答案:");
		setbuf(stdin, NULL);
		scanf("%s",an);     
		setbuf(stdin, NULL);                      //回答问题
		strncpy(YY->an, an, sizeof(an));
		YY->ice_3 = 131;
		send(socket_fd, YY, sizeof(XINXI),0);       //将答案发送到服务端
		read(socket_fd, buf, sizeof(buf));
		printf("\t\t%s\n", buf);
		printf("\t\t请输入enter继续!!!\n");
		setbuf(stdin, NULL);
		getchar();
		setbuf(stdin, NULL);
	//}while(strcmp(buf, "答案错误") == 0);
}

void face(XINXI *YY)
{
	int n;
	while(1)	
    {
		setbuf(stdin, NULL);
		printf("\033c");
		printf("\t\t/*******************************/\n");
		printf("\t\t/**************1 登陆**************/\n");
    	printf("\t\t/**************2 注册**************/\n");
	    printf("\t\t/************3 找回密码*************/\n");
		printf("\t\t/************0 退出程序*************/\n");
	    printf("\t\t请选择：");
		scanf("%d",&n);
		if(n == 0)
		{
			exit(0);
		}
		if(n > 0 && n <= 3)
		{
			break;
		}
		else
		{
			continue;
		}
	}
	YY->ice_2 = 10+n;         //选择功能
}