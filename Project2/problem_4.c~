#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

volatile int in_cs = 0;
volatile int *count;
int timing;


void mfence (void) {
    asm volatile ("mfence" : : : "memory");
}

struct spin_lock_t{
    volatile int lock;
};
struct spin_lock_t s;



/*
 * atomic_cmpxchg
 *
 * equivalent to atomic execution of this code:
 *
 * if (*ptr == old) {
 *   *ptr = new;
 *   return old;
 * } else {
 *   return *ptr;
 * }
 *
 */
static inline int atomic_cmpxchg (volatile int *ptr, int old, int new) {
    int ret;
    asm volatile ("lock cmpxchgl %2,%1"
                  : "=a" (ret), "+m" (*ptr)
                  : "r" (new), "0" (old)
                  : "memory");
    return ret;
}

void spin_lock(struct spin_lock_t *s){
    while(atomic_cmpxchg(&(s->lock), 0, 1));
}

void spin_unlock(struct spin_lock_t *s){
    atomic_cmpxchg(&(s->lock), 1, 0);
}


void *use_cs(void *arg){
    int thread = *((int *)arg);
    //printf("Thread started: %d\n",thread);
    while(timing){
    spin_lock(&(s));
    count[thread]++;
    assert(in_cs == 0);
    in_cs++;
    assert(in_cs == 1);
    in_cs++;
    assert(in_cs == 2);
    in_cs++;
    assert(in_cs == 3);
    in_cs = 0;
    spin_unlock(&(s));
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
