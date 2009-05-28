/*
 * Name: Anton Johansson
 * Mail: dit06ajn@cs.umu.se
 * Time-stamp: "2009-05-28 23:46:40 anton"
 */

#include "mcryptlogger.h"
#include "queue.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>


QueuePtr freeMsgQueue;
pthread_mutex_t freeQueueMutex;
pthread_cond_t freeQueueCond;

QueuePtr filledMsgQueue;
pthread_mutex_t filledQueueMutex;
pthread_cond_t filledQueueCond;

char key[127];
int global_counter;
void *sayHello(void *ptr);
void *readThreadInit(void *ptr);
void *cryptThreadInit(void *ptr);
int readKey(char key[], int maxkeysize);

void usage() {
    printf("Usage: mcryptlogger -C [number of computers] -B [number of buffers] -P [number of threads]\n");
}

int main(int argc, char **argv) {
    // Check for flags
    int nrOfCryptThreads;
    int nrOfReadThreads;
    int nrOfBuffers;
    int ch;
    const char *optstring = "C:P:B:";
    while((ch = getopt(argc, argv, optstring)) != -1) {
        switch (ch) {
            case 'P': /* Number of read-threads */
                nrOfReadThreads = atoi(optarg);
                break;
            case 'C': /* Number of computers (crypt-threads) */
                nrOfCryptThreads = atoi(optarg);
                break;
            case 'B': /* Number of buffers */
                nrOfBuffers = atoi(optarg);
                break;
            case '?':
            default:
                usage();
                exit(1);
        }
    }
    argc -= optind;
    argv += optind;

    if (nrOfBuffers < 1 || nrOfCryptThreads < 1 || nrOfReadThreads < 1) {
        printf("nrOfBuffers: %d\n", nrOfBuffers);
        printf("nrOfCryptThreads: %d\n", nrOfCryptThreads);
        printf("nrOfReadThreads: %d\n", nrOfReadThreads);
        usage();
        exit(1);
    }

    /* Init Queues to hold Buffers with log messages */
    freeMsgQueue = createQueue(nrOfBuffers);
    filledMsgQueue = createQueue(nrOfBuffers);

    /* Read key from file `keys' */
    char key[MAXKEYSIZE];
    readKey(key, MAXKEYSIZE);
    printf("key: %s\n", key);

    /* Init mutexs */
    if (pthread_mutex_init(&freeQueueMutex, NULL) != 0
        || pthread_mutex_init(&filledQueueMutex, NULL) != 0) {
        fprintf(stderr, "Failed to init mutex\n");
    }

    /* Init condition variables */
    if(pthread_cond_init(&freeQueueCond, NULL) != 0
       || pthread_cond_init(&filledQueueCond, NULL) != 0) {
        fprintf(stderr, "Failed to init condition variables.\n");
    }
    
    /* Start read threads */
    pthread_t readThreadArray[nrOfReadThreads];
    for (int i = 0; i < nrOfReadThreads; i++) {
        if (pthread_create(&readThreadArray[i], NULL, readThreadInit, "hello") != 0) {
            fprintf(stderr, "Failed to create thread.\n");
        }
        pthread_join(readThreadArray[i], NULL);
    }

    /* Start crypt threads */
    pthread_t cryptThreadArray[nrOfCryptThreads];
    for (int i = 0; i < nrOfCryptThreads; i++) {
        if (pthread_create(&cryptThreadArray[i], NULL, cryptThreadInit, "hello") != 0) {
            fprintf(stderr, "Failed to create thread.\n");
        }
        pthread_join(cryptThreadArray[i], NULL);
    }

    sayHello("Förälder");
    return 0;
}

/** Read key form file `keys' put in key[] */
int readKey(char key[], int maxkeysize) {
    FILE *keyFile;
    if ((keyFile = fopen("keys", "r")) == NULL
        || fgets(key, maxkeysize, keyFile) == NULL) {
        perror("Couldn't open keys:");
        return 1;
    }
    
    fclose(keyFile);
    return 0;
}

void *readThreadInit(void *ptr) {
    int id = global_counter++;
    printf("Hello readThread!%s\n", (char*) ptr);
    printf("Goodbye from tthread %d\n", id);
    return NULL;
}


void *cryptThreadInit(void *ptr) {
    int id = global_counter++;
    printf("Hello cryptThreadInit!%s\n", (char*) ptr);
    printf("Goodbye from tthread %d\n", id);
    return NULL;
}

void* sayHello(void *ptr) {
    int id = global_counter++;
    printf("Hello World!%s\n", (char*) ptr);
    printf("Goodbye from tthread %d\n", id);
    return NULL;
}
