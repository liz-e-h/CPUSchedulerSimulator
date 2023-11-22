# Round Robin CPU Scheduler Simulator

## About The Project

This project implements a round-robin CPU scheduling simulator. The input to the
simulator is a set of processes and a time slice. Each process is described by an id, arrival time, and CPU burst. The simulator simulates RR scheduling on these processes and for each process it calculate its start time and finish time. The simulator also computes a condensed execution sequence of all processes.

## Getting Started

To compile all code, type:

```
$ make
```

To run the resulting code on file test.txt with quantum=2 and max. execution sequence length of 20:

```
$ ./scheduler 2 20 < test.txt
```
