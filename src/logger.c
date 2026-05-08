#include "../include/logger.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

Logger *logger_create(const char *filename)
{
  Logger *logger = (Logger *)malloc(sizeof(Logger));
  pthread_mutex_init(&logger->mutex, NULL);
  logger->messageCount = 0;

  if (filename)
  {
    logger->logFile = fopen(filename, "w");
  }
  else
  {
    logger->logFile = NULL;
  }

  return logger;
}

void logger_destroy(Logger *logger)
{
  if (logger)
  {
    if (logger->logFile)
    {
      fclose(logger->logFile);
    }
    pthread_mutex_destroy(&logger->mutex);
    free(logger);
  }
}

void log_message(Logger *logger, const char *msg)
{
  if (!logger)
    return;

  pthread_mutex_lock(&logger->mutex);

  time_t now = time(NULL);
  char timestamp[64];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));

  if (logger->logFile)
  {
    fprintf(logger->logFile, "[%s] %s\n", timestamp, msg);
    fflush(logger->logFile);
  }

  logger->messageCount++;
  pthread_mutex_unlock(&logger->mutex);
}

void log_formatted(Logger *logger, const char *format, ...)
{
  if (!logger)
    return;

  char buffer[512];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  log_message(logger, buffer);
}
