#include "cli.h"


int main()
{
	int id;
	DENN *XX = (DENN*)malloc(sizeof(DENN));
	XINXI YY;
	char buf[50];
	//face(XX);

	int port = atoi("9999");      //从命令行获取端口号
	if( port<1025 || port>65535 )       //0~1024一般给系统使用，一共可以分配到65535
	{
		printf("端口号范围应为1025~65535");
		return -1;
	}
	
	//1 创建tcp通信socket
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd == -1)
	{
		perror("socket failed!\n");
	}
 
	//2 连接服务器
	struct sockaddr_in server_addr = {0};//服务器的地址信息
	server_addr.sin_family = AF_INET;//IPv4协议
	server_addr.sin_port = htons(port);//服务器端口号
	server_addr.sin_addr.s_addr = inet_addr("192.168.30.238");         //设置服务器IP
	int ret = connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));       //连接服务器
	if(ret == -1)
	{
		perror("connect failed!\n");
	}
	memset(&YY, 0, sizeof(XINXI));
	Socket_fd = socket_fd;

	do{          //*******************************************************************************

		YY.ice_1 = 1;
		do
		{
			memset(&YY, 0, sizeof(XINXI));
			YY.ice_1 = 1;
			printf("\033c");
			face(&YY);     //1        2       3          0
			if(YY.ice_2 == 11)             //登录
			{
				printf("\033c");
				C_denn(&YY, socket_fd);
				M_ID = YY.m_id;
				break;
			}
			else if(YY.ice_2 == 12)        //注册
			{
				printf("\033c");
				C_zhuce(&YY, socket_fd);
				//face(XX);
			} 
			else if(YY.ice_2 == 13)       //找回密码
			{
				printf("\033c");
				C_zhaohui(&YY, socket_fd);

				//face(XX);
			}
			else if(YY.ice_2 == 0)
			{
				exit(0);
			}
			else
			{
				printf("无此功能，请重新选择！\n");
			}
		}while(1);

		do{  
			int ice;
			setbuf(stdin, NULL);
			printf("\033c");
			get_XX(socket_fd);
			memset(&YY, 0, sizeof(XINXI));
			printf("\t\t*************************************\n");
			printf("\t\t***********1 好友管理******************\n");
			printf("\t\t***********2 聊天群管理****************\n");
			printf("\t\t***********3 好友聊天*****************\n");
			printf("\t\t***********4 群聊天*******************\n");
			printf("\t\t***********5 查看通知******************\n");
			printf("\t\t***********6 文件传输******************\n");
			printf("\t\t***********0 退出帐号 *****************\n");
			printf("\t\t请输入你的选择：");
			scanf("%d", &ice);
			printf("\033c");
			if(ice == 1)        //好友管理
			{
				YY.ice_1 = 2;
				//printf("%d\n", YY.ice);
				//int ret = send(socket_fd, &YY, sizeof(YY), 0);
				C_haoy(&YY, XX, socket_fd);
			}
			else if(ice == 2)  //聊天群管理
			{
				YY.ice_1 = 3;
				C_group(&YY, XX, socket_fd);
			}
			else if(ice == 3)  //好友聊天
			{
				YY.ice_1 = 4;
				//int ret  = send(socket_fd, &YY, sizeof(YY),0);
				C_haoyouliaot(&YY, XX, socket_fd);
			}
			else if(ice == 4)  //群聊
			{
				YY.ice_1 = 5;
				C_group_com(&YY, XX, socket_fd);
			}
			else if(ice == 5)  //查看通知 
			{
				YY.ice_1 = 6;
				YY.m_id  = M_ID;
				//printf("%d\n", YY.ice);
				int ret = send(socket_fd, &YY, sizeof(YY), 0);
				C_TongZ(&YY, XX, socket_fd);
			}
			else if(ice == 6)  //文件传输
			{
				YY.ice_1 = 7;
				YY.m_id = M_ID;
				C_document(&YY, socket_fd);
				//printf("BBBBBBBB\n");
			}	
			else if(ice == 0)  //退出帐号
			{
				YY.ice_1 = 404;
				YY.m_id = M_ID;
				send(socket_fd, &YY, sizeof(XINXI), 0);
				break;
			}
			//printf("CCCCCC\n");
		}while(1);

	}while(1);

	//4 关闭通信socket
	close(socket_fd);
 
	do{

	}while(1);
	
	return 0;
}



