#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define _M_OFFSET(mem, type)              (offsetof(type, mem))
#define NUM(x)                                            (sizeof(x)/sizeof(x[0]))

#ifdef DBUG
 #define ERR_PRINT(str) \
    do \
    { \
        char errbuf[ERRBUFLEN] = {'\0'}; \
        snprintf(errbuf, ERRBUFLEN, "[file: %s line: %d] %s", \
                                    __FILE__, __LINE__, str); \
        fprintf(stderr, "\033[31m"); \
        perror(errbuf); \
        fprintf(stderr, "\033[0m"); \
    } while (0)
#define INFO_PRINT(str) \
    do \
    { \
        printf("\033[31m"); \
        printf("[file: %s line: %d] %s\n", __FILE__, __LINE__, str); \
        printf("\033[0m"); \
    } while(0)
#else
#define ERR_PRINT(str)
#define INFO_PRINT(str)

#endif //#ifdef DBUG

#endif//#ifndef _UTILS_H_

