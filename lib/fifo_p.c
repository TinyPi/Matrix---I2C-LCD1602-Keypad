#include "./includes/fifo_p.h"
#include <sys/types.h>
#include <dirent.h>
#include "common.h"

//#define BUFF_SIZE 256
#define BUFF_SIZE 512

//write_p
int write_p(int fd, char *buf, int len) {

  int real_write, i=0;

	if (BUFF_SIZE < len) {
	printf("Write buff is bigger than BUFF_MAX \n");
	}
//write
	if ((real_write = write(fd,buf,len)) > 0) {
	printf("[ckt-li]write to FIFO Data is:");
	for(; i < real_write; ++i){
	printf("[%x]", buf[i]);
	}
	printf("\n");
    //printf("Write into pipe: '%s'.\n",buf);
	}
	else {
	printf("FIFO writer error!!!\n");
	return -1;
	}

    return real_write;
}

// read fifo_p
int read_p(int fd, char *buf, int len)
{

    int real_read, i=0;

//read
	memset(buf, 0, sizeof(len));
	real_read = read(fd, buf, len);
	printf("[ckt-li] real read = %d!!!\n", real_read);
	printf("[ckt-li]read_p from FIFO Data is:");
	for(; i < real_read; ++i){
	printf("[%x]", buf[i]);
	}
	printf("\n");
	if (0 < real_read)
	{
		return real_read;
	}
	else if (0 == real_read)
	{
		printf("there is not any buf here!!!\n");
		return 1;
	}
	else
	{
		printf("there is an error happening here!!!\n");
		return -1;
	}
}
