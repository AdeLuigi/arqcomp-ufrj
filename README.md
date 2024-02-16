# C Code Overview for Computer Architecture Course at UFRJ

## Introduction

This C code is part of a project for the Computer Architecture course at the Federal University of Rio de Janeiro (UFRJ) during the 2023.2 semester. It simulates a basic operating system's process scheduling and I/O handling mechanism, providing a practical understanding of how processes are managed and executed in a computer system.

## Features

- **Process Scheduling**: The simulation includes both high and low priority process queues to manage the execution order of processes based on their priority levels.
- **Time Slicing**: Implements a time-slicing mechanism to prevent a single process from monopolizing the CPU, ensuring fair CPU time distribution among all processes.
- **I/O Handling**: Manages I/O requests through separate queues for different I/O devices, such as disks, printers, and magnetic tapes, simulating how an operating system handles I/O operations.
- **Process States**: Tracks the state of each process (e.g., New, Ready, Running, Blocked, Exit) to simulate the lifecycle of processes in an operating system.
- **Initialization and Execution Flow**: Includes initialization functions to set up the simulation environment and a main loop to simulate the execution of processes over time units.

## Code Structure

- **Initialization**: The `init()` function initializes I/O devices, processes, and queues necessary for the simulation.
- **Main Loop**: The core simulation loop in the `main()` function iterates over time units, managing process scheduling, execution, I/O operations, and state transitions.
- **Process and I/O Queues**: Uses separate queues for high and low priority processes, as well as individual I/O queues for different types of I/O devices.
- **Process Execution and Preemption**: Simulates process execution and preempts processes that exceed their time slice, moving them to a lower priority queue.
- **I/O Handling**: Manages I/O interrupts and simulates process blocking and unblocking based on I/O operations.
- **Termination**: Checks for the completion of all processes and terminates the simulation, printing out the turnaround times for each process.

## Usage

To use this code, you need a C compiler like `gcc` or `clang`. Compile the code with all its dependencies (`queue.h`, `variables.h`, `process.h`) and run the resulting executable. The simulation will output logs of its execution, showing how processes are scheduled, executed, and how they interact with I/O devices.

## Dependencies

This code relies on the following header files, which must be included in the project directory:

- `queue.h`: Defines the structure and functions for queue management.
- `variables.h`: Contains global variables and definitions used throughout the simulation.
- `process.h`: Defines the structure and functions related to process management.

## Conclusion

This simulation provides a simplified but insightful view into the process scheduling and I/O handling mechanisms of an operating system. It is an educational tool for students in the Computer Architecture course at UFRJ to understand the fundamental concepts of operating systems in a practical, hands-on manner.

## Note

Please ensure all dependencies are correctly set up and that you understand the basic concepts of process management and scheduling to fully grasp the simulation's workings.