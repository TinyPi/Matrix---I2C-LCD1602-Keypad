#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include "fifo_p.h"

extern int server_init();

int main(int argc, char **argv)
{

    //pthread_t id_read,id_write;
    pthread_t id_read;
    pthread_t server_thread;
	int ret;
    //void *recycle; 

    printf("welcome ! this is server routine\r\n");
    server_thread = server_init();

    ret = pthread_create(&id_read,NULL,(void *)read_pthread,NULL);
    if(ret != 0){
    printf ("Create pthread error!\n");
    exit (1);
    }

    pthread_join(id_read,NULL); //recycle resource
    pthread_join(server_thread, NULL); //recycle resource
    
    return 0;
}
