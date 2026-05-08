#define _GNU_SOURCE
#include "../include/consumer.h"
#include "../include/logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

extern volatile int running;
extern Logger *global_logger;

Consumer *consumer_create(const char *id, Buffer *input, Buffer *output, int durationMs)
{
  Consumer *consumer = (Consumer *)malloc(sizeof(Consumer));
  strncpy(consumer->id, id, 15);
  consumer->id[15] = '\0';
  consumer->inputBuffer = input;
  consumer->outputBuffer = output;
  consumer->durationMs = durationMs;
  consumer->consumedCount = 0;
  consumer->detector = NULL;
  return consumer;
}

void consumer_destroy(Consumer *consumer)
{
  if (consumer)
  {
    free(consumer);
  }
}

void *consumer_run(void *arg)
{
  Consumer *consumer = (Consumer *)arg;
  char logMsg[256];

  while (running)
  {
    int value = 0;
    if (consumer->detector)
      deadlock_detector_set_waiting(consumer->detector, consumer->id, "input buffer full");
    if (buffer_remove(consumer->inputBuffer, &value) != 0)
    {
      if (consumer->detector)
        deadlock_detector_set_ready(consumer->detector, consumer->id);
      break;
    }
    if (consumer->detector)
      deadlock_detector_set_ready(consumer->detector, consumer->id);
    consumer->consumedCount++;

    snprintf(logMsg, sizeof(logMsg), "[CONSUMER %s] consumed %d <- Buffer %c",
             consumer->id, value, consumer->inputBuffer->name);
    if (global_logger)
      log_message(global_logger, logMsg);
    printf("%s\n", logMsg);

    usleep(consumer->durationMs * 1000);

    if (consumer->outputBuffer != NULL)
    {
      int newValue = value + 1;
      if (consumer->detector)
        deadlock_detector_set_waiting(consumer->detector, consumer->id, "output buffer empty");
      if (buffer_insert(consumer->outputBuffer, newValue) != 0)
      {
        if (consumer->detector)
          deadlock_detector_set_ready(consumer->detector, consumer->id);
        break;
      }
      if (consumer->detector)
        deadlock_detector_set_ready(consumer->detector, consumer->id);

      snprintf(logMsg, sizeof(logMsg), "[CONSUMER %s] produced %d -> Buffer %c",
               consumer->id, newValue, consumer->outputBuffer->name);
      if (global_logger)
        log_message(global_logger, logMsg);
      printf("%s\n", logMsg);
    }
  }
  return NULL;
}
