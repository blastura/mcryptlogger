/*
 * Name: Anton Johansson
 * Mail: dit06ajn@cs.umu.se
 * Time-stamp: "2009-06-01 19:08:35 anton"
 */

#include "mcryptlogger.h"
#include "queue.h"
#include "xorcrypt.h"

/* Variables */
static QueuePtr freeBufQueue;
static QueuePtr filledBufQueue;

static char key[127];
static int *fifo_count;
static pthread_mutex_t fifo_mutex;
static int nrOfReadThreads; // Number of fifos

/* Methods */
void *readThreadInit(void *ptr);
void *cryptThreadInit(void *ptr);
int readKey(char key[], int maxkeysize);

void usage() {
    printf("Usage: mcryptlogger -C [number of computers] -B [number of buffers] -P [number of threads]\n");
}

int main(int argc, char* const argv[]) {
    // Check for flags
    int nrOfCryptThreads = 0;
    nrOfReadThreads = 0;
    int nrOfBuffers = 0;
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
    freeBufQueue = createQueue(nrOfBuffers);
    filledBufQueue = createQueue(nrOfBuffers);
    printf("filledbufqueue: "); printQueue(filledBufQueue);
    int i;
    for (i = 0; i < nrOfBuffers; i++) {
        LogBuf b;
        b.fifo = -1;
        b.message = malloc(LOG_MSG_SIZE);
        enqueue(freeBufQueue, b);
        printf("enqueueing%d\n", i);
    }

    printf("freeBufQueue: "); printQueue(freeBufQueue);

    /* Read key from file `keys' */
    readKey(key, MAXKEYSIZE);
    printf("key: %s\n", key);

    /* Init fifo_count and fifo_mutex */
    fifo_count = malloc(nrOfReadThreads);
    if (pthread_mutex_init(&fifo_mutex, NULL) != 0) {
        fprintf(stderr, "Couldn't init fifo_mutex\n");
        exit(1);
    }

    /* Start read threads */
    pthread_t readThreadArray[nrOfReadThreads];
    for (i = 0; i < nrOfReadThreads; i++) {
        if (pthread_create(&readThreadArray[i], NULL, readThreadInit,
                           (void*) i) != 0) {
            fprintf(stderr, "Failed to create thread.\n");
        }
    }

    /* Start crypt threads */
    pthread_t cryptThreadArray[nrOfCryptThreads];
    for (i = 0; i < nrOfCryptThreads; i++) {
        if (pthread_create(&cryptThreadArray[i], NULL, cryptThreadInit,
                           (void*) i) != 0) {
            fprintf(stderr, "Failed to create thread.\n");
        }
    }

    // Wait for read threads
    for (i = 0; i < nrOfReadThreads; i++) {
        pthread_join(readThreadArray[i], NULL);
    }

    // Wait for crypt threads
    for (i = 0; i < nrOfCryptThreads; i++) {
        pthread_join(cryptThreadArray[i], NULL);
    }

    /* TODO: Cleanup free stuff */
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

/** Function to initialize readThreads */
void *readThreadInit(void *ptr) {
    int id = (int) ptr;
    printf("Hello from readthread %d\n", id);
    printf("filledBufQueue from readThread: "); printQueue(filledBufQueue);
    char fifo_name[2]; // TODO: fileLength
    sprintf(fifo_name, "%d", id);
    unlink(fifo_name); // Remove possibly existing fifo
    /* Create fifo */
    if (mkfifo(fifo_name, S_IRWXU) != 0) {
        fprintf(stderr, "Failed to create fifo");
        exit(1);
    }

    while (1) {
        int fd;
        /* Open fifo for reading */
        if ((fd = open(fifo_name, O_RDONLY)) < 0) {
            fprintf(stderr, "Couldn't open fifo '%s' for reading", fifo_name);
            unlink(fifo_name);
            exit(1);
        }

        int n;
        unsigned char rbuf[LOG_MSG_SIZE];
        /* Wait for one byte from fifo */
        if ((n = read(fd, rbuf, 1)) == 1) {
            //write(STDOUT_FILENO, buf, n);
            // TODO: fix to while loop
        }

        /* Wait and read one byte from fifo */
        LogBuf *lbuf;
        /* Wait for a free buffer to fill */
        lbuf = dequeue(freeBufQueue);
        printf("ReadThread: Got a freeBuf.dequeue\n");
        printQueue(filledBufQueue);

        /* Read rest of content (LOG_MSG_SIZE - 1) from fifo */
        if ((n = read(fd, rbuf, (LOG_MSG_SIZE - 1))) == (LOG_MSG_SIZE - 1)) {
            /* Create buffert containing message and fifo number */
            lbuf->fifo = id;
            lbuf->message = rbuf;// = {id, buf};
            fprintf(stderr, "ReadThread: enqueuing new message\n");
            printQueue(filledBufQueue);
            enqueue(filledBufQueue, *lbuf);
        } else {
            fprintf(stderr, "ReadThread: Logg-message of incorrect size: %d\n", n);
        }
        close(fd);
    }

    unlink(fifo_name);

    printf("Goodbye from read Thread %d\n", id);
    return NULL;
}

void *cryptThreadInit(void *ptr) {
    int id = (int) ptr;
    printf("Hello cryptThreadInit! %d\n", id);

    while (1) {

        /* Wait for filledQueueCond, if another thread steals buffer
         * queue is still returns NULL then wait again */
        LogBuf *lbuf = dequeue(filledBufQueue);
        fprintf(stderr, "CryptThread: got a filledBufQueue.dequeue\n");

        printf("message to crypt fifo: %d\n", lbuf->fifo);
        //printf("message to crypt: %s\n", lbuf->message);

        unsigned char *cryptMsg = xorcrypt(lbuf->message, LOG_MSG_SIZE, (unsigned char*) key);
        printf("cryptmsg: %s\n", cryptMsg);
        pthread_mutex_lock(&fifo_mutex);
        printf("fifo: %d, fifo_count: %d\n", lbuf->fifo, fifo_count[lbuf->fifo]++);
        pthread_mutex_unlock(&fifo_mutex);
    }

    printf("Goodbye from crypt thread %d\n", id);
    return NULL;
}
