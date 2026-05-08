#include "../include/deadlock_detector.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

DeadlockDetector *deadlock_detector_create(void)
{
  DeadlockDetector *detector = (DeadlockDetector *)malloc(sizeof(DeadlockDetector));
  detector->threadCount = 0;
  detector->deadlockCount = 0;
  pthread_mutex_init(&detector->stateMutex, NULL);
  return detector;
}

void deadlock_detector_destroy(DeadlockDetector *detector)
{
  if (detector)
  {
    pthread_mutex_destroy(&detector->stateMutex);
    free(detector);
  }
}

void deadlock_detector_register_thread(DeadlockDetector *detector, const char *threadId)
{
  pthread_mutex_lock(&detector->stateMutex);
  if (detector->threadCount < MAX_THREADS)
  {
    strncpy(detector->threads[detector->threadCount].threadId, threadId, 15);
    detector->threads[detector->threadCount].threadId[15] = '\0';
    detector->threads[detector->threadCount].waiting = false;
    detector->threads[detector->threadCount].waitStart = time(NULL);
    detector->threadCount++;
  }
  pthread_mutex_unlock(&detector->stateMutex);
}

void deadlock_detector_set_waiting(DeadlockDetector *detector, const char *threadId, const char *waitingFor)
{
  pthread_mutex_lock(&detector->stateMutex);
  for (int i = 0; i < detector->threadCount; i++)
  {
    if (strcmp(detector->threads[i].threadId, threadId) == 0)
    {
      detector->threads[i].waiting = true;
      strncpy(detector->threads[i].waitingFor, waitingFor, 15);
      detector->threads[i].waitingFor[15] = '\0';
      detector->threads[i].waitStart = time(NULL);
      break;
    }
  }
  pthread_mutex_unlock(&detector->stateMutex);
}

void deadlock_detector_set_ready(DeadlockDetector *detector, const char *threadId)
{
  pthread_mutex_lock(&detector->stateMutex);
  for (int i = 0; i < detector->threadCount; i++)
  {
    if (strcmp(detector->threads[i].threadId, threadId) == 0)
    {
      detector->threads[i].waiting = false;
      break;
    }
  }
  pthread_mutex_unlock(&detector->stateMutex);
}

void deadlock_detector_check(DeadlockDetector *detector)
{
  pthread_mutex_lock(&detector->stateMutex);

  // Count waiting threads
  int waitingCount = 0;
  for (int i = 0; i < detector->threadCount; i++)
  {
    if (detector->threads[i].waiting)
    {
      time_t now = time(NULL);
      if (difftime(now, detector->threads[i].waitStart) > 5)
      {
        waitingCount++;
      }
    }
  }

  // Simple circular wait detection
  if (waitingCount > 1)
  {
    printf("[DEADLOCK DETECTED] Circular wait suspected. Threads waiting: %d\n", waitingCount);
    detector->deadlockCount++;

    for (int i = 0; i < detector->threadCount; i++)
    {
      if (detector->threads[i].waiting)
      {
        printf("  %s waiting for %s\n", detector->threads[i].threadId,
               detector->threads[i].waitingFor);
      }
    }
  }

  pthread_mutex_unlock(&detector->stateMutex);
}

int deadlock_detector_get_count(DeadlockDetector *detector)
{
  pthread_mutex_lock(&detector->stateMutex);
  int count = detector->deadlockCount;
  pthread_mutex_unlock(&detector->stateMutex);
  return count;
}
