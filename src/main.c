#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include "buffer.h"
#include "producer.h"
#include "consumer.h"
#include "deadlock_detector.h"
#include "logger.h"
#include "metrics.h"
#include "config_parser.h"

volatile int running = 1;
Logger *global_logger = NULL;
Metrics *global_metrics = NULL;
DeadlockDetector *global_detector = NULL;

void signal_handler(int sig __attribute__((unused)))
{
  running = 0;
  printf("\n\nShutting down...\n");
}

void *deadlock_monitor_run(void *arg __attribute__((unused)))
{
  while (running)
  {
    sleep(1);
    if (global_detector)
    {
      deadlock_detector_check(global_detector);
    }
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Usage: %s <config_file>\n", argv[0]);
    printf("Example: %s configs/experiment1.conf\n", argv[0]);
    return 1;
  }

  srand(time(NULL));

  // Set up signal handler
  signal(SIGINT, signal_handler);

  // Initialize logger
  global_logger = logger_create("simulation.log");
  global_metrics = metrics_create();
  global_detector = deadlock_detector_create();

  // Parse configuration
  Configuration *config = config_parse(argv[1]);
  if (!config)
  {
    printf("Failed to parse configuration file\n");
    return 1;
  }

  config_print(config);

  // Register threads in detector
  for (int i = 0; i < config->producerCount; i++)
  {
    deadlock_detector_register_thread(global_detector, config->producers[i]->id);
    config->producers[i]->detector = global_detector;
  }
  for (int i = 0; i < config->consumerCount; i++)
  {
    deadlock_detector_register_thread(global_detector, config->consumers[i]->id);
    config->consumers[i]->detector = global_detector;
  }

  // Create deadlock monitor thread
  pthread_t monitorThread;
  pthread_create(&monitorThread, NULL, deadlock_monitor_run, NULL);

  // Start producer threads
  for (int i = 0; i < config->producerCount; i++)
  {
    pthread_create(&config->producers[i]->thread, NULL,
                   producer_run, config->producers[i]);
  }

  // Start consumer threads
  for (int i = 0; i < config->consumerCount; i++)
  {
    pthread_create(&config->consumers[i]->thread, NULL,
                   consumer_run, config->consumers[i]);
  }

  printf("Simulation running for %d seconds...\n\n", config->simulationDuration);

  // Run simulation
  sleep(config->simulationDuration);

  // Stop simulation
  running = 0;

  printf("\nWaiting for threads to finish...\n");

  // Join all threads
  for (int i = 0; i < config->producerCount; i++)
  {
    pthread_join(config->producers[i]->thread, NULL);
  }
  for (int i = 0; i < config->consumerCount; i++)
  {
    pthread_join(config->consumers[i]->thread, NULL);
  }
  pthread_join(monitorThread, NULL);

  // Record end time and print metrics
  global_metrics->endTime = time(NULL);

  printf("\n\n============ SIMULATION RESULTS ============\n\n");

  // Print producer stats
  printf("PRODUCERS:\n");
  for (int i = 0; i < config->producerCount; i++)
  {
    printf("  %s: produced %ld items\n",
           config->producers[i]->id,
           config->producers[i]->producedCount);
  }

  printf("\nCONSUMERS:\n");
  for (int i = 0; i < config->consumerCount; i++)
  {
    printf("  %s: consumed %ld items\n",
           config->consumers[i]->id,
           config->consumers[i]->consumedCount);
  }

  printf("\nBUFFERS:\n");
  for (int i = 0; i < config->bufferCount; i++)
  {
    buffer_print_stats(config->buffers[i]);
  }

  metrics_print_report(global_metrics);

  printf("Deadlock Detections: %d\n", deadlock_detector_get_count(global_detector));
  printf("Log file: simulation.log\n");
  printf("==========================================\n");

  // Cleanup
  config_cleanup(config);
  logger_destroy(global_logger);
  metrics_destroy(global_metrics);
  deadlock_detector_destroy(global_detector);

  return 0;
}
