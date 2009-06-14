#include "queue.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void createQueue(Queue *q, int max) {
    if ((q->contents = (LogBuf **) malloc(max * sizeof(LogBuf*))) == NULL) {
        perror("Init queue:");
        fprintf(stderr, "Couldn't initialize queue\n");
        exit(1);
    }

    int i;
    for (i = 0; i < max; i++) {
        if ((q->contents[i] = (LogBuf *) malloc(sizeof(LogBuf))) == NULL) {
            perror("Init queue:");
            fprintf(stderr, "Couldn't initialize queue\n");
            exit(1);
        }
    }

    if (pthread_mutex_init(&q->mutex, NULL) != 0) {
        free(q);
        free(q->contents);
        fprintf(stderr, "Couldn't initialize queue, failed to init mutex lock\n");
        exit(1);
    }

    if (pthread_cond_init(&q->cond, NULL) != 0) {
        free(q);
        free(q->contents);
        pthread_mutex_destroy(&q->mutex);
        fprintf(stderr,
                "Couldn't initialize queue, failed to init condition variable\n");
        exit(1);
    }

    q->max = max;
    q->front = 0;
    q->count = 0;
}

void destroyQueue(Queue *q) {
    // free queue and buffers in it?   
}

int isEmpty(Queue *q) {
    return (q->count <= 0);
}

void enqueue(Queue *q, LogBuf *logBuf) {
    fprintf(stderr, "enqueue: waiting for lock!, %d\n", q->count);
    pthread_mutex_lock(&q->mutex);
    fprintf(stderr, "enqueue: locked!, %d\n", q->count);
    if (q->count >= q->max) {
        pthread_mutex_unlock(&q->mutex);
        fprintf(stderr, "Queue is full q->max: %d, q->count: %d!\n",
                q->max, q->count);
        printQueue(q);
        exit(1);
    }

    q->contents[(q->front + q->count++) % q->max] = logBuf;

    pthread_mutex_unlock(&q->mutex);
    fprintf(stderr, "dequeue: unlocked!, %d\n", q->count);
    if (q->count == 1 ) {
        fprintf(stderr, "Queue: not empty! Cond signal, %d\n", q->count);
        if (pthread_cond_signal(&q->cond) != 0) {
            fprintf(stderr, "Couldn't signal cond variable\n");
        }
    }
}

LogBuf* dequeue(Queue *q) {
    pthread_mutex_lock(&q->mutex);
    fprintf(stderr, "dequeue: waiting for lock!, %d\n", q->count);
    fprintf(stderr, "dequeue: locked!, %d\n", q->count);
    while (isEmpty(q)) {
        fprintf(stderr, "dequeue: Queue is empty, waiting for cond!\n");
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    fprintf(stderr, "Queue: dequeueing!, %d\n", q->count);

    LogBuf *result;
    result = q->contents[q->front];
    q->front++;
    q->front %= q->max;
    q->count--;
    pthread_mutex_unlock(&q->mutex);
    fprintf(stderr, "dequeue: unlocked!, %d\n", q->count);
    /* Is there a risk to overwrite same memory with next insertion at
     * contents[q->front] since it is circular? */
    return result;
}

void printQueue(Queue *q) {
    pthread_mutex_lock(&q->mutex);
    printf("front(%d), count(%d)\n",
           q->front, q->count);
    pthread_mutex_unlock(&q->mutex);
}

int testQueue() {
    int nrOfBuffers = 4;
    Queue q;
    createQueue(&q, nrOfBuffers);
    assert(&q != NULL);
    assert(isEmpty(&q));
    
    /* Create logbufs */
    int i;
    for (i = 0; i < nrOfBuffers; i++) {
        LogBuf *b;
        b = (LogBuf*) malloc(sizeof(LogBuf));
        printf("LogBuf address: %p\n", b);
        printf("fifo address: %p\n", &b->fifo);
        b->fifo = i;
        b->message = malloc(nrOfBuffers + 1);
        printf("message address: %p\n", b->message);
        char msg[(nrOfBuffers + 1)]; // TODO: fileLength
        sprintf(msg, "%d", i);
        b->message = (unsigned char*) msg;
        printf("============== b->message %s\n", b->message);
        
        //assert((int) b->message == (int) "hello ey");
        assert(b->fifo == i);
        
        enqueue(&q, b);
    }
    
    assert(!isEmpty(&q));
    
    for (i = 0; i < nrOfBuffers; i++) {
        LogBuf *result = dequeue(&q);
        printf("fifo: %d, i: %d\n", result->fifo, i);
        printf("fifo address: %p\n", result);
        assert(result->fifo == i);
        char msg[(nrOfBuffers + 1)]; // TODO: fileLength
        sprintf(msg, "%d", i);
        assert((int) result->message == (int) msg);
        printf("============== b->message %s, address: %p\n", result->message, &result->message);
        /* free(result->message); */
        /* free(result); */
    }
    assert(isEmpty(&q));
    destroyQueue(&q);
    //exit(0);
    return 0;
}
