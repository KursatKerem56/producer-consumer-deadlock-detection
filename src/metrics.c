#include "../include/metrics.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

Metrics *metrics_create(void)
{
  Metrics *metrics = (Metrics *)malloc(sizeof(Metrics));
  metrics->totalProcessed = 0;
  metrics->totalWaitingTime = 0.0;
  metrics->totalBlockingTime = 0.0;
  metrics->deadlockCount = 0;
  metrics->startTime = time(NULL);
  metrics->endTime = 0;
  pthread_mutex_init(&metrics->mutex, NULL);
  return metrics;
}

void metrics_destroy(Metrics *metrics)
{
  if (metrics)
  {
    pthread_mutex_destroy(&metrics->mutex);
    free(metrics);
  }
}

void metrics_add_processed(Metrics *metrics)
{
  if (!metrics)
    return;
  pthread_mutex_lock(&metrics->mutex);
  metrics->totalProcessed++;
  pthread_mutex_unlock(&metrics->mutex);
}

void metrics_add_wait_time(Metrics *metrics, double waitTime)
{
  if (!metrics)
    return;
  pthread_mutex_lock(&metrics->mutex);
  metrics->totalWaitingTime += waitTime;
  pthread_mutex_unlock(&metrics->mutex);
}

void metrics_add_block_time(Metrics *metrics, double blockTime)
{
  if (!metrics)
    return;
  pthread_mutex_lock(&metrics->mutex);
  metrics->totalBlockingTime += blockTime;
  pthread_mutex_unlock(&metrics->mutex);
}

void metrics_increment_deadlock(Metrics *metrics)
{
  if (!metrics)
    return;
  pthread_mutex_lock(&metrics->mutex);
  metrics->deadlockCount++;
  pthread_mutex_unlock(&metrics->mutex);
}

void metrics_print_report(Metrics *metrics)
{
  if (!metrics)
    return;

  pthread_mutex_lock(&metrics->mutex);

  double runtime = difftime(metrics->endTime, metrics->startTime);
  if (runtime == 0)
    runtime = 1;

  double throughput = metrics->totalProcessed / runtime;

  printf("\n============ METRICS REPORT ============\n");
  printf("Runtime: %.2f seconds\n", runtime);
  printf("Total Items Processed: %ld\n", metrics->totalProcessed);
  printf("Throughput: %.2f items/sec\n", throughput);
  printf("Total Waiting Time: %.2f ms\n", metrics->totalWaitingTime);
  printf("Total Blocking Time: %.2f ms\n", metrics->totalBlockingTime);
  printf("Deadlock Occurrences: %d\n", metrics->deadlockCount);
  printf("========================================\n\n");

  pthread_mutex_unlock(&metrics->mutex);
}
