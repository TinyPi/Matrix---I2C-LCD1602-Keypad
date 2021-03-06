#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "inf_server.h"
#include <pthread.h>
#ifndef DDD
#include "fifo_p.h"
#include "fifo_thread.h"
#endif


#define MYPORT 8088    // the port users will be connecting to
#define BACKLOG 128     // how many pending connections queue will hold
#define BUF_SIZE 512

int fd_A[BACKLOG];    // accepted connection fd
int conn_amount;    // current connection amount


void print_hex(char *buf, int sz)
{
	int idx;

	for (idx=0; idx<sz; idx ++)
		printf("%x ", buf[idx]);
	printf("\r\n");
}


int pack_and_send(char *buf, unsigned int size)
{
	char buf_tmp[512];
	unsigned char client_id = buf[1];
	unsigned int  buf_size = (unsigned int)buf[0] -1;
     
	buf_tmp[0] = 0x86;
	buf_tmp[1] = buf_size;

	memcpy((void *)&buf_tmp[2], (void *)&buf[2], buf_size);
	buf_tmp[buf_size + 2] = 0x68;
	send(fd_A[client_id], buf_tmp, buf_size+3, 0);
	printf("send buf size=%d\r\n", buf_size+3);
	return 0;
}


void parse(char client_id, char *buf, unsigned int size)
{

	char dst_buf[512];
	unsigned int len = 0;
	if ( (buf[0]&0xff) != (unsigned char)0x86)
	{
		printf("pkt has wrong\r\n");	
	}
	dst_buf[0] = buf[1]+1; // length;
	dst_buf[1] = client_id;
	len = (0xff & buf[1]);
	memcpy((void *)&dst_buf[2], &buf[2], len);
#ifndef DDD
	//printf("before write to fifo: %s\n", dst_buf);
	write_to_fifo(WRITE_FIFO, dst_buf, len+2);
#endif
	printf("dst_buf, rcv=%d\r\n", size);
	print_hex(buf, size);
	//send(fd_A[client_id], buf, size, 0);
}

static unsigned char gbuf[512];
static unsigned int  gcount = 0;
int CALL_BACK(int client_id, char *buf, unsigned int size)
{	
	static unsigned int locker = 0;	
	static unsigned char pkt_size;	

#if 1
	printf("%x====%d\r\n", buf[0], size);

	if ((buf[0]&0xff) == 0x86 && locker == 0)
	{
		locker = 1;
		gcount = 0;
	}	

	if (locker >= 1)
	{
		gbuf[gcount ++] = buf[0];

		if (locker ++ == 2)
		{
			pkt_size = (buf[0]&0xff);
			pkt_size += 3;
			printf("get=%d!!!!!!!!!!\r\n", pkt_size);
		}

		//printf("===========pkt_siez======%d\r\n", pkt_size, gcount);
		if ( gcount == (unsigned int)pkt_size )
		{
			if ( (buf[0]&0xff) == 0x68)
				parse(client_id, gbuf, gcount);
			else
				printf("rcv data error, not found 0x68 \r\n"); 
			locker = 0;
			gcount = 0;
		}
	}
#endif
	return 0;	
}

void *thread_listen_socket(void *arg)
{
	int sock_fd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct sockaddr_in server_addr;    // server address information
	struct sockaddr_in client_addr; // connector's address information
	socklen_t sin_size;
	int yes = 1;
	//pthread_t thread_good;
	char buf[BUF_SIZE];
	int ret;
	int i;

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	server_addr.sin_family = AF_INET;         // host byte order
	server_addr.sin_port = htons(MYPORT);     // short, network byte order
	server_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
		perror("bind");
		exit(1);
	}

	if (listen(sock_fd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	printf("listen port %d\n", MYPORT);


	fd_set fdsr;
	int maxsock;
	struct timeval tv;

	conn_amount = 0;
	sin_size = sizeof(client_addr);
	maxsock = sock_fd;
	while (1) 
	{
		// initialize file descriptor set
		FD_ZERO(&fdsr);
		FD_SET(sock_fd, &fdsr);
		// timeout setting
		tv.tv_sec = 30;
		tv.tv_usec = 0;

		// add active connection to fd set
		for (i = 0; i<BACKLOG; i++) 
		{
			if (fd_A[i] != 0) 
			{
				FD_SET(fd_A[i], &fdsr);
			}
		}

		ret = select(maxsock + 1, &fdsr, NULL, NULL, &tv);
		if (ret < 0) {
			perror("select");
			break;
		} else if (ret == 0) {
			printf("timeout\n");
			continue;
		}

		// check every fd in the set
		for (i = 0; i<conn_amount; i++)
		{
			if (FD_ISSET(fd_A[i], &fdsr)) 
			{
				ret = recv(fd_A[i], buf, 1, 0);
				if (ret <= 0) 
				{        // client close
					printf("client[%d] close\n", i);
					close(fd_A[i]);
					FD_CLR(fd_A[i], &fdsr);
					fd_A[i] = 0;
					conn_amount--;
				} 
				else 
				{   
					// receive data
					//printf("client[%d] send:%s\n", i, buf);
					CALL_BACK(i,buf,ret);
				}
			}
		}

		// check whether a new connection comes
		if (FD_ISSET(sock_fd, &fdsr)) 
		{
			new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
			if (new_fd <= 0) 
			{
				perror("accept");
				continue;
			}
			// add to fd queue
			if (conn_amount < BACKLOG) 
			{
				fd_A[conn_amount++] = new_fd;
				printf("new connection client[%d] %s:%d\n", conn_amount,
						inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
				if (new_fd > maxsock)
					maxsock = new_fd;
			}
			else 
			{
				printf("max connections arrive, exit\n");
				//send(new_fd, "bye", 4, 0);
				close(new_fd);
				break;
			}
		}
		//showclient();
	}
	return NULL;
}


pthread_t server_init(void)
{
	pthread_t thread_good;
	pthread_create(&thread_good, NULL, thread_listen_socket, NULL);
	return thread_good;
}


#ifdef DDD
// use test routine: input cc server.c -DDDD -lpthread
void main(int argc, char **argv)
{
	pthread_t id = server_init();
	pthread_join(id, NULL);
}
#endif


