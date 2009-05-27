#include "mcryptlogger.h" /* Get Buffer typedef */

typedef struct Queue *QueuePtr;

typedef struct Queue {
    int front;
    int count;
    int max;
    LogMsg *contents;//[MAX_BUFS];
} Queue;

QueuePtr createQueue(int max);
int isEmpty(QueuePtr q);
void enqueue(QueuePtr q, LogMsg logMsg);
LogMsg dequeue(QueuePtr q);
void printQueue(QueuePtr q);
