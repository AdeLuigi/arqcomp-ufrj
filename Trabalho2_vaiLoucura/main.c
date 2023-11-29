/*
Name: Lucas Tatsuya Tanaka
DRE:1180058149

Name: Ademario Vitor Costa de Santana
DRE:118143148

Name: Felipe de Jesus Araujo da Conceição
DRE:119180575
*/

#include <stdio.h>

#include <stdlib.h>

#include <time.h>

#include <string.h>

#include <assert.h>

#include "queue.h"

#include "variables.h"

#include "process.h"

/* Process Queue */
QUEUE queue_high_priot;
QUEUE queue_low_priot;
QUEUE queue_io[3];

/* Calls all initialization functions */
void init() {
  initialize_io();
  initialize_process();
  initialize_queue();
}

int main() {
  srand(time(NULL));
  init();
  print_val();

  FILE * file_execucao = fopen("log_execucao.txt", "w");

  int scal_proc = 0; /* Number of processes that have already been scheduled */
  int time_unit = 0; /* Counter variable that informs which time unit is being executed */
  int exec_pid = -1; /* PID of the process that is currently running */
  int same_proc_count = 0; /* Counter variable that informs how long the same process has been running */
  int i;

  /* Main loop */
  while (!finished()) {
    fprintf(file_execucao, "Time %d: ", time_unit);
    if (exec_pid == -1)
      fprintf(file_execucao, "0 processes running\n\n");
    else
      fprintf(file_execucao, "PID = %d running\n\n", exec_pid);

    /* Counts the execution time of the current process and checks if it has finished its execution */
    if (exec_pid != -1) {
      int idx = pid_to_idx(exec_pid);
      same_proc_count++;
      list_proc[idx].time_srvc_count++;
      if (list_proc[idx].time_srvc_count == list_proc[idx].time_srvc) {
        fprintf(file_execucao, "- PID = %d finished its execution\n", exec_pid);
        list_proc[idx].time_end = time_unit;
        list_proc[idx].status = EXIT;
        exec_pid = -1;
      }
    }

    /* Puts in the queue the processes that start at this time unit */
    while (scal_proc < NUM_PROC && list_proc[scal_proc].time_start == time_unit) {
      list_proc[scal_proc].status = NEW;
      fprintf(file_execucao, "- PID = %d CREATED - high priority\n", list_proc[scal_proc].pid);
      list_proc[scal_proc].status = READY;
      queue_push( & queue_high_priot, list_proc[scal_proc].pid);
      scal_proc++;
    }

    /* Checks processes that returned from IO and puts them in the queue */
    for (i = 0; i < NUM_IO; i++) {
      /* pid of the first process in the queue of the i-th IO */
      int pid_ret = queue_front( & queue_io[i]);

      /* If there is a process, check if it will return from IO now */
      if (pid_ret != -1) {
        int idx = pid_to_idx(pid_ret);
        if (list_proc[idx].io_return_time == time_unit) {
          queue_pop( & queue_io[i]);
          IO curr_io = * list_proc[idx].io[list_proc[idx].curr_io_idx];
          if (curr_io.queue_priority == HIGH_PRIOT) {
            fprintf(file_execucao, "- PID = %d returned from an IO of type %s and was put in the high priority queue\n", list_proc[idx].pid, curr_io.type);
            queue_push( & queue_high_priot, list_proc[idx].pid);
          } else {
            fprintf(file_execucao, "- PID = %d returned from an IO of type %s and was put in the low priority queue\n", list_proc[idx].pid, curr_io.type);
            queue_push( & queue_low_priot, list_proc[idx].pid);
          }
          list_proc[idx].status = READY;
        }
      }
    }

    /* Checks IO interrupts of the current process */
    if (exec_pid != -1) {
      int idx = pid_to_idx(exec_pid);
      for (i = 0; i < list_proc[idx].num_io; i++) {
        /* Checks if the IO interrupt occurs at this time unit */
        if (list_proc[idx].time_io[i] == list_proc[idx].time_srvc_count) {
          /* Puts the PID of the process in the queue of the corresponding IO */
          if (strcmp(disk.type, list_proc[idx].io[i] -> type) == 0)
            queue_push( & queue_io[DISK], exec_pid);
          else if (strcmp(printer.type, list_proc[idx].io[i] -> type) == 0)
            queue_push( & queue_io[PRINTER], exec_pid);
          else if (strcmp(mag_tape.type, list_proc[idx].io[i] -> type) == 0)
            queue_push( & queue_io[MAG_TAPE], exec_pid);

          /* Blocks the process, sets the time when it will return and which IO it is doing */
          fprintf(file_execucao, "- PID = %d suffered an IO interrupt of type %s and was blocked\n", exec_pid, list_proc[idx].io[i] -> type);
          list_proc[idx].curr_io_idx = i;
          list_proc[idx].io_return_time = time_unit + list_proc[idx].io[i] -> time_exec;
          list_proc[idx].status = BLOCKED;
          exec_pid = -1;
          break;
        }
      }
    }

    /* Preemption to not exceed the time slice */
    if (exec_pid != -1 && same_proc_count == TIMESLICE) {
      fprintf(file_execucao, "- PID = %d executed the maximum number of consecutive time units (%d) and was put in the low priority queue\n", exec_pid, TIMESLICE);
      queue_push( & queue_low_priot, exec_pid);
      exec_pid = -1;
    }

    /* Puts a process in execution, if possible */
    if (exec_pid == -1) {
      same_proc_count = 0;
      /* Search for a process in the high priority queue */
      exec_pid = queue_pop( & queue_high_priot);
      if (exec_pid != -1) {
        fprintf(file_execucao, "- PID = %d left the high priority queue and was put in execution\n", exec_pid);
        list_proc[pid_to_idx(exec_pid)].status = RUNNING;
      }
      /* If the high priority queue is empty, search for a process in the low priority queue */
      else {
        exec_pid = queue_pop( & queue_low_priot);
        if (exec_pid != -1) {
          fprintf(file_execucao, "- PID = %d left the low priority queue and was put in execution\n", exec_pid);
          list_proc[pid_to_idx(exec_pid)].status = RUNNING;
        }
      }
    }

    /* Prints the queues */
    fprintf(file_execucao, "High-priority queue: ");
    print_queue( & queue_high_priot, file_execucao);
    fprintf(file_execucao, "Low-priority queue: ");
    print_queue( & queue_low_priot, file_execucao);
    fprintf(file_execucao, "I/O queue for Disk: ");
    print_queue( & queue_io[DISK], file_execucao);
    fprintf(file_execucao, "I/O queue for Printer: ");
    print_queue( & queue_io[PRINTER], file_execucao);
    fprintf(file_execucao, "I/O queue for mag_tape: ");
    print_queue( & queue_io[MAG_TAPE], file_execucao);

    fprintf(file_execucao, "\n**************************************\n");

    time_unit++;
  }

  FILE * file_turnarounds = fopen("log_turnarounds.txt", "w");
  fprintf(file_turnarounds, "Turnarounds:\n");
  for (i = 0; i < NUM_PROC; i++) {
    fprintf(file_turnarounds, "Process %d: %d\n", i + 100, list_proc[i].time_end - list_proc[i].time_start);
  }

  fclose(file_execucao);
  fclose(file_turnarounds);

  printf("End Simulation\n");

  return 0;
}