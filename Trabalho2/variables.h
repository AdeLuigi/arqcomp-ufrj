#ifndef variables
#define variables

#define TIMESLICE 4
#define HIGH_PRIOT 1
#define LOW_PRIOT 0
#define DISK 0
#define PRINTER 1
#define MAG_TAPE 2
#define NUM_PROC 10
#define NUM_IO 3
#define MAX_NUM_IO_PROC 3
#define TMP_START_FIRST_PROC 5
#define MAX_TMP_START 10
#define MIN_TMP_SRVC_PROC 2
#define MAX_TMP_SRVC_PROC 20
#define TIME_EXEC_DISK 5
#define TIME_EXEC_MAG_TAPE 8
#define TIME_EXEC_PRINTER 12
#define NUM_STATUS_PROC 3
#define NUM_PRIORITY_PROC 3
#define NEW 1
#define READY 2
#define RUNNING 3
#define BLOCKED 4
#define EXIT 5

typedef struct _io {

  char type[100]; /* Type of IO: "disk", "printer" or "mag_tape" */
  int time_exec; /* Time that the IO takes to be fully performed */
  int queue_priority; /* Indicates which priority queue a process that finishes doing this type of IO goes to */

}
IO;

typedef struct _process {

  int pid;
  int ppid;
  int priority;
  int status; /* The status can be NEW, READY, RUNNING, BLOCKED OR EXIT. Will have value 0 for processes not yet created */
  IO ** io; /* Vector of pointers to each IO that the process will execute */
  int num_io; /* Number of IO's of the process */
  int time_start; /* Time that the process starts */
  int time_srvc; /* Total service time of the process */
  int * time_io; /* Start time of each IO */
  int time_srvc_count; /* Counter of time already executed of the process */
  int curr_io_idx; /* If the process is in IO, this variable stores the index of which IO the process is in */
  int io_return_time; /* Indicates in which time unit the process will return from IO */
  int time_end; /* Time that the process finished. Used to calculate the turnaround */

}
PROCESS;

typedef struct _queue {

  int begin;
  int end;
  int size; /* Number of elements in the queue */
  int * arr; /* Vector of elements of the queue */
  int arr_size; /* Maximum size of the queue */

}
QUEUE;
#endif