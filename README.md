# Operating Systems Project - Producer Consumer with Deadlock Detection

## Overview

This project implements a multi-threaded producer-consumer system with deadlock detection in C. It demonstrates synchronization mechanisms (mutexes and semaphores) and includes configurable experiments with metrics collection.

## Features

- **Producer-Consumer Pattern**: Multiple producers and consumers working with circular buffers
- **Thread Synchronization**: Mutexes and semaphores for safe buffer access
- **Deadlock Detection**: Simple circular wait detection algorithm
- **Configurable Experiments**: Parse config files to set up different scenarios
- **Metrics Collection**: Track throughput, wait times, and deadlock occurrences
- **Thread-Safe Logging**: Synchronized logging to file and console

## Build

```bash
make
```

## Run

Run experiment 1 (default):

```bash
make run
```

Or run a specific experiment:

```bash
make run1  # Experiment 1: Simple producer-consumer
make run2  # Experiment 2: Pipeline architecture
make run3  # Experiment 3: Deadlock scenario
```

Run with custom config:

```bash
./app configs/experiment1.conf
```

## Project Structure

```
OS-HW/
├── include/          # Header files
│   ├── buffer.h
│   ├── producer.h
│   ├── consumer.h
│   ├── deadlock_detector.h
│   ├── logger.h
│   ├── metrics.h
│   └── config_parser.h
├── src/              # Implementation files
│   ├── buffer.c
│   ├── producer.c
│   ├── consumer.c
│   ├── deadlock_detector.c
│   ├── logger.c
│   ├── metrics.c
│   ├── config_parser.c
│   └── main.c
├── configs/          # Configuration files
│   ├── experiment1.conf
│   ├── experiment2.conf
│   └── experiment3.conf
├── Makefile
└── README.md
```

## Configuration Format

Config files support the following syntax:

- **Buffer**: `A[10]` - Create buffer A with size 10
- **Producer**: `P1>A` - Producer P1 feeds buffer A
- **Consumer**: `A>C1` - Consumer C1 consumes from buffer A
- **Pipeline**: `A>C1>B` - Consumer C1 consumes from A, produces to B
- **Timing**: `P1:10` - Producer P1 interval 10ms; `C1:20` - Consumer C1 duration 20ms
- **Duration**: `t:30` - Run simulation for 30 seconds

Example:

```
A[10]
B[5]
P1>A
P2>A
A>C1>B
B>C2
P1:15
P2:20
C1:10
C2:15
t:30
```

## Experiments

### Experiment 1: Simple Producer-Consumer

- Single buffer with 2 producers and 2 consumers
- Expected: Smooth operation, no deadlock

### Experiment 2: Pipeline Architecture

- Multiple buffers in sequence (A → B → C)
- Demonstrates data processing pipeline
- Expected: Steady throughput, no deadlock

### Experiment 3: Deadlock Scenario

- Thread A locks Buffer A then Buffer B
- Thread B locks Buffer B then Buffer A
- Expected: Circular wait detected

## Output

The program outputs:

1. **Console**: Real-time logs of producer/consumer actions
2. **simulation.log**: Timestamped log file with all events
3. **Statistics**: Final report with:
   - Items produced/consumed per thread
   - Buffer statistics
   - Throughput (items/sec)
   - Waiting and blocking times
   - Deadlock detections

## Synchronization Mechanisms

### Circular Buffer

- `mutex`: Protects shared buffer state
- `empty`: Semaphore counts empty slots
- `full`: Semaphore counts filled slots

### Deadlock Detection

- Each thread registers its state
- Monitor checks for threads waiting > 5 seconds
- Detects circular wait patterns
- Logs when deadlock is suspected

## Key Implementation Details

1. **Thread Safety**:
   - All shared resources protected by mutexes
   - Semaphores enforce buffer capacity constraints
   - Logger uses mutex for atomic output

2. **Producer Logic**:
   - Generate random values (0-999)
   - Wait for empty slot (sem_wait on empty)
   - Insert value into circular buffer
   - Signal full slot (sem_post on full)
   - Sleep for specified interval

3. **Consumer Logic**:
   - Wait for full slot (sem_wait on full)
   - Remove value from buffer
   - Signal empty slot (sem_post on empty)
   - Process value (sleep for duration)
   - Optionally produce to next buffer

4. **Deadlock Detection**:
   - Tracks each thread's waiting state
   - Monitors wait duration
   - Reports when multiple threads wait > 5 seconds
   - Identifies which resources are being waited for

## Performance Tips

- Adjust buffer sizes in config for throughput testing
- Modify intervals/durations to create contention
- Use smaller simulation times for quick testing
- Check simulation.log for detailed event traces

## Common Issues and Solutions

- **Deadlock on startup**: Check buffer sizes, ensure producers can insert
- **No deadlock detected**: Wait times may be too short, increase simulation time
- **High context switching**: Use larger intervals to reduce thread switching

## Clean Up

```bash
make clean
```

Removes compiled binary and log files.

## Compilation Notes

- Requires POSIX threads (pthread)
- Tested on Linux with GCC
- Windows: Use MinGW or WSL

## Author Notes

This implementation focuses on:

- Clean modular architecture
- Proper synchronization practices
- Practical deadlock detection
- Educational clarity
