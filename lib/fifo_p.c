 #include "./fifo_p.h"

#define BUFF_SIZE 1024

//write_p
int write_p(const char *filename, int mode,char *buf, int len) {

         int fd;
         int real_write;

//check
        if(access(filename,F_OK) == 0) {
          if((mkfifo(filename,mode)<0) && (errno != EEXIST)) {
            printf("Can NOT create fifo file!\n");
            return -1;
         }
        }

//open
        if((fd=open(filename,mode)) == -1) {
        printf("Open fifo error!\n");
        return -1;
        }
//write
        if ((real_write = write(fd,buf,len)) > 0) {
         printf("Write into pipe: '%s'.\n",buf);
        }

     close(fd);
     return 0;
}

// read fifo_p
char *read_p(const char *filename, int mode) {

         int fd;
         int real_read;
         char buf[BUFF_SIZE];

//check
         if(access(filename,F_OK) == -1) {
          if((mkfifo(filename,mode) < 0)&&(errno != EEXIST)) {
            printf("Can NOT create fifo file!\n");
            return NULL;
          }
        }

//open
        if((fd=open(filename,mode|O_NONBLOCK)) == -1){
         printf("Open fifo error!\n");
         return NULL;
        }

//read
       do {
         memset(buf,0,sizeof(buf));
         if ((real_read = read(fd,buf,sizeof(buf)))>0) {
             printf("Read from pipe: '%s'.\n",buf);
         }
       } while (real_read > 0);

     close(fd);
     return buf;
}
