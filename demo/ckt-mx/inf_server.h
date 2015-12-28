#ifndef __SERVER_API__
#define __SERVER_API__


typedef struct
{
        int           CLIENT_ID;
        int           SOCKET_FD;
        unsigned char START_TOKEN;
        unsigned char CMD_LENGTH;
        unsigned char pBUF[256];
        unsigned char END_TOKEN;
}PASER_t;

extern int pack_and_send(char *buf, unsigned int size);


#endif
