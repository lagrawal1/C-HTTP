#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "threadpool.h"

// queue implementation of tpool_work_t
tpool_work_t* tpool_work_create(thread_func_t func_, void* arg_){
    if (func_ == NULL){
        return NULL;
    }
    
    tpool_work_t* work = malloc(sizeof(tpool_work_t));
    work->func = func_;
    work->arg = arg_;
    work->next = NULL;
    return work;

}

static void tpool_work_destroy(tpool_work_t* work){
    if (work == NULL){
        return;
    }
    free(work);
    return;
}

static tpool_work_t* tpool_work_deque(tpool_t* tpool ){

    if (tpool == NULL){
        return NULL;
    }

    if (tpool->work_first == NULL){
        return NULL;
    }

    tpool_work_t* work = tpool->work_first;

    if (work->next == NULL)
        tpool->work_last = NULL;

    tpool->work_first = work->next;
    return work;
}

// return false is unable to enqueue work, or true if work enqueued
bool tpool_work_enqueue(tpool_t* tpool, thread_func_t func_, void* arg_){
    if (tpool == NULL){
        return false;
    }

    tpool_work_t* work = tpool_work_create(func_, arg_);

    if (work == NULL){
        return false;
    }

    pthread_mutex_lock(&tpool->work_lock);

    if (tpool->work_first == NULL){
        tpool->work_last = tpool->work_first = work;
    } else {
        tpool->work_last->next = work;
        tpool->work_last = work;
    }

    pthread_cond_broadcast(&tpool->work_cond);
    pthread_mutex_unlock(&tpool->work_lock);
    return true;
}


static void* tpool_worker(void* arg_){
    tpool_t* tpool = arg_;

    tpool_work_t* work;


    while (1){
        pthread_mutex_lock(&tpool->work_mutex);

        while (tpool->work_first == NULL && !tpool->stop)
            pthread_cond_wait(&tpool->work_cond, &tpool->work_mutex);

        if (tpool->stop)
            break;

        tpool_work_t* work = tpool_work_deque(tpool);
        tpool->working_cnt++;

        pthread_work_unlock(&tpool->work_mutex);

        if (work!=NULL){
            work->func(work->arg);
            tpool_work_destroy(work);
        }

        pthread_mutex_lock(&tpool->work_lock);
        tpool->work_cnt--;

        if (!tpool->stop && )

    }


}

void* test(void* arg){
    printf("Made Thread");
    return NULL;
}

int main(){
    return 0;
}
