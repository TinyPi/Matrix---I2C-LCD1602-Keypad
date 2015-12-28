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


#define MYPORT 8088    // the port users will be connecting to

#define BACKLOG 128     // how many pending connections queue will hold

#define BUF_SIZE 512

int fd_A[BACKLOG];    // accepted connection fd
int conn_amount;    // current connection amount


int pack_and_send(char *buf, unsigned int size)
{
#if 0
	pkt->CLIENT_ID = client_id;
	pkt->START_TOKEN = 0x86;
	pkt->CMD_LENGTH = size;
	memcpy((void *)pkt->pBUF, (void *)buf, pkt->CMD_LENGTH);
	pkt->END_TOKEN = 0x68;
#endif
	return 0;
}


void parse(int client_id, char *buf, unsigned int size, PASER_t *pkt)
{
	pkt->CLIENT_ID = client_id;
	pkt->START_TOKEN = buf[0];
	pkt->CMD_LENGTH = buf[1];
	memcpy((void *)pkt->pBUF, (void *)&buf[2], pkt->CMD_LENGTH);
	pkt->END_TOKEN = buf[size-1];
	pkt->SOCKET_FD = fd_A[client_id];
}


int CALL_BACK(int client_id, char *buf, unsigned int size)
{	
	PASER_t pkt;
	parse(client_id, buf, size, &pkt);
	//call send_fifo 
		
	return 0;	
}

int server_init(void)
{
	int sock_fd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct sockaddr_in server_addr;    // server address information
	struct sockaddr_in client_addr; // connector's address information
	socklen_t sin_size;
	int yes = 1;
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
				ret = recv(fd_A[i], buf, sizeof(buf), 0);
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
				send(new_fd, "bye", 4, 0);
				close(new_fd);
				break;
			}
		}
		//showclient();
	}

	// close other connections
	for (i = 0; i < BACKLOG; i++) 
	{
		if (fd_A[i] != 0) {
			close(fd_A[i]);
		}
	}

	exit(0);
}





