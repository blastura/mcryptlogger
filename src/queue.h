#include "mcryptlogger.h" /* Get Buffer typedef */

typedef struct {
    int front;
    int count;
    int max;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    LogBuf **contents;
} Queue;

void createQueue(Queue *q, int max);
void destroyQueue(Queue *q);
int isEmpty(Queue *q);
void enqueue(Queue *q, LogBuf *logBuf);
LogBuf *dequeue(Queue *q);
void printQueue(Queue *q);
int testQueue();
