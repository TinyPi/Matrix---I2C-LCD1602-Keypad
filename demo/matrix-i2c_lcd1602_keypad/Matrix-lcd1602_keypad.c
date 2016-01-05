#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <string.h> 
#include <sys/socket.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <pthread.h>
#include "libfahw.h"
#define TIME_STR_BUFSIZE     32


int server_sockfd = -1;//服务器端套接字
int client_sockfd = -1;//客户端套接字
int devFD = 0;
int revcmd = 0;
char revbuf[BUFSIZ] = {0};

pthread_t thread[2];
pthread_mutex_t mut;

#define mydebug  printf("nanopi-wz: ");printf

/*
void LCD1602KeyDispStr(int dev, int x, int y, char *str)
{
	mydebug("%s\n", str);
}

int LCD1602GetKey(int dev)
{
	int key = 0;
	
	mydebug("please input key value(0f, 17, 1b, 1d, 1e)\n");
	scanf("%x", &key);
	mydebug("press key is: %x\n", key);
	
	return key;
}

int LCD1602KeyInit(void)
{
	return 10;
}

void LCD1602KeyClear(int dev)
{
	mydebug("clear screen\n");
}
*/

unsigned long GetLocalAddr(int dev, char *eth)
{
    struct ifreq ifr;
	int socketFD;
	struct sockaddr_in *server;
	
    socketFD = socket(AF_INET, SOCK_DGRAM, 0); 
	 
    if(socketFD == -1) 
	{
        mydebug("socket error\n");
        return -1;
    }
	
    strcpy(ifr.ifr_name, eth);
    if(ioctl(socketFD, SIOCGIFADDR, &ifr) < 0) 
    {
		return -1;
    } 
    else 
    {
        server = (struct sockaddr_in*)&(ifr.ifr_addr);
		mydebug("local ip address is: %s\n", inet_ntoa(server->sin_addr));
		LCD1602KeyDispStr(dev, 0, 1, inet_ntoa(server->sin_addr));
		return server->sin_addr.s_addr;
    }
}

