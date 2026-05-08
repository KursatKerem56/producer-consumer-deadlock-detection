#ifndef METRICS_H
#define METRICS_H

#include <pthread.h>
#include <time.h>

typedef struct {
    long totalProcessed;
    double totalWaitingTime;
    double totalBlockingTime;
    int deadlockCount;
    time_t startTime;
    time_t endTime;
    pthread_mutex_t mutex;
} Metrics;

Metrics* metrics_create(void);
void metrics_destroy(Metrics *metrics);
void metrics_add_processed(Metrics *metrics);
void metrics_add_wait_time(Metrics *metrics, double waitTime);
void metrics_add_block_time(Metrics *metrics, double blockTime);
void metrics_increment_deadlock(Metrics *metrics);
void metrics_print_report(Metrics *metrics);

#endif
