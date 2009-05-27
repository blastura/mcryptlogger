#include "queue.h"

#include <stdlib.h>
#include <stdio.h>


QueuePtr createQueue(int max) {
    QueuePtr q;
    q = (QueuePtr) malloc(sizeof(Queue));
    q->contents = malloc(sizeof(LogMsg) * max);
    q->max = max;    
    q->front = 0;
    q->count = 0;
    return q;
}

int isEmpty(QueuePtr q) {
    return (q->count == 0);
}

void enqueue(QueuePtr q, LogMsg logMsg) {
    if (q->count >= q->max) {
        fprintf(stderr, "Queue is full!\n");
        exit(1);
    }
    q->contents[(q->front + q->count++) % q->max] = logMsg;
}

LogMsg dequeue(QueuePtr q) {
    if (q->count <= 0) {
        fprintf(stderr, "Queue is empty!\n");
        exit(1);
    }
    LogMsg result = q->contents[q->front];
    q->front++;
    q->front %= q->max;
    q->count--;
    return result;
}

void printQueue(QueuePtr q) {
    printf("front(%d), count(%d)\n",
           q->front, q->count);
}
