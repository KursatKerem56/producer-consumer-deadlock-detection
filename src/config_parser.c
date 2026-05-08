#include "../include/config_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

Configuration *config_parse(const char *filename)
{
  Configuration *config = (Configuration *)malloc(sizeof(Configuration));
  config->bufferCount = 0;
  config->producerCount = 0;
  config->consumerCount = 0;
  config->simulationDuration = 30;

  FILE *file = fopen(filename, "r");
  if (!file)
  {
    printf("Error: Cannot open config file %s\n", filename);
    free(config);
    return NULL;
  }

  char line[256];
  Buffer *bufferMap[26] = {0}; // Map A-Z to buffers

  while (fgets(line, sizeof(line), file))
  {
    // Remove newline
    line[strcspn(line, "\n")] = 0;

    // Skip empty lines and comments
    if (line[0] == '\0' || line[0] == '#')
      continue;

    // Parse buffer definition: A[10] B[5]
    if (isalpha(line[0]) && line[1] == '[')
    {
      char bufName = line[0];
      int bufSize = atoi(&line[2]);
      if (bufSize > 0 && config->bufferCount < MAX_BUFFERS)
      {
        config->buffers[config->bufferCount] = buffer_create(bufSize, bufName);
        bufferMap[bufName - 'A'] = config->buffers[config->bufferCount];
        config->bufferCount++;
      }
    }
    // Parse producer: P1>A P2>A P3>B
    else if (line[0] == 'P' && isdigit(line[1]) && line[2] == '>')
    {
      char producerId[16];
      char targetBuf = line[3];
      int interval = 10;

      // Check for interval specification: P1:10
      char *colonPos = strchr(line, ':');
      if (colonPos)
      {
        sscanf(colonPos + 1, "%d", &interval);
      }

      snprintf(producerId, sizeof(producerId), "P%c", line[1]);

      if (bufferMap[targetBuf - 'A'] && config->producerCount < MAX_PRODUCERS)
      {
        config->producers[config->producerCount] =
            producer_create(producerId, bufferMap[targetBuf - 'A'], interval);
        config->producerCount++;
      }
    }
    // Parse consumer: A>C1 B>C3>A
    else if (isalpha(line[0]) && line[1] == '>')
    {
      char inputBuf = line[0];
      char consumerId[16];
      char outputBuf = '\0';
      int duration = 10;
      int pos = 2;

      // Read consumer ID
      while (pos < (int)strlen(line) && line[pos] != '>' && line[pos] != ':')
      {
        pos++;
      }
      strncpy(consumerId, &line[2], pos - 2);
      consumerId[pos - 2] = '\0';

      // Check for output buffer: B>C3>A
      if (pos < (int)strlen(line) && line[pos] == '>')
      {
        outputBuf = line[pos + 1];
        pos += 2;
      }

      // Check for duration: C1:10
      if (pos < (int)strlen(line) && line[pos] == ':')
      {
        sscanf(&line[pos + 1], "%d", &duration);
      }

      if (bufferMap[inputBuf - 'A'] && config->consumerCount < MAX_CONSUMERS)
      {
        Buffer *outBuf = (outputBuf != '\0') ? bufferMap[outputBuf - 'A'] : NULL;
        config->consumers[config->consumerCount] =
            consumer_create(consumerId, bufferMap[inputBuf - 'A'], outBuf, duration);
        config->consumerCount++;
      }
    }
    // Parse simulation duration: t:20
    else if (line[0] == 't' && line[1] == ':')
    {
      sscanf(&line[2], "%d", &config->simulationDuration);
    }
  }

  fclose(file);
  return config;
}

void config_cleanup(Configuration *config)
{
  if (config)
  {
    for (int i = 0; i < config->bufferCount; i++)
    {
      buffer_destroy(config->buffers[i]);
    }
    for (int i = 0; i < config->producerCount; i++)
    {
      producer_destroy(config->producers[i]);
    }
    for (int i = 0; i < config->consumerCount; i++)
    {
      consumer_destroy(config->consumers[i]);
    }
    free(config);
  }
}

void config_print(Configuration *config)
{
  if (!config)
    return;

  printf("=== Configuration ===\n");
  printf("Buffers: %d\n", config->bufferCount);
  for (int i = 0; i < config->bufferCount; i++)
  {
    printf("  %c[%d]\n", config->buffers[i]->name, config->buffers[i]->size);
  }
  printf("Producers: %d\n", config->producerCount);
  for (int i = 0; i < config->producerCount; i++)
  {
    printf("  %s -> %c (interval: %d ms)\n", config->producers[i]->id,
           config->producers[i]->targetBuffer->name, config->producers[i]->intervalMs);
  }
  printf("Consumers: %d\n", config->consumerCount);
  for (int i = 0; i < config->consumerCount; i++)
  {
    printf("  %s: %c -> %s (duration: %d ms)\n", config->consumers[i]->id,
           config->consumers[i]->inputBuffer->name,
           config->consumers[i]->outputBuffer ? (char[]){config->consumers[i]->outputBuffer->name, '\0'} : "NULL",
           config->consumers[i]->durationMs);
  }
  printf("Simulation Duration: %d seconds\n", config->simulationDuration);
  printf("====================\n\n");
}
