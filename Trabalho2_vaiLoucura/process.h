#ifndef process
#define process

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "variables.h"

/* Variaveis dos tipos de IO */
IO disk;
IO printer;
IO mag_tape;

/* Lista de processos */
PROCESS *list_proc;

/* Funcao que inicializa as variaveis de IO */
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


/* Funcao que inicializa a lista de processos */
void initialize_process(){
	int i, j;
	list_proc = (PROCESS *)malloc(NUM_PROC*sizeof(PROCESS));

	if(!list_proc){
		printf("Erro ao alocar memoria para lista de processos\n");
		exit(1);
	}

	for(i = 0; i < NUM_PROC; i++){
		if(i == 0) {
			list_proc[i].time_start = (int)(rand() % (1 + MAX_TMP_START) );
		}
		else {
			list_proc[i].time_start = list_proc[i-1].time_start + (int)(rand() % (1 + MAX_TMP_START) );
		}
		list_proc[i].pid = i + 100;
		list_proc[i].ppid = 1;
		list_proc[i].priority = (int)(rand() % NUM_PRIORITY_PROC);
		list_proc[i].status = 0;
		list_proc[i].time_srvc_count = 0;
		list_proc[i].curr_io_idx = -1;
		list_proc[i].io_return_time = -1;

		int time_srvc_proc = (int)(rand()% (1 + MAX_TMP_SRVC_PROC) );
		list_proc[i].time_srvc = time_srvc_proc < MIN_TMP_SRVC_PROC ? MIN_TMP_SRVC_PROC : time_srvc_proc;

		/* Seleciona a quantidade de interrupcoes de IO do processo */
		list_proc[i].num_io = (int)(rand() % (1 + MAX_NUM_IO_PROC) );
		if(list_proc[i].num_io > list_proc[i].time_srvc - 2) {
			list_proc[i].num_io = list_proc[i].time_srvc - 2;
		}

		/* Aloca memoria para a lista de IOs do processo */
		list_proc[i].io = (IO **)malloc(list_proc[i].num_io * sizeof(IO *));
		if(!list_proc[i].io){
			printf("Erro ao alocar memoria para lista de IO do processo: %d\n",i);
			exit(1);
		}

		/* Aloca memoria para a lista dos tempos de inicio de cada IO do processo */
		list_proc[i].time_io = (int*)malloc(list_proc[i].num_io * sizeof(int));
		if(!list_proc[i].time_io){
			printf("Erro ao alocar memoria para lista de tempo de inicio de IO do processo: %d\n",i);
			exit(1);
		}

		/* Seleciona os tipos de IO para o processo */
		for(j=0;j<list_proc[i].num_io;j++){
			switch(rand() % 3){
				case 0: /* DISK */
					list_proc[i].io[j] = &disk;
					break;
				case 1: /* PRINTER */
					list_proc[i].io[j] = &printer;
					break;
				case 2: /* MAG_TAPE */
					list_proc[i].io[j] = &mag_tape;
					break;
			}

			/* Seleciona o tempo de inicio do IO */
			/* Garante que todos os IOs do processo tenham tempos diferentes */
			/* Um IO nao pode ser a primeira nem a ultima coisa que um processo faz */
			int time_io = -1;
			while(time_io == -1 || time_io == 0 || time_io == list_proc[i].time_srvc) {
				int k;
				time_io = rand() % list_proc[i].time_srvc;
				for(k = 0; k < j; k++) {
					if(list_proc[i].time_io[k] == time_io) {
						time_io = -1;
					}
				}
			}
			list_proc[i].time_io[j] = time_io;
		}
	}
}

/* Imprime os valores iniciais das informacoes do processos */
void print_val(){
	int i,j;

	FILE *file_val = fopen("log_init_val.txt", "w");

	for(i=0;i<NUM_PROC;i++){

		fprintf(file_val, "Processo PID: %d \n",list_proc[i].pid);
		fprintf(file_val, "PPID: %d \n",list_proc[i].ppid);
		fprintf(file_val, "Prioridade: %d \n",list_proc[i].priority);
		fprintf(file_val, "Tempo Inicio: %d \n",list_proc[i].time_start);
		fprintf(file_val, "Tempo Servico: %d \n",list_proc[i].time_srvc);


		int tam_io=list_proc[i].num_io;
		for(j=0;j<tam_io;j++){
			fprintf(file_val, "Tipo IO: %s \n",list_proc[i].io[j]->type);
			fprintf(file_val, "Tempo de inicio do IO: %d \n", list_proc[i].time_io[j]);

		}
		fprintf(file_val, "--------------\n");

	}

	fclose(file_val);
}


/* Funcao que informa se todos os processos ja terminaram */
int finished() {
	int i;
	for(i = 0; i < NUM_PROC; i++) {
		if(list_proc[i].status != EXIT) {
			return 0;
		}
	}
	return 1;
}

/* Funcao que mapeia o pid do processo ao seu indice na lista de processos */
int pid_to_idx(int pid) {
	return pid - 100;
}

#endif
