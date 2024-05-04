#ifndef _MACRO_H
#define _MACRO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARGS_CHECK(argc,num) {if((argc) != num) {fprintf(stderr,"argc error!\n");exit(-1);}}
#define ERROR_CHECK(ret,num,msg) {if(ret == num) {perror(msg);exit(-1);}}
#define THREAD_ERROR_CHECK(ret,msg) {if(ret != 0) {fprintf(stderr,"%s:%s\n",msg,strerror(ret));exit(-1);}}

#endif
