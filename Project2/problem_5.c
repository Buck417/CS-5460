#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

volatile int in_cs = 0;
volatile int *count;
int timing;

struct fair_spin_lock_t{
    volatile int my_num;
    volatile int current_num;
};
struct fair_spin_lock_t s;



/*
 * atomic_xadd
 *
 * equivalent to atomic execution of this code:
 *
 * return (*ptr)++;
 *
 */
static inline int atomic_xadd(volatile int *ptr) {
    register int val __asm__("eax") = 1;
    asm volatile("lock xaddl %0,%1"
                 : "+r" (val)
                 : "m" (*ptr)
                 : "memory"
        );
    return val;
}

void fair_spin_lock(struct fair_spin_lock_t *s){
    volatile int my_ticket = atomic_xadd(&(s->my_num));
    while(my_ticket != (s->current_num));
}

void fair_spin_unlock(struct fair_spin_lock_t *s){
    atomic_xadd(&(s->current_num));
}


void *use_cs(void *arg){
    int thread = *((int *)arg);
    //printf("Thread started: %d\n",thread);
    while(timing){
        fair_spin_lock(&(s));
    count[thread]++;
    assert(in_cs == 0);
    in_cs++;
    assert(in_cs == 1);
    in_cs++;
    assert(in_cs == 2);
    in_cs++;
    assert(in_cs == 3);
    in_cs = 0;
    fair_spin_unlock(&(s));
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


    count = malloc(sizeof(int) * num_threads);
    pthread_t *mythreads = malloc(sizeof(pthread_t) * num_threads);
    int i;
    int create_thread = -1;
    s.my_num = 0;
    s.current_num = 0;
    for(i = 0; i < num_threads; i++){
        count[i] = 0;
    }

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

    //Print the count each thread entered its critical section
    for(i = 0; i < num_threads; i++){
        printf("Thread %d entered the critical section %d times\n", i, count[i]);
        count[i] = 0;
    }

    free((void *)count);
    free((void *)mythreads);


    return 0;
}
