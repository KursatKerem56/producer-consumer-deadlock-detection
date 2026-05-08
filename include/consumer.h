#ifndef CONSUMER_H
#define CONSUMER_H

#include "buffer.h"
#include "deadlock_detector.h"
#include <pthread.h>

typedef struct {
    char id[16];
    Buffer *inputBuffer;
    Buffer *outputBuffer;
    int durationMs;
    pthread_t thread;
    long consumedCount;
    DeadlockDetector *detector;
} Consumer;

Consumer* consumer_create(const char *id, Buffer *input, Buffer *output, int durationMs);
void consumer_destroy(Consumer *consumer);
void* consumer_run(void *arg);

#endif
