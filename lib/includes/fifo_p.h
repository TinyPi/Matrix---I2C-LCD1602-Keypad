 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <errno.h>
 #include <unistd.h>
 #include <fcntl.h>

#ifndef _FIFO_P_
#define _FIFO_P_


int write_p(int, char *, int);

int read_p(int, char *, int);

#endif
