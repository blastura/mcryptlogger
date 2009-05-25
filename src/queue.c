#include "queue.h"

#include <stdlib.h>
#include <stdio.h>


QueuePtr createQueue() {
    QueuePtr q;
    q = (QueuePtr) malloc(sizeof(Queue));
    q->front = 0;
    q->count = 0;
    return q;
}

int isEmpty(QueuePtr q) {
    return (q->count == 0);
}

void enqueue(QueuePtr q, LogMsg logMsg) {
    if (q->count >= MAX_BUFS) {
        fprintf(stderr, "Queue is full!\n");
        exit(1);
    }
    q->contents[(q->front + q->count++) % MAX_BUFS] = logMsg;
}

LogMsg dequeue(QueuePtr q) {
    if (q->count <= 0) {
        fprintf(stderr, "Queue is empty!\n");
        exit(1);
    }
    LogMsg result = q->contents[q->front];
    q->front++;
    q->front %= MAX_BUFS;
    q->count--;
    return result;
}

void printQueue(QueuePtr q) {
    printf("front(%d), count(%d)\n",
           q->front, q->count);
}
