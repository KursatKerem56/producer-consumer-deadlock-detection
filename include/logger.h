#ifndef LOGGER_H
#define LOGGER_H

#include <pthread.h>
#include <stdio.h>

typedef struct {
    pthread_mutex_t mutex;
    FILE *logFile;
    int messageCount;
} Logger;

Logger* logger_create(const char *filename);
void logger_destroy(Logger *logger);
void log_message(Logger *logger, const char *msg);
void log_formatted(Logger *logger, const char *format, ...);

#endif
