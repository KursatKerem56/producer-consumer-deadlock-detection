#ifndef PRODUCER_H
#define PRODUCER_H

#include "buffer.h"
#include "deadlock_detector.h"
#include <pthread.h>

typedef struct {
    char id[16];
    Buffer *targetBuffer;
    int intervalMs;
    pthread_t thread;
    long producedCount;
    DeadlockDetector *detector;
} Producer;

Producer* producer_create(const char *id, Buffer *buffer, int intervalMs);
void producer_destroy(Producer *producer);
void* producer_run(void *arg);

#endif
