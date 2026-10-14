#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int counter;
    pthread_mutex_t* lock;
    pthread_cond_t* cv;
} ThreadData;

ThreadData data;

void *mythread1(void* arg){
    printf("%s: begin\n", (char *) arg);
    for (int i =0; i<1e7;i++) {
        pthread_mutex_lock(data.lock);
        data.counter++;
        pthread_mutex_unlock(data.lock);
    }
    printf("%s: done\n", (char *) arg);
    return NULL;
}

void *mythread2(void* arg){
    printf("%s: begin\n", (char *) arg);
    for (int i =0; i<1e7;i++) {
        while (data.counter < 500,000){
            pthread_cond_wait(cv);
            printf("%s is waiting", (char *) arg);
        }
        pthread_mutex_lock(data.lock);
        data.counter++;
        pthread_mutex_unlock(data.lock);
    }
    printf("%s: done\n", (char *) arg);
    return NULL;
}




int main(int argc, char* argv[]){
    pthread_t p1, p2;
    data.counter = 0;
    data.lock = malloc(sizeof(pthread_mutex_t));
    data.cv = malloc(sizeof(pthread_cond_t));
    


    int rc = pthread_mutex_init(data.lock, NULL);
    int cv = pthread_cond_init(data.cv, 

    if (rc != 0){
        perror("pthread_mutex_init call failed: ");
    }

    printf("main:begin (counter = %d)\n", data.counter);
    pthread_create(&p1, NULL, mythread1, "A");
    pthread_create(&p2, NULL, mythread, "B");

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("main: done with both (counter = %d)\n", data.counter);

    return 0;
}

