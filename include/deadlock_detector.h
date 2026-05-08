#ifndef DEADLOCK_DETECTOR_H
#define DEADLOCK_DETECTOR_H

#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define MAX_THREADS 32

typedef struct {
    char threadId[16];
    bool waiting;
    char waitingFor[16];
    time_t waitStart;
} ThreadState;

typedef struct {
    ThreadState threads[MAX_THREADS];
    int threadCount;
    pthread_mutex_t stateMutex;
    int deadlockCount;
} DeadlockDetector;

DeadlockDetector* deadlock_detector_create(void);
void deadlock_detector_destroy(DeadlockDetector *detector);
void deadlock_detector_register_thread(DeadlockDetector *detector, const char *threadId);
void deadlock_detector_set_waiting(DeadlockDetector *detector, const char *threadId, const char *waitingFor);
void deadlock_detector_set_ready(DeadlockDetector *detector, const char *threadId);
void deadlock_detector_check(DeadlockDetector *detector);
int deadlock_detector_get_count(DeadlockDetector *detector);

#endif
