#include <pthread.h>
#include <semaphore.h>
//#include <dispatch/dispatch.h>
#include <stdio.h>
#include<unistd.h>

#define EATING 0
#define HUNGRY 1
#define THINKING 2
#define N 5

int status[N] = {THINKING};
sem_t s[N];
sem_t mutex;
//dispatch_semaphore_t s[N];
//dispatch_semaphore_t mutex;

void take_fork(int);
void put_fork(int);

void *phil(void*);

int main(){

    pthread_t threads[N];
    int phil_id[N] = {0,1,2,3,4};

    sem_init(&mutex,0,1);
    //mutex = dispatch_semaphore_create(1);

    for(int i = 0; i < N; i++)
      sem_init(&s[i],0,0);
    // s[i] = dispatch_semaphore_create(0);

    for(int i = 0; i < N; i++)
        pthread_create(&threads[i], NULL, phil, &phil_id[i]);

    for(int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);

    return 0;
}

void *phil(void* id){

    int* i = id;

    while(1){
        printf("Philosopher %d is Thinking\n", *i);
        sleep(1);
        take_fork(*i);
        sleep(1);
        put_fork(*i);
    }

}

void take_fork(int id){

    sem_wait(&mutex);
    //dispatch_semaphore_wait(mutex, DISPATCH_TIME_FOREVER);

    if((status[(id+4)%N] != EATING) && (status[(id+1)%N] != EATING)){
        status[id] = EATING;
        sem_post(&mutex);
        //dispatch_semaphore_signal(mutex);
    } else {
        status[id] = HUNGRY;
        printf("Philosopher %d is Hungry\n", id);
        sem_post(&mutex);
        //dispatch_semaphore_signal(mutex);
        sem_wait(&s[id]);
        //dispatch_semaphore_wait(s[id], DISPATCH_TIME_FOREVER);
    }

    printf("Philosopher %d is Eating\n", id);

}

void put_fork(int id){

    sem_wait(&mutex);
    //dispatch_semaphore_wait(mutex, DISPATCH_TIME_FOREVER);

    status[id] = THINKING;

    int id_l = (id+4)%N;
    if((status[id_l] == HUNGRY) && (status[(id_l+4)%N] != EATING)){
        status[id_l] = EATING;
        sem_post(&s[id_l]);
        //dispatch_semaphore_signal(s[id_l]);
    }

    int id_r = (id+1)%N;
    if((status[id_r] == HUNGRY) && (status[(id_r+1)%N] != EATING)){
        status[id_r] = EATING;
        sem_post(&s[id_r]);
        //dispatch_semaphore_signal(s[id_r]);
    }

    sem_post(&mutex);
    //dispatch_semaphore_signal(mutex);
}