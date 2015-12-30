#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef _FIFO_THREAD_
#define _FIFO_THREAD_


//#define BUFF_SIZE 512
#define READ_FIFO  "/tmp/fifo_2"
#define WRITE_FIFO "/tmp/fifo_1"

//char fifo_read[BUFF_SIZE];

int __read_pthread(const char *);
void read_pthread();
int write_to_fifo(const char *, char *, unsigned int );


#endif
