#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

typedef int bool;
enum{false, true};
volatile bool *entering_thread;
volatile int *number;

volatile int in_cs = 0;
volatile int *count;
bool timing;
int global_num_threads;

void lock(int thread){
    entering_thread[thread] = true;
    int maxthread = 0;
    int i;
    for(i = 0; i < global_num_threads; i++){
        int thread_value = number[i];
        if(maxthread < thread_value){
            maxthread = thread_value;
        }
    }
    number[thread] = maxthread + 1;
    entering_thread[thread] = false;
    int j;
    for(j = 0; j < global_num_threads; j++){
        while(entering_thread[j]){sched_yield();}
        while((number[j] != 0) &&
              (number[j] < number[thread] ||
               (number[j] == number[thread] && j < thread))){sched_yield();}
    }
}

void unlock(int thread){
    number[thread] = 0;
}

void *use_cs(void *arg){
    int thread = *((int *)arg);
    //printf("Thread started: %d\n",thread);
    while(timing){
    lock(thread);
    count[thread]++;
    assert(in_cs == 0);
    in_cs++;
    assert(in_cs == 1);
    in_cs++;
    assert(in_cs == 2);
    in_cs++;
    assert(in_cs == 3);
    in_cs = 0;
     unlock(thread);
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

    //Allocate memory for arrays of thread position, thread start/stop, and threads themselves
    entering_thread = malloc(sizeof(bool) * num_threads);
    number = malloc(sizeof(int) * num_threads);
    count = malloc(sizeof(int) * num_threads);
    pthread_t *mythreads = malloc(sizeof(pthread_t) * num_threads);
    int i;
    global_num_threads = num_threads;
    int create_thread = -1;
    for(i = 0; i < num_threads; i++){
        entering_thread[i] = false;
        number[i] = 0;
        count[i] = 0;
    }

    //This loop is what I used to prove that without taskset -c 1, my lock implementation would fail
    //I had to let this run a couple minutes before an assert would fail.
    //Running this loop with taskset would cause the program to finish the given time * 1000
    // int j;
    //for(j = 0; j < 1000; j++){

    //Create threads and begin looping until sleep() ends
    timing = true;
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
    timing = false;
    int join_thread = -1;

    //Return threads
    for(i = 0; i < num_threads; i++){
        join_thread = pthread_join(mythreads[i], NULL);
        if(join_thread != 0){
            fprintf(stderr, "Thread not joined\n");
            exit(1);
        }
    }

    //Print the count each thread entered its critical section
    for(i = 0; i < num_threads; i++){
        printf("Thread %d entered the critical section %d times\n", i, count[i]);
        count[i] = 0;
    }
    // }

    free((void *)entering_thread);
    free((void *)number);
    free((void *)count);
    free((void *)mythreads);






    return 0;
}
