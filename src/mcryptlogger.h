#define LOG_MSG_SIZE 4000
#define MAXKEYSIZE 127
#define MAX_BUFS 4

#ifndef LOGMSG
//typedef struct my_struct_type my_short_type_t;
#define LOGMSG 1

typedef struct {
    int n;
    int fifo;
    unsigned char message[LOG_MSG_SIZE];
} LogMsg;
#endif /* LOGMSG  */
