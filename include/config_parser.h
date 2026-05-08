#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include "buffer.h"
#include "producer.h"
#include "consumer.h"

#define MAX_BUFFERS 10
#define MAX_PRODUCERS 10
#define MAX_CONSUMERS 10

typedef struct {
    Buffer *buffers[MAX_BUFFERS];
    int bufferCount;
    
    Producer *producers[MAX_PRODUCERS];
    int producerCount;
    
    Consumer *consumers[MAX_CONSUMERS];
    int consumerCount;
    
    int simulationDuration;
} Configuration;

Configuration* config_parse(const char *filename);
void config_cleanup(Configuration *config);
void config_print(Configuration *config);

#endif
