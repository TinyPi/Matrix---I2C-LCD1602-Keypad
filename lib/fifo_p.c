 #include "./includes/fifo_p.h"

#define BUFF_SIZE 256

//write_p
int write_p(int fd, char *buf, int len) {

    int real_write;

	if (BUFF_SIZE < len)
	{
		printf("Write buff is bigger than BUFF_MAX \n");
	}
//write
    if ((real_write = write(fd,buf,len)) > 0) {
        printf("Write into pipe: '%s'.\n",buf);
    }
	else
	{
		printf("FIFO writer error!!!\n");
		return -1;
	}

    return 0;
}

// read fifo_p
int read_p(int fd, char *buf, int len)
{

    int real_read;

//read
	memset(buf, 0, sizeof(len));
	real_read = read(fd, buf, sizeof(len));
	if (0 < real_read)
	{
		return 0;
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
