#include "mcryptlogger.h" /* Get Buffer typedef */

typedef struct Queue *QueuePtr;

typedef struct Queue {
    LogMsg contents[MAX_BUFS];
    int front;
    int count;
} Queue;

QueuePtr createQueue();
int isEmpty(QueuePtr q);
void enqueue(QueuePtr q, LogMsg logMsg);
LogMsg dequeue(QueuePtr q);
void printQueue(QueuePtr q);
