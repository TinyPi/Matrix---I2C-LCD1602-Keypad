#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

typedef unsigned char uchar;
typedef unsigned int uint;

typedef enum _bool
{
    FALSE = 0,
    TRUE = 1
}BOOL;

enum bug_level
{
    LINFO =  0,
    LDEBUG,
    LWARN,
    LERR,
};

#define __DEBUG

#define _M_OFFSET(mem, type)              (offsetof(type, mem))
#define NUM(x)                                            (sizeof(x)/sizeof(x[0]))

#ifdef __DEBUG
#define BUGLEVEL    LINFO
#define PDEBUG(buglevel, fmt, ...)   \
do  \
{   \
    if(buglevel >= BUGLEVEL) \
        printf("["__FILE__ "]%s  Line:%d:" fmt "\n",__func__,  __LINE__, ##__VA_ARGS__); \
}while(0)
#else
#define PDEBUG(buglevel, fmt, ...)
#endif //#ifdef DBUG

#endif//#ifndef _UTILS_H_

