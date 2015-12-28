 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>
 #include "fifo_p.h"

#define BUFF_SIZE 512

//write_p
int write_p(int fd, char *buf, int len) {

         
         int real_write;


//write
        if ((real_write = write(fd,buf,len)) > 0) {
         printf("Write into pipe: '%s'.\n",buf);
        }

     return 0;
}

// read fifo_p
char *read_p( int fd) {

         
         int real_read;
         char buf[BUFF_SIZE];


//read
       do {
         memset(buf,0,sizeof(buf));
         if ((real_read = read(fd,buf,sizeof(buf)))>0) {
             printf("Read from pipe: '%s'.\n",buf);
         }
       } while (real_read > 0);

     return buf;
}
