# QUICK START GUIDE

## Build & Run (Windows with WSL)

### Build the project:

```bash
wsl bash -c "cd /mnt/c/'Users/Kürşat Kerem'/Desktop/code/OS-HW && make clean && make"
```

### Run Experiment 1 (Simple Producer-Consumer):

```bash
wsl bash -c "cd /mnt/c/'Users/Kürşat Kerem'/Desktop/code/OS-HW && timeout 35 ./app configs/experiment1.conf"
```

### Run Experiment 2 (Pipeline):

```bash
wsl bash -c "cd /mnt/c/'Users/Kürşat Kerem'/Desktop/code/OS-HW && timeout 35 ./app configs/experiment2.conf"
```

### Run Experiment 3 (Deadlock Scenario):

```bash
wsl bash -c "cd /mnt/c/'Users/Kürşat Kerem'/Desktop/code/OS-HW && timeout 35 ./app configs/experiment3.conf"
```

### View logs:

```bash
wsl bash -c "cd /mnt/c/'Users/Kürşat Kerem'/Desktop/code/OS-HW && tail -50 simulation.log"
```

## What's Implemented

### ✅ Core Features

- [x] Circular buffer with mutex and semaphores
- [x] Producer threads (multiple, configurable)
- [x] Consumer threads (multiple, configurable)
- [x] Consumer→Producer pipelines
- [x] Thread-safe logging with timestamps
- [x] Performance metrics collection
- [x] Deadlock detection (simplified circular wait detection)
- [x] Configuration parser (.conf files)
- [x] Signal handling for graceful shutdown

### ✅ Synchronization

- [x] Proper mutex usage
- [x] Correct semaphore patterns
- [x] No busy waiting
- [x] No race conditions
- [x] Proper resource cleanup

### ✅ Experiments

- [x] Experiment 1: Simple producer-consumer (no deadlock expected)
- [x] Experiment 2: Pipeline architecture (data flowing through stages)
- [x] Experiment 3: Deadlock scenario (circular dependency)

### ✅ Documentation

- [x] README.md - Full documentation
- [x] PROJECT_SUMMARY.md - Detailed implementation summary
- [x] This guide - Quick start instructions
- [x] Code comments - Clear explanations

## Configuration Format

Create custom .conf files with this syntax:

```conf
# Define buffers
A[10]           # Buffer A, size 10
B[5]            # Buffer B, size 5

# Define producers
P1>A            # Producer P1 → Buffer A
P2>B            # Producer P2 → Buffer B

# Define consumers
A>C1            # Buffer A → Consumer C1
B>C2>A          # Buffer B → C2 → Buffer A (pipeline)

# Set timing (in milliseconds)
P1:15           # Producer P1 interval 15ms
P2:20           # Producer P2 interval 20ms
C1:10           # Consumer C1 duration 10ms
C2:25           # Consumer C2 duration 25ms

# Simulation duration (in seconds)
t:30            # Run for 30 seconds
```

## Output Files

### simulation.log

Timestamped log of all producer/consumer operations:

```
[2026-05-08 17:13:18] [PRODUCER P1] produced 42 -> Buffer A
[2026-05-08 17:13:18] [CONSUMER C1] consumed 42 <- Buffer A
[2026-05-08 17:13:18] [CONSUMER C1] produced 43 -> Buffer B
```

### Console Output

Final statistics:

- Total items produced/consumed per thread
- Buffer status
- Throughput (items/sec)
- Deadlock detections
- Timing information

## Project Files

```
OS-HW/
├── src/                    # Source code (8 files)
├── include/                # Headers (7 files)
├── configs/                # Configuration files (3 experiments)
├── Makefile               # Build configuration
├── README.md              # Full documentation
├── PROJECT_SUMMARY.md     # Detailed summary
├── QUICKSTART.md          # This file
└── app                    # Compiled executable (ready to run)
```

## Common Commands

```bash
# Clean build
wsl bash -c "cd /mnt/c/'Users/Kürşat Kerem'/Desktop/code/OS-HW && make clean && make"

# Remove old logs
wsl bash -c "cd /mnt/c/'Users/Kürşat Kerem'/Desktop/code/OS-HW && rm -f simulation.log"

# Count log entries
wsl bash -c "cd /mnt/c/'Users/Kürşat Kerem'/Desktop/code/OS-HW && wc -l simulation.log"

# Search for deadlock detections
wsl bash -c "cd /mnt/c/'Users/Kürşat Kerem'/Desktop/code/OS-HW && grep -i deadlock simulation.log"
```

## What to Expect

### Experiment 1: Normal Operation

- Steady production and consumption
- No deadlocks
- Smooth throughput
- Buffer stays partially filled

### Experiment 2: Pipeline

- Data flowing through stages A→B→C
- Consumers processing sequentially
- Each stage adds value (value + 1)
- Normal completion

### Experiment 3: Deadlock

- Thread contention on multiple buffers
- Potential for circular wait detection
- Monitor thread reports suspicious patterns
- System handles gracefully

## Success Indicators ✅

- [x] Binary compiles without errors
- [x] Application runs without crashing
- [x] Producers generate values
- [x] Consumers process values
- [x] Logs are generated with timestamps
- [x] Final statistics display
- [x] No zombie processes
- [x] Clean resource cleanup

---

**Project Status: COMPLETE AND FUNCTIONAL** 🎉
