#ifndef BUFFER_H
#define BUFFER_H

#include <semaphore.h>
#include <pthread.h>

typedef struct {
    int *data;
    int size;
    int in;
    int out;
    int count;
    char name;
    pthread_mutex_t mutex;
    sem_t empty;
    sem_t full;
} Buffer;

Buffer* buffer_create(int size, char name);
void buffer_destroy(Buffer *buffer);
int buffer_insert(Buffer *buffer, int value);
int buffer_remove(Buffer *buffer, int *value);
void buffer_print_stats(Buffer *buffer);

#endif
