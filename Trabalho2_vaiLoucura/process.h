#ifndef process
#define process

#include <stdio.h>

#include <stdlib.h>

#include <time.h>

#include <string.h>

#include "variables.h"

/* IO types variables */
IO disk;
IO printer;
IO mag_tape;

/* Process list */
PROCESS * list_proc;

/* Function that initializes the IO variables */
void initialize_io() {
  strcpy(disk.type, "disk");
  disk.queue_priority = LOW_PRIOT;
  disk.time_exec = TIME_EXEC_DISK;

  strcpy(printer.type, "printer");
  printer.queue_priority = HIGH_PRIOT;
  printer.time_exec = TIME_EXEC_PRINTER;

  strcpy(mag_tape.type, "mag_tape");
  mag_tape.queue_priority = HIGH_PRIOT;
  mag_tape.time_exec = TIME_EXEC_MAG_TAPE;
}

/* Function that initializes the process list */
void initialize_process() {
  int i, j;
  list_proc = (PROCESS * ) malloc(NUM_PROC * sizeof(PROCESS));

  if (!list_proc) {
    printf("Error allocating memory\n");
    exit(1);
  }

  for (i = 0; i < NUM_PROC; i++) {
    if (i == 0) {
      list_proc[i].time_start = (int)(rand() % (1 + MAX_TMP_START));
    } else {
      list_proc[i].time_start = list_proc[i - 1].time_start + (int)(rand() % (1 + MAX_TMP_START));
    }
    list_proc[i].pid = i + 100;
    list_proc[i].ppid = 1;
    list_proc[i].priority = (int)(rand() % NUM_PRIORITY_PROC);
    list_proc[i].status = 0;
    list_proc[i].time_srvc_count = 0;
    list_proc[i].curr_io_idx = -1;
    list_proc[i].io_return_time = -1;

    int time_srvc_proc = (int)(rand() % (1 + MAX_TMP_SRVC_PROC));
    list_proc[i].time_srvc = time_srvc_proc < MIN_TMP_SRVC_PROC ? MIN_TMP_SRVC_PROC : time_srvc_proc;

    /* Selects the number of IO interrupts of the process */
    list_proc[i].num_io = (int)(rand() % (1 + MAX_NUM_IO_PROC));
    if (list_proc[i].num_io > list_proc[i].time_srvc - 2) {
      list_proc[i].num_io = list_proc[i].time_srvc - 2;
    }

    /* Allocates memory for the IO list of the process */
    list_proc[i].io = (IO ** ) malloc(list_proc[i].num_io * sizeof(IO * ));
    if (!list_proc[i].io) {
      printf("Error allocating memory - IO of the process: %d\n", i);
      exit(1);
    }

    /* Allocates memory for the list of start times of each IO of the process */
    list_proc[i].time_io = (int * ) malloc(list_proc[i].num_io * sizeof(int));
    if (!list_proc[i].time_io) {
      printf("Error allocating memory of IO start time of the process: %d\n", i);
      exit(1);
    }

    /* Selects the IO types for the process */
    for (j = 0; j < list_proc[i].num_io; j++) {
      switch (rand() % 3) {
      case 0:
        /* DISK */
        list_proc[i].io[j] = & disk;
        break;
      case 1:
        /* PRINTER */
        list_proc[i].io[j] = & printer;
        break;
      case 2:
        /* MAG_TAPE */
        list_proc[i].io[j] = & mag_tape;
        break;
      }

      /* Selects the IO start time */
      /* Ensures that all IOs of the process have different times */
      /* An IO cannot be the first or the last thing a process does */
      int time_io = -1;
      while (time_io == -1 || time_io == 0 || time_io == list_proc[i].time_srvc) {
        int k;
        time_io = rand() % list_proc[i].time_srvc;
        for (k = 0; k < j; k++) {
          if (list_proc[i].time_io[k] == time_io) {
            time_io = -1;
          }
        }
      }
      list_proc[i].time_io[j] = time_io;
    }
  }
}

/* Prints the initial values of the process information */
void print_val() {
  int i, j;

  FILE * file_val = fopen("log_init_val.txt", "w");

  printf("\nLogs de inicialização das variáveis iniciais:\n");

  for (i = 0; i < NUM_PROC; i++) {

    printf("Process PID: %d \n", list_proc[i].pid);
    printf("PPID: %d \n", list_proc[i].ppid);
    printf("Priority: %d \n", list_proc[i].priority);
    printf("Start Time: %d \n", list_proc[i].time_start);
    printf("Service Time: %d \n", list_proc[i].time_srvc);

    /*
    fprintf(file_val, "Process PID: %d \n", list_proc[i].pid);
    fprintf(file_val, "PPID: %d \n", list_proc[i].ppid);
    fprintf(file_val, "Priority: %d \n", list_proc[i].priority);
    fprintf(file_val, "Start Time: %d \n", list_proc[i].time_start);
    fprintf(file_val, "Service Time: %d \n", list_proc[i].time_srvc);
    */

    int tam_io = list_proc[i].num_io;
    for (j = 0; j < tam_io; j++) {

      /*
      fprintf(file_val, "IO Type: %s \n", list_proc[i].io[j] -> type);
      fprintf(file_val, "IO start time: %d \n", list_proc[i].time_io[j]);
      */

      printf("IO Type: %s \n", list_proc[i].io[j]->type);
      printf("IO start time: %d \n", list_proc[i].time_io[j]);

    }
    fprintf(file_val, "*******************\n");

  }

  fclose(file_val);
}

/* Function that informs if all processes have already finished */
int finished() {
  int i;
  for (i = 0; i < NUM_PROC; i++) {
    if (list_proc[i].status != EXIT) {
      return 0;
    }
  }
  return 1;
}

/* Function that maps the process pid to its index in the process list */
int pid_to_idx(int pid) {
  return pid - 100;
}

#endif