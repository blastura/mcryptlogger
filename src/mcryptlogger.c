/*
 * Name: Anton Johansson
 * Mail: dit06ajn@cs.umu.se
 * Time-stamp: "2009-05-27 13:30:07 anton"
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
QueuePtr filledMsgQueue;
char key[127];

int global_counter;
void* sayHello(void *ptr);
int readKey(char key[], int maxkeysize);
void usage();
void usage() {
    printf("Usage: mcryptlogger -C [number of computers] -B [number of buffers] -P [number of threads]\n");
}

/* Program ska läsa in nyckeln (bestående av en sträng med max 127
 * tecken på en rad) ur filen "keys" , skapa C+P trådar, och vänta på
 * att någon av följande signaler inträffar: */

/* Skapa två köer, en med lediga buffrar och en med fyllda.
 *
 * Ha en mutex till varje kö och en condition för att representera
 * händelsen att kön blir icke-tom.
 * 
 * En läs-tråd (producent) kommer vänta tills något finns att läsa
 * från FIFOn. Låsa mutexen för kön med tomma buffrar, vänta på lediga
 * buffrar condition variabeln, ta en buffert ur kön och låsa upp
 * mutexen. Därefter kan den läsa in resten av datat i bufferten. När
 * den är färdig med det så låser den mutexen för kön med fulla
 * buffrar, stoppar in bufferten i den kön och låser upp mutexen. Om
 * buffertlistan var tom innan detta så ska den broadcasta att listan
 * inte längre är tom. Krypteringstrådarna gör motsvarande (fast mer
 * eller mindre tvärtom). */

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
            case 'C': /* Number of computers */
                nrOfComputers = atoi(optarg);
                break;
            case 'P': /* Number of threads */
                nrOfThreads = atoi(optarg);
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

    if (nrOfBuffers < 1 || nrOfThreads < 1 || nrOfComputers < 1) {
        printf("nrOfBuffers: %d\n", nrOfBuffers);
        printf("nrOfComputers: %d\n", nrOfComputers);
        printf("nrOfThreads: %d\n", nrOfThreads);
        usage();
        exit(1);
    }
    
    /* Init Queues to hold Buffers with log messages */
    freeMsgQueue = createQueue(nrOfBuffers);
    filledMsgQueue = createQueue(nrOfBuffers);
    char key[MAXKEYSIZE];
    readKey(key, MAXKEYSIZE);
    printf("key: %s\n", key);
    
    
    int threadIDs[(nrOfThreads + nrOfComputers)];
    pthread_t threadsArray[(nrOfThreads + nrOfComputers)];
    global_counter = 0;
    for (int i = 0; i < (nrOfThreads + nrOfComputers); i++) {
        threadIDs[i] = pthread_create(&threadsArray[i], NULL, sayHello, NULL);
        pthread_join(threadsArray[i], NULL);
    }
    
    sayHello(NULL);
    return 0;
}

/** Read key form file `keys' put in key[] */
int readKey(char key[], int maxkeysize) {
    FILE *keyFile;
    if ((keyFile = fopen("keys", "r")) == NULL) {
        perror("Couldn't open keys:");
        return 1;
    }
    if (fgets(key, maxkeysize, keyFile) == NULL) {
        perror("Couldn't read key:");
        return 1;
    }
    fclose(keyFile);
    return 0;
}

void* sayHello(void *ptr) {
    int id = global_counter++;
    printf("Hello World!\n");
    printf("Goodbye from tthread %d\n", id);
    return NULL;
}