int CreatServer(unsigned long serverip, int *pserver_sockfd, int *pclient_sockfd)
{
	int server_sockfd;//服务器端套接字
	int client_sockfd;//客户端套接字
	struct sockaddr_in my_addr;   //服务器网络地址结构体
	struct sockaddr_in remote_addr; //客户端网络地址结构体
	int sin_size;
	
	memset(&my_addr,0,sizeof(my_addr)); //数据初始化--清零
	my_addr.sin_family=AF_INET; //设置为IP通信
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//my_addr.sin_addr.s_addr = serverip;//服务器IP地址--允许连接到所有本地地址上
	my_addr.sin_port=htons(8000); //服务器端口号
	
	
	/*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/
	if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
	{  
		perror("socket\n");
		return -1;
	}
 
        /*将套接字绑定到服务器的网络地址上*/
	if (bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)
	{
		perror("bind\n");
		return -1;
	}
	
	/*监听连接请求--监听队列长度为5*/
	listen(server_sockfd, 5);
	sin_size = sizeof(struct sockaddr_in);

	/*等待客户端连接请求到达*/
	mydebug("wait client...\n");
	if((client_sockfd = accept(server_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0)
	{
		perror("accept\n");
		return -1;
	}
	
	LCD1602KeyClear(devFD);
	usleep(1000);
	LCD1602KeyDispStr(devFD, 0, 0, "Client success ");
	LCD1602KeyDispStr(devFD, 0, 1, "  Connectivity ");
	
	mydebug("accept client %s\n",inet_ntoa(remote_addr.sin_addr));
	//len = send(client_sockfd,"Welcome to nanopi\n",21,0);//发送欢迎信息
	
	pthread_mutex_lock(&mut);
	
	*pserver_sockfd = server_sockfd;
	*pclient_sockfd = client_sockfd;
	
	pthread_mutex_unlock(&mut);
	
	return 0;
}
/*
        F2(up)        
F1(left)        F4(right)   F5(ok)
        F3(down)

*/

void *KeyThread1()
{
	mydebug ("KeyThread\n");

    int keyValue = 0;
    int lastKeyValue = -1;
    int showDefault = 1;
    int needClear = 1;
	int conMsg = 0;
	int _revcmd = 0;
	
	unsigned long serverip = 0;

    if ((devFD = LCD1602KeyInit()) == -1) 
	{
        mydebug("Fail to init LCD1602\n");
		pthread_exit(NULL);
    }
	
    LCD1602KeyClear(devFD);
    usleep(1000);
	
    mydebug("waiting key press...\n");
	
    while (1) 
	{
		pthread_mutex_lock(&mut);
		_revcmd = revcmd;
		pthread_mutex_unlock(&mut);
		
		switch (_revcmd)
		{
			case 0x1:
				_revcmd = 0;
				LCD1602KeyClear(devFD);
				usleep(1000);
				LCD1602KeyDispStr(devFD, 0, 1, revbuf+24);
				revbuf[24] = '\0';
                LCD1602KeyDispStr(devFD, 0, 0, revbuf+8);
			break;
			
			case 0x2:
				_revcmd = 0;
				keyValue = 0x1d;
			break;
			
			case 0x3:
				_revcmd = 0;
				keyValue = 0x1b;
			break;
			
			case 0x4:
				_revcmd = 0;
				keyValue = 0x1e;
			break;
			
			case 0x5:
				_revcmd = 0;
				keyValue = 0x17;
			break;
			
			case 0x6:
				_revcmd = 0;
				keyValue = 0x0f;
			break;
			
			default:
				keyValue = LCD1602GetKey(devFD);
			break;
		}
		
        if (keyValue != lastKeyValue) 
		{
			mydebug("current pressed key is %x", keyValue);
            lastKeyValue = keyValue;
        } 
		else if (showDefault != 1)
		{
			pthread_mutex_lock(&mut);
			revcmd = 0;
			pthread_mutex_unlock(&mut);
            continue;
        }
		
        switch (keyValue) 
		{
            // F1 left
        case 0x1e:  // display ip address
			showDefault = 0;
			LCD1602KeyClear(devFD);
			usleep(1000);
			LCD1602KeyDispStr(devFD, 0, 0, "IP");
			LCD1602KeyDispStr(devFD, 0, 1, "Restart");
            conMsg = 0x0;
            break;
			
            // F2  up  
        case 0x1d: //
		    showDefault = 0;
			LCD1602KeyClear(devFD);
			usleep(1000);
			LCD1602KeyDispStr(devFD, 0, 0, "IP ^");
			LCD1602KeyDispStr(devFD, 0, 1, "Restart");
            conMsg = 0x1;
            break;
			
            // F3   down 
        case 0x1b: // start board display program
            showDefault = 0;
			LCD1602KeyClear(devFD);
			usleep(1000);
			LCD1602KeyDispStr(devFD, 0, 0, "IP");
			LCD1602KeyDispStr(devFD, 0, 1, "Restart ^");
            conMsg = 0x2;
            break;
			
            // F4  right
        case 0x17: //
		    showDefault = 0;
			LCD1602KeyClear(devFD);
			usleep(1000);
			LCD1602KeyDispStr(devFD, 0, 0, "IP");
			LCD1602KeyDispStr(devFD, 0, 1, "Restart");
            conMsg = 0x1;
            break;
			
            // F5  ok
        case 0x0f: 
		    showDefault = 0;
			switch (conMsg)
			{
				case 0x1: // display IP
					LCD1602KeyClear(devFD);
					usleep(1000);
					LCD1602KeyDispStr(devFD, 0, 0, "PORT: 8088    ");
					if ((serverip = GetLocalAddr(devFD, "wlan0")) > 0)
					{
						CreatServer(serverip, &server_sockfd, &client_sockfd); // start server
					}
					else
					{
						mydebug("get ip address failure\n");
					}
					break;
				
				case 0x2: // reboot
					usleep(2000);
					sync();
					reboot(RB_AUTOBOOT);
					break;
			}
            break;
        }
        if (showDefault == 1) 
		{
            if (needClear) 
			{
                LCD1602KeyClear(devFD);
                usleep(1000);
                LCD1602KeyDispStr(devFD, 0, 0, "IP");
                LCD1602KeyDispStr(devFD, 0, 1, "Restart    ");
                needClear = 0;
            }
        } 
		else 
		{
            needClear = 1;
            usleep(1000);
        }
    }    
    mydebug("quit reading key press\n");
    LCD1602KeyDeInit(devFD);

	pthread_exit(NULL);
}

void *SocketThread2()
{
	char buf[BUFSIZ] = { 0 };  //数据传送的缓冲区
	int len = 0;
	int _revcmd;
	
	mydebug("SocketThread2\n");

	for (; ; )
	{
		usleep(100);
        
		// 客户端已经连接，准备接收数据
		if (server_sockfd > 0 && client_sockfd > 0)
		{
			/*接收客户端的数据并将其发送给客户端--recv返回接收到的字节数，send返回发送的字节数*/
			while ((len = recv(client_sockfd, buf, BUFSIZ, 0)) >0 )
			{
				buf[len]='\0';
				mydebug("sever receive: %s\n",buf);
				
				if (strstr(buf, "DISPLAY"))
				{
					_revcmd = 0x01;
				}
				else if (strstr(buf, "UP"))
				{
					_revcmd = 0x02;
				}
				else if (strstr(buf, "DOWN"))
				{
					_revcmd = 0x03;
				}
				else if (strstr(buf, "LEFT"))
				{
					_revcmd = 0x04;
				}
				else if (strstr(buf, "RIGHT"))
				{
					_revcmd = 0x05;
				}
				else if (strstr(buf, "CONFIRM"))
				{
					_revcmd = 0x06;
				}
				else
				{
					_revcmd = 0x0;
				}
				mydebug("sever revcmd: %d\n",_revcmd);
				
				pthread_mutex_lock(&mut);
				revcmd = _revcmd;
				memcpy(revbuf, buf, BUFSIZ);
				pthread_mutex_unlock(&mut);
				
				usleep(5000);
				if(send(client_sockfd,buf,len,0)<0)
				{
					perror("write");
					break;
				}
			}
			
			pthread_mutex_lock(&mut);
			
			close(client_sockfd);
			close(server_sockfd);
			
			pthread_mutex_unlock(&mut);
			
			pthread_exit(NULL);
		}
	}
}

void thread_create(void)
{
	int temp;
	memset(&thread, 0, sizeof(thread));          //comment1
	
	/*创建线程*/
	if((temp = pthread_create(&thread[0], NULL, KeyThread1, NULL)) != 0)  //comment2 
	{
		mydebug("线程1创建失败!\n");
	}    		
	else
	{
		mydebug("线程1被创建\n");
	}
			
	if((temp = pthread_create(&thread[1], NULL, SocketThread2, NULL)) != 0)  //comment3
	{
		mydebug("线程2创建失败");
	}
	else
	{
		mydebug("线程2被创建\n");
	}			
}

/*用默认属性初始化互斥锁*/
void thread_wait(void)
{
	/*等待线程结束*/
	if(thread[0] !=0)
	{   
		//comment4    
		pthread_join(thread[0], NULL);
		mydebug("线程1已经结束\n");
	}
	if(thread[1] !=0) 
	{  
		//comment5
	    pthread_join(thread[1], NULL);
		mydebug("线程2已经结束\n");
	}
}

int main(int argc, char* argv[])
{   
	mydebug("server start \n");

	/*用默认属性初始化互斥锁*/	
	pthread_mutex_init(&mut, NULL);

	thread_create();
	thread_wait();

	return 0;
}










