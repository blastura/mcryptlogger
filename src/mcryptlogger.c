/*
 * Name: Anton Johansson
 * Mail: dit06ajn@cs.umu.se
 * Time-stamp: "2009-05-25 21:37:27 anton"
 */

#include "mcryptlogger.h"
#include "queue.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>

int global_counter;
void* sayHello(void *ptr);
void usage();
void usage() {
    printf("Usage: mcryptlogger -C [number of computers] -B [number of buffers] -P [number of threads]");
}

void testQueue() {
    /* Test queue */
    LogMsg l1 = {1, 2, "hej"};
    LogMsg l2 = {1, 2, "hej nummer 2"};
    LogMsg l3 = {1, 2, "hej nummer 3"};
    LogMsg l4 = {1, 2, "hej nummer 4"};

    printf("msg: %s\n", l1.message);
    
    QueuePtr q = createQueue();
    printQueue(q);
    
    enqueue(q, l1);
    printQueue(q);
    
    enqueue(q, l2);
    printQueue(q);
    
    enqueue(q, l3);
    printQueue(q);
    
    enqueue(q, l4);
    printQueue(q);
    
    printf("dequeue: %s\n", dequeue(q).message);
    printQueue(q);
    
    enqueue(q, l1);
/*     printQueue(q); */
    
    printf("dequeue: %s\n", dequeue(q).message);
    printQueue(q);
    
    printf("dequeue: %s\n", dequeue(q).message);
    printQueue(q);
    
    printf("dequeue: %s\n", dequeue(q).message);
    printQueue(q);
    
    enqueue(q, l2);
    printQueue(q);
    
    enqueue(q, l3);
    printQueue(q);
    
    enqueue(q, l4);
    printQueue(q);


}

/* Program ska läsa in nyckeln (bestående av en sträng med max 127
 * tecken på en rad) ur filen "keys" , skapa C+P trådar, och vänta på
 * att någon av följande signaler inträffar: */

/* Skapa två köer, en med lediga buffrar och en med fyllda. Ha en
 * mutex till varje kö, och en condition för att representera
 * händelsen att kön blir icke-tom. En läs-tråd (producent) kommer
 * vänta tills något finns att läsa från FIFOn. Låsa mutexen för kön
 * med tomma buffrar, vänta på lediga buffrar condition variabeln, ta
 * en buffert ur kön och låsa upp mutexen. Därefter kan den läsa in
 * resten av datat i bufferten. När den är färdig med det så låser den
 * mutexen för kön med fulla buffrar, stoppar in bufferten i den kön
 * och låser upp mutexen. Om buffertlistan var tom innan detta så ska
 * den broadcasta att listan inte längre är tom. Krypteringstrådarna
 * gör motsvarande (fast mer eller mindre tvärtom). */

int main(int argc, char **argv) {
    /* Flags: Den första kommandoradsparameter (C) är antalet datorer
       som kan generera logmeddelanden, Det andra
       komandoradsparametern (P) är antalet trådar som ska användas
       för krypteringen, och det tredje talet B är antalet buffrar att
       använda. Därefter ska C+P trådar startas.
    */
    
    // Check for flags
    int nrOfThreads;
    int nrOfComputers;
    int nrOfBuffers;
    int ch;
    const char *optstring = "C:P:B:";
    while((ch = getopt(argc, argv, optstring)) != -1) {
        switch (ch) {
            case 'C': // Number of computers
                nrOfComputers = atoi(optarg);
                break;
            case 'P': // Number of threads
                nrOfThreads = atoi(optarg);
                break;
            case 'B': // Number of buffers
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

    if (nrOfBuffers < 1 || nrOfThreads < 1 || nrOfComputers < 1) {
        printf("nrOfBuffers: %d\n", nrOfBuffers);
        printf("nrOfComputers: %d\n", nrOfComputers);
        printf("nrOfThreads: %d\n", nrOfThreads);
        usage();
        exit(1);
    }
    
    testQueue();
    
    global_counter = 0;
    int threadID;
    pthread_t the_other_thread;
    threadID = pthread_create(&the_other_thread,
                              NULL, sayHello,
                              NULL);
    
    pthread_join(the_other_thread, NULL);
    sayHello(NULL);
    return 0;
}

void* sayHello(void *ptr) {
    int id = global_counter++;
    printf("Hello World!\n");
    printf("Goodbye from tthread %d\n", id);
    return NULL;
}
