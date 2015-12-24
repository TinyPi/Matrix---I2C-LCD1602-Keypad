#ifndef _FIFO_P_
#define _FIFO_P_

int write_p(const char *filename, int mode,char *buf, int len);

char *read_p(const char *filename, int mode);

#endif