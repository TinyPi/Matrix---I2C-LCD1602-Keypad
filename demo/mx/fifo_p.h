#ifndef _FIFO_P_
#define _FIFO_P_

int write_p(int fd, char *buf, int len);

char *read_p(int fd);

#endif