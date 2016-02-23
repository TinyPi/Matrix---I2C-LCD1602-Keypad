#ifndef _CKT_ANALYSIS_H_
#define _CKT_ANALYSIS_H_

#include <stdio.h>
#include "analysis.h"
#include "unpack.h"


typedef struct
{
    int write_fd;
    int read_fd;
    package package;
}unpack_source;

#endif  //#ifndef _CKT_ANALYSIS_H_