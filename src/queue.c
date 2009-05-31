#include "queue.h"

#include <stdlib.h>
#include <stdio.h>


QueuePtr createQueue(int max) {
    QueuePtr q;
    if ((q = (QueuePtr) malloc(sizeof(Queue))) != NULL) {
        if ((q->contents = malloc(sizeof(LogBuf) * max)) == NULL) {
            free(q);
            fprintf(stderr, "Couldn't initialize queue\n");
            return NULL;
        }

        if (pthread_mutex_init(&q->mutex, NULL) != 0) {
            free(q);
            free(q->contents);
            fprintf(stderr, "Couldn't initialize queue, failed to init mutex lock\n");
            return NULL;
        }

        if (pthread_cond_init(&q->cond, NULL) != 0) {
            free(q);
            free(q->contents);
            pthread_mutex_destroy(&q->mutex);
            fprintf(stderr, "Couldn't initialize queue, failed to init condition variable\n");
            return NULL;
        }

        q->max = max;
        q->front = 0;
        q->count = 0;
    } else {
        fprintf(stderr, "Couldn't initialize queue\n");
    }
    return q;
}

int isEmpty(QueuePtr q) {
    return (q->count <= 0);
}

void enqueue(QueuePtr q, LogBuf logBuf) {
    pthread_mutex_lock(&q->mutex);
    if (q->count >= q->max) {
        pthread_mutex_unlock(&q->mutex);
        fprintf(stderr, "Queue is full q->max: %d, q->count: %d!\n", q->max, q->count);
        printQueue(q);
        exit(1);
    }

    q->contents[(q->front + q->count++) % q->max] = logBuf;

    pthread_mutex_unlock(&q->mutex);
    if (q->count == 1 ) {
        printf("Cond signal\n");
        if (pthread_cond_broadcast(&q->cond) != 0) {
            fprintf(stderr, "Couldn't signal cond variable\n");
        }
    }
}

LogBuf *dequeue(QueuePtr q) {
    pthread_mutex_lock(&q->mutex);
    if (isEmpty(q)) {
        pthread_mutex_unlock(&q->mutex);
        fprintf(stderr, "Queue is empty!\n");
        return NULL;
    }
    
    LogBuf *result;
    result = &q->contents[q->front];
    q->front++;
    q->front %= q->max;
    q->count--;
    pthread_mutex_unlock(&q->mutex);
    /* Is there a risk to overwrite same memory with next insertion at
     * contents[q->front] since it is circular? */
    return result;
}

void printQueue(QueuePtr q) {
    printf("front(%d), count(%d)\n",
           q->front, q->count);
}
