#ifndef MCRYPTLOGGER_H
#define MCRYPTLOGGER_H

#define LOG_MSG_SIZE 4000
#define MAXKEYSIZE 127
#define MAX_BUFS 4

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
    unsigned char *message;
} LogBuf;

#endif
