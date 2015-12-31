#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#define _M_OFFSET(mem, type)              (offsetof(type, mem))
#define NUM(x)                                            (sizeof(x)/sizeof(x[0]))

#endif

