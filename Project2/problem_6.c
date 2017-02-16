#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

volatile int in_cs = 0;
volatile int total = 0;
volatile double in_circle = 0;
int timing;
pthread_mutex_t lock;


void *use_cs(){
    while(timing){
    pthread_mutex_lock(&lock);
    assert(in_cs == 0);
    in_cs++;
    assert(in_cs == 1);
    in_cs++;
    assert(in_cs == 2);
    in_cs++;
    assert(in_cs == 3);
    in_cs = 0;

    double rand_x = (double)rand() / RAND_MAX;
    double rand_y = (double)rand() / RAND_MAX;
    total++;
    if(((rand_x*rand_x) + (rand_y*rand_y)) <= 1)
        in_circle++;
    pthread_mutex_unlock(&lock);

     };
    return NULL;
}


int main(int argc, const char* argv[]){
    int num_threads, num_seconds;
    char *ptr;

    /*Error Handling*/
    if(argc != 3){
        printf("Needs exactly two integer parameters for # of threads and time to run\n");
        printf("%d\n", argc);
        return 0;
    }
    errno = 0;
    num_threads = (int)(strtol(argv[1], &ptr, 10));
    if(ptr == argv[1]){
        fprintf(stderr, "No digits entered for first arg\n");
        return 0;
    }
    num_seconds = (int)(strtol(argv[2], &ptr, 10));
    if(ptr == argv[2]){
        fprintf(stderr, "No digits entered for second arg\n");
        return 0;
    }
    if(errno == ERANGE){
        perror("strtol");
    }
    if(num_threads < 0 || num_seconds < 0){
        fprintf(stderr, "Must be positive integers");
        return 0;
    }
    /*End Error Handling*/



    pthread_t *mythreads = malloc(sizeof(pthread_t) * num_threads);
    int i;
    int create_thread = -1;

    //Create threads and begin looping until sleep() ends
    timing = 1;
    for(i = 0; i < num_threads; i++) {
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        create_thread = pthread_create(&mythreads[i], NULL, (void *)&use_cs, arg);
        if(create_thread != 0){
            fprintf(stderr, "Thread not created\n");
            exit(1);
        }
    }

    //Run threads until num_seconds is reached
    sleep(num_seconds);
    timing = 0;
    int join_thread = -1;

    //Return threads
    for(i = 0; i < num_threads; i++){
        join_thread = pthread_join(mythreads[i], NULL);
        if(join_thread != 0){
            fprintf(stderr, "Thread not joined\n");
            exit(1);
        }
    }
    //Calculates pi with the global variables used in the threads
    double pi = (4.0*in_circle)/total;
    printf("%d points were generated to produce an approximate pi value of %f\n", total, pi);

    free((void *)mythreads);

    return 0;
}
