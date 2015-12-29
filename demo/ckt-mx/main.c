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
    int ret;
    //void *recycle; 

    printf("welcome ! this is server routine\r\n");
    server_init();

    //pthread_mutex_init(&lock, NULL);
    //pthread_cond_init(&notempty, NULL);
    //pthread_cond_init(&notfull, NULL);
    //readpos = 0;
    //writepos = 0;

    ret = pthread_create(&id_read,NULL,(void *)read_pthread,NULL);
    if(ret != 0){
    printf ("Create pthread error!\n");
    exit (1);
    }

    //ret = pthread_create(&id_write, NULL, write_pthread, NULL);
    //if(ret != 0){
    //printf ("Create pthread error!\n");
    //exit (1);
    //}

    pthread_join(id_read,NULL); //recycle resource
    //pthread_join(id_write,&recycle);
    return 0;
}
