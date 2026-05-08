#define _GNU_SOURCE
#include "../include/producer.h"
#include "../include/logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

extern volatile int running;
extern Logger *global_logger;

Producer *producer_create(const char *id, Buffer *buffer, int intervalMs)
{
  Producer *producer = (Producer *)malloc(sizeof(Producer));
  strncpy(producer->id, id, 15);
  producer->id[15] = '\0';
  producer->targetBuffer = buffer;
  producer->intervalMs = intervalMs;
  producer->producedCount = 0;
  producer->detector = NULL;
  return producer;
}

void producer_destroy(Producer *producer)
{
  if (producer)
  {
    free(producer);
  }
}

void *producer_run(void *arg)
{
  Producer *producer = (Producer *)arg;
  char logMsg[256];

  while (running)
  {
    int value = rand() % 1000;
    if (producer->detector)
      deadlock_detector_set_waiting(producer->detector, producer->id, "buffer empty");
    if (buffer_insert(producer->targetBuffer, value) != 0)
    {
      if (producer->detector)
        deadlock_detector_set_ready(producer->detector, producer->id);
      break;
    }
    if (producer->detector)
      deadlock_detector_set_ready(producer->detector, producer->id);
    producer->producedCount++;

    snprintf(logMsg, sizeof(logMsg), "[PRODUCER %s] produced %d -> Buffer %c",
             producer->id, value, producer->targetBuffer->name);
    if (global_logger)
      log_message(global_logger, logMsg);
    printf("%s\n", logMsg);

    usleep(producer->intervalMs * 1000);
  }
  return NULL;
}
