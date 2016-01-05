#include <pthread.h>
#include "fifo_thread.h"
#include "fifo_p.h"
#include "inf_server.h"

#define BUFF_SIZE 512


int __read_pthread(const char *filename)
{
	int fd, rc;
	char fifo_read[BUFF_SIZE];

	if(access(filename,F_OK) != 0) {
		if((mkfifo(filename,0666)<0) && (errno != EEXIST)) {
			printf("Can NOT create fifo file %s,errno %d\n", filename, errno);
			return -1;
		}
	}
	if((fd=open(filename,O_RDWR)) == -1) {
		printf("Open fifo %s error!\n", filename);
		return -1;
	}

	do {

		//read buff
		int i=0;
		memset(fifo_read, 0, sizeof(fifo_read));
		printf("[ckt-li] read fifo:%s, rc = %d 111111!\n", filename, rc);
		rc = read_p(fd, fifo_read, sizeof(fifo_read));//block until read data
		if(rc < 0) {
			printf("read fifo failed, rc %d\n", rc);	
		}
		printf("[ckt-li]read from FIFO Data is:");
		for(; i < rc; ++i) {
		printf("[%x]", fifo_read[i]);
		}
    printf("\n");
		// call send API
		pack_and_send(fifo_read, sizeof(fifo_read));
		printf("[ckt-li]pack and send done!\n");
		//readpos++;
		//pthread_cond_signal(&notempty);

		//usleep(500);
		//pthread_mutex_unlock(&lock);

	}while (1);

	close (fd) ;
	pthread_exit(0);
	return 0;

}
void read_pthread()
{
	int ret;
	ret = __read_pthread(READ_FIFO);

	if (ret)
		printf("__read_pthread err!\n");

}


int write_to_fifo(const char *filename, char *buf, unsigned int len)
{
	int fd,rc, i = 0;

	if(access(filename,F_OK) != 0) {
		if((mkfifo(filename,0666)<0) && (errno != EEXIST)) {
			printf("Can NOT create fifo file %s,errno %d\n", filename, errno);
			return -1;
		}
	}
	if((fd=open(filename,O_RDWR)) == -1) {
		printf("Open fifo %s error!\n", filename);
		return -1;
	}
	//while (1) {
	//pthread_mutex_lock(&lock);

	//if (writepos == 0)
	//pthread_cond_wait(&notfull, &lock);

	// write data
	rc = write_p(fd, buf,len);
	if(rc < 0)
		printf("write fifo failed,rc %d\n", rc);
	//writepos--;
	//pthread_mutex_unlock(&lock);
	//}
	//close (fd) ;
	//pthread_exit(0);
    printf("[ckt-li]Write to FIFO Data is:");
    for(; i < rc; ++i)
    {
        printf("[%x]", buf[i]);
    }
    printf("\n");

	return rc;

}
