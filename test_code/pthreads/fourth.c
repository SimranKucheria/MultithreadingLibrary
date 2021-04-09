
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
static volatile int glob = 0;
static pthread_spinlock_t splock;
static pthread_mutex_t mtx;
static int useMutex = 0;
static int numOuterLoops;
static int numInnerLoops;

static void *
threadFunc(void *arg)
{
    int s;

    for (int j = 0; j < numOuterLoops; j++) {
        if (useMutex) {
            s = pthread_mutex_lock(&mtx);
            if (s != 0)
                //errExitEN(s, "pthread_mutex_lock");
                printf("hiS");
        } else {
            s = pthread_spin_lock(&splock);
            if (s != 0)
                //errExitEN(s, "pthread_spin_lock");
                 printf("hiS");
        }

        for (int k = 0; k < numInnerLoops; k++)
            glob++;

        if (useMutex) {
            s = pthread_mutex_unlock(&mtx);
            if (s != 0)
                //errExitEN(s, "pthread_mutex_unlock");
                 printf("hi");
        } else {
            s = pthread_spin_unlock(&splock);
            if (s != 0)
                //errExitEN(s, "pthread_spin_unlock");
                 printf("hi");
        }
    }

    return NULL;
}

static void
usageError(char *pname)
{
    fprintf(stderr,
            "Usage: %s [-s] num-threads "
            "[num-inner-loops [num-outer-loops]]\n", pname);
    fprintf(stderr,
            "    -q   Don't print verbose messages\n");
    fprintf(stderr,
            "    -s   Use spin locks (instead of the default mutexes)\n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    int opt, s;
    int numThreads;
    pthread_t *thread;
    int verbose;

    /* Prevent runaway/forgotten process from burning up CPU time forever */

    alarm(120);         /* Unhandled SIGALRM will kill process */

    useMutex = 0;
    verbose = 1;
    while ((opt = getopt(argc, argv, "qs")) != -1) {
        switch (opt) {
        case 'q':
            verbose = 0;
            break;
        case 's':
            printf("here");
            useMutex = 0;
            break;
        default:
            usageError(argv[0]);
        }
    }

    if (optind >= argc)
        usageError(argv[0]);

    numThreads = atoi(argv[optind]);
    numInnerLoops = (optind + 1 < argc) ? atoi(argv[optind + 1]) : 1;
    numOuterLoops = (optind + 2 < argc) ? atoi(argv[optind + 2]) : 10000000;

    if (verbose) {
        printf("Using %s\n", useMutex ? "mutexes" : "spin locks");
        printf("\tthreads: %d; outer loops: %d; inner loops: %d\n",
                numThreads, numOuterLoops, numInnerLoops);
    }

    thread = calloc(numThreads, sizeof(pthread_t));
    if (thread == NULL)
        printf("calloc");

    if (useMutex) {
        s = pthread_mutex_init(&mtx, NULL);
        if (s != 0)
           // errExitEN(s, "pthread_mutex_init");
            printf("hi");
    } else {
        s = pthread_spin_init(&splock, 0);
        if (s != 0)
           // errExitEN(s, "pthread_spin_init");
            printf("hi");
    }

    for (int j = 0; j < numThreads; j++) {
        s = pthread_create(&thread[j], NULL, threadFunc, NULL);
        if (s != 0)
           // errExitEN(s, "pthread_create");
            printf("hi");
    }

    for (int j = 0; j < numThreads; j++) {
        s = pthread_join(thread[j], NULL);
        if (s != 0)
            printf("hi");
            //errExitEN(s, "pthread_join");
    }

    if (verbose){
        printf("glob = %d\n", glob);
    }
    exit(EXIT_SUCCESS);
}
