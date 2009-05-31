#define LOG_MSG_SIZE 4000
#define MAXKEYSIZE 127
#define MAX_BUFS 4

#ifndef LOGBUF
//typedef struct my_struct_type my_short_type_t;
#define LOGBUF 1

#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
    int fifo;
    int n;
    unsigned char *message;
} LogBuf;
#endif /* LOGBUF  */
