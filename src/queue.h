#include "mcryptlogger.h" /* Get Buffer typedef */

typedef struct Queue *QueuePtr;

typedef struct Queue {
    int front;
    int count;
    int max;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    LogBuf *contents;
} Queue;

QueuePtr createQueue(int max);
int isEmpty(QueuePtr q);
void enqueue(QueuePtr q, LogBuf logBuf);
LogBuf *dequeue(QueuePtr q);
void printQueue(QueuePtr q);
