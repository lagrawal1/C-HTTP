#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

typedef void* (*thread_func_t)(void*);
typedef struct tpool_work_t tpool_work_t;
typedef struct tpool_t tpool_t;

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void * arg){
    int rv;

    if ((rv = pthread_create(thread, attr, start_routine, arg))!=0)
        fprintf(stderr, "pthread_create: %s\n", strerror(rv));
    return; 
}

void Pthread_join(pthread_t thread,void **value_ptr){
    int rv;

    if ((rv = pthread_join(thread, value_ptr)) != 0)
        fprintf(stderr, "pthread_join: %s\n", strerror(rv));
    return;
}

typedef struct tpool_work_t {
    thread_func_t func;
    void* arg;
    tpool_work_t* next;
} tpool_work_t;


typedef struct tpool_t {
    pthread_t* pool;
    size_t thread_cnt;

    pthread_cond_t work_cond; // use to signal when work arrives
    pthread_mutex_t work_lock; // use for locking    
    pthread_cond_t working_cond; // use to signal when no threads are working

    size_t working_count;

    tpool_work_t* work_first;
    tpool_work_t* work_last;
    bool stop;
} tpool_t;

void tpool_init(tpool_t* tpool, size_t thread_cnt_, pthread_t* pool_){
    tpool->pool = pool_;
    int rc;

    if ((rc = pthread_mutex_init(&tpool->work_lock, NULL)) != 0){
        fprintf(stderr, "pthread_mutex_init: %s\n", strerror(rc));
        exit(1);
    }

    if ((rc = pthread_cond_init(&tpool->work_cond, NULL)) !=0){
        fprintf(stderr, "pthread_cond_init: %s\n", strerror(rc));
        exit(1);
    }

    if ((rc = pthread_cond_init(&tpool->working_cond, NULL))!=0 ){
        fprintf(stderr, "pthread_cond_init: %s\n", strerror(rc));
        exit(1);
    }

    tpool->work_first = NULL, tpool->work_last = NULL;
}

#endif
