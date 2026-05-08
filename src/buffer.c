#define _POSIX_C_SOURCE 200809L
#include "../include/buffer.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

extern volatile int running;

static void add_timeout(struct timespec *ts, long timeoutMs)
{
  ts->tv_sec += timeoutMs / 1000;
  ts->tv_nsec += (timeoutMs % 1000) * 1000000L;
  if (ts->tv_nsec >= 1000000000L)
  {
    ts->tv_sec++;
    ts->tv_nsec -= 1000000000L;
  }
}

static int wait_for_semaphore(sem_t *semaphore)
{
  while (running)
  {
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    add_timeout(&timeout, 100);

    if (sem_timedwait(semaphore, &timeout) == 0)
    {
      return 0;
    }

    if (errno != ETIMEDOUT && errno != EINTR)
    {
      return -1;
    }
  }

  return -1;
}

Buffer *buffer_create(int size, char name)
{
  Buffer *buffer = (Buffer *)malloc(sizeof(Buffer));
  buffer->data = (int *)malloc(sizeof(int) * size);
  buffer->size = size;
  buffer->in = 0;
  buffer->out = 0;
  buffer->count = 0;
  buffer->name = name;

  pthread_mutex_init(&buffer->mutex, NULL);
  sem_init(&buffer->empty, 0, size);
  sem_init(&buffer->full, 0, 0);

  return buffer;
}

void buffer_destroy(Buffer *buffer)
{
  if (buffer)
  {
    pthread_mutex_destroy(&buffer->mutex);
    sem_destroy(&buffer->empty);
    sem_destroy(&buffer->full);
    free(buffer->data);
    free(buffer);
  }
}

int buffer_insert(Buffer *buffer, int value)
{
  if (wait_for_semaphore(&buffer->empty) != 0)
  {
    return -1;
  }

  pthread_mutex_lock(&buffer->mutex);

  buffer->data[buffer->in] = value;
  buffer->in = (buffer->in + 1) % buffer->size;
  buffer->count++;

  pthread_mutex_unlock(&buffer->mutex);
  sem_post(&buffer->full);

  return 0;
}

int buffer_remove(Buffer *buffer, int *value)
{
  if (wait_for_semaphore(&buffer->full) != 0)
  {
    return -1;
  }

  pthread_mutex_lock(&buffer->mutex);

  *value = buffer->data[buffer->out];
  buffer->out = (buffer->out + 1) % buffer->size;
  buffer->count--;

  pthread_mutex_unlock(&buffer->mutex);
  sem_post(&buffer->empty);

  return 0;
}

void buffer_print_stats(Buffer *buffer)
{
  pthread_mutex_lock(&buffer->mutex);
  printf("Buffer %c: count=%d, size=%d\n", buffer->name, buffer->count, buffer->size);
  pthread_mutex_unlock(&buffer->mutex);
}
