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
#define READY	2
#define RUNNING 3
#define BLOCKED 4
#define EXIT 5


typedef struct _io{

	char type[100]; /*  Tipo de IO: "disk", "printer" ou "mag_tape" */
	int time_exec;/*  Tempo que o IO demora para ser totalmente realizado */
	int queue_priority;/*  Indica para qual fila de prioridade vai um processo que termina de fazer esse tipo de IO */

} IO;

typedef struct _process{

	int pid;
	int ppid;
	int priority;
	int status; /* O status pode ser NEW, READY, RUNNING, BLOCKED OU EXIT. Terá valor 0 para processos ainda nao criados */
	IO **io; /* Vetor de ponteiros para cada IO que o processo executara */
	int num_io; /* Quantidade de IO's do processo */
	int time_start; /* Tempo que o processo comeca */
	int time_srvc;/*  Tempo de servico total do processo */
	int *time_io; /* Tempo de inicio de cada IO */
	int time_srvc_count; /* ontador de tempo ja executado do processo */
	int curr_io_idx;/*  Caso o processo esteja em IO, essa variavel guarda o indice de qual IO o processo esta */
	int io_return_time; /* Indica em qual unidade de tempo o processo voltara do IO */
	int time_end;  /* Tempo que o processo finalizou. Utilizado para calcular o turnaround */

} PROCESS;

typedef struct _queue{

	int begin;
	int end;
	int size;/*  Quantidade de elementos na fila */
	int *arr; /* Vetor de elementos da fila */
	int arr_size;/*  Tamanho maximo da fila */

} QUEUE;
#endif
