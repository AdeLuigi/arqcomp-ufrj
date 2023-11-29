/*
Nome: Lucas Tatsuya Tanaka
DRE:1180058149

Nome: Ademario Vitor Costa de Santana
DRE:118143148

Nome: Felipe de Jesus Araujo da Conceição
DRE:
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "queue.h"
#include "variables.h"
#include "process.h"

/* Fila de Processos */
QUEUE queue_high_priot;
QUEUE queue_low_priot;
QUEUE queue_io[3];

/* Chama todas as funcoes de inicializacao */
void init() {
	initialize_io();
	initialize_process();
	initialize_queue();
}

int main() {
	srand(time(NULL));
	init();
	print_val();

	FILE *file_execucao = fopen("log_execucao.txt", "w");

	int scal_proc = 0; /*  Quantidade de processos que ja foram escalonados */
	int time_unit = 0; /*  Variavel contadora que informa qual unidade de tempo esta sendo executada */
	int exec_pid = -1;/*  PID do processo que esta sendo executado no momento */
	int same_proc_count = 0;/*   Variavel contadora que informa ha quanto tempo um mesmo processo esta executando */
	int i;

	/* Loop principal */
	while(!finished()) {
		fprintf(file_execucao, "Time %d: ", time_unit);
		if(exec_pid == -1)
			fprintf(file_execucao, "0 processes running\n\n");
		else
			fprintf(file_execucao, "PID = %d em execucao\n\n", exec_pid);

		/* Contabiliza o tempo de execucao do processo atual e verifica se ele terminou sua execucao */
		if(exec_pid != -1) {
			int idx = pid_to_idx(exec_pid);
			same_proc_count++;
			list_proc[idx].time_srvc_count++;
			if(list_proc[idx].time_srvc_count == list_proc[idx].time_srvc) {
				fprintf(file_execucao, "- PID = %d terminou sua execucao\n", exec_pid);
				list_proc[idx].time_end = time_unit;
				list_proc[idx].status = EXIT;
				exec_pid = -1;
			}
		}

		/* Coloca na fila os processos que iniciarem nesta unidade de tempo */
		while(scal_proc < NUM_PROC && list_proc[scal_proc].time_start == time_unit) {
			list_proc[scal_proc].status = NEW;
			fprintf(file_execucao, "- PID = %d CRIADO - alta prioridade\n", list_proc[scal_proc].pid);
			list_proc[scal_proc].status = READY;
			queue_push(&queue_high_priot, list_proc[scal_proc].pid);
			scal_proc++;
		}

		/* Verifica processos que voltaram do IO e coloca-os na fila */
		for(i = 0; i < NUM_IO; i++) {
			/* pid do primeiro processo da fila do i-esimo IO */
			int pid_ret = queue_front(&queue_io[i]);

			/* Se houver processo, verifica se ele vai retornar do IO agora */
			if(pid_ret != -1) {
				int idx = pid_to_idx(pid_ret);
				if(list_proc[idx].io_return_time == time_unit) {
					queue_pop(&queue_io[i]);
					IO curr_io = *list_proc[idx].io[list_proc[idx].curr_io_idx];
					if(curr_io.queue_priority == HIGH_PRIOT) {
						fprintf(file_execucao, "- PID = %d voltou de um IO do tipo %s e foi colocado na fila de alta prioridade\n", list_proc[idx].pid, curr_io.type);
						queue_push(&queue_high_priot, list_proc[idx].pid);
					}
					else {
						fprintf(file_execucao, "- PID = %d voltou de um IO do tipo %s e foi colocado na fila de baixa prioridade\n", list_proc[idx].pid, curr_io.type);
						queue_push(&queue_low_priot, list_proc[idx].pid);
					}
					list_proc[idx].status = READY;
				}
			}
		}

		/* Verifica interrupcoes de IO do processo atual */
		if(exec_pid != -1) {
			int idx = pid_to_idx(exec_pid);
			for(i = 0; i < list_proc[idx].num_io; i++) {
				/* Verifica se a interrupcao de IO ocorre nesta unidade de tempo */
				if(list_proc[idx].time_io[i] == list_proc[idx].time_srvc_count) {
					/* Coloca o PID do processo na fila do IO correspondente */
					if(strcmp(disk.type, list_proc[idx].io[i]->type) == 0)
						queue_push(&queue_io[DISK], exec_pid);
					else if(strcmp(printer.type, list_proc[idx].io[i]->type) == 0)
						queue_push(&queue_io[PRINTER], exec_pid);
					else if(strcmp(mag_tape.type, list_proc[idx].io[i]->type) == 0)
						queue_push(&queue_io[MAG_TAPE], exec_pid);

					/* Bloqueia o processo, define o tempo em que o ele retornara e qual IO ele esta fazendo */
					fprintf(file_execucao, "- PID = %d sofreu uma interrupcao de IO do tipo %s e foi bloqueado\n", exec_pid, list_proc[idx].io[i]->type);
					list_proc[idx].curr_io_idx = i;
					list_proc[idx].io_return_time = time_unit + list_proc[idx].io[i]->time_exec;
					list_proc[idx].status = BLOCKED;
					exec_pid = -1;
					break;
				}
			}
		}

		/* Preempcao para nao exceder o time slice */
		if(exec_pid != -1 && same_proc_count == TIMESLICE) {
			fprintf(file_execucao, "- PID = %d executou o maximo de unidades de tempo seguidas (%d) e foi colocado na fila de baixa prioridade\n", exec_pid, TIMESLICE);
			queue_push(&queue_low_priot, exec_pid);
			exec_pid = -1;
		}

		/* Coloca um processo em execucao, se for possivel */
		if(exec_pid == -1) {
			same_proc_count = 0;
			/* Busca um processo na fila de alta prioridade */
			exec_pid = queue_pop(&queue_high_priot);
			if(exec_pid != -1) {
				fprintf(file_execucao, "- PID = %d saiu da fila de alta prioridade e foi colocado em execucao\n", exec_pid);
				list_proc[pid_to_idx(exec_pid)].status = RUNNING;
			}
			/* Se a fila de alta prioridade estiver vazia, busca um processo na fila de baixa prioridade */
			else {
				exec_pid = queue_pop(&queue_low_priot);
				if(exec_pid != -1) {
					fprintf(file_execucao, "- PID = %d saiu da fila de baixa prioridade e foi colocado em execucao\n", exec_pid);
					list_proc[pid_to_idx(exec_pid)].status = RUNNING;
				}
			}
		}

		/* Imprime as filas */
		fprintf(file_execucao, "High-priority queue:     ");
		print_queue(&queue_high_priot, file_execucao);
		fprintf(file_execucao, "Low-priority queue:    ");
		print_queue(&queue_low_priot, file_execucao);
		fprintf(file_execucao, "I/O queue for Disk:     ");
		print_queue(&queue_io[DISK], file_execucao);
		fprintf(file_execucao, "I/O queue for Printer:  ");
		print_queue(&queue_io[PRINTER], file_execucao);
		fprintf(file_execucao, "I/O queue for mag_tape: ");
		print_queue(&queue_io[MAG_TAPE], file_execucao);

		fprintf(file_execucao, "\n-----------------------------------------------\n");

		time_unit++;
	}

	FILE *file_turnarounds = fopen("log_turnarounds.txt", "w");
	fprintf(file_turnarounds, "Turnarounds:\n");
	for(i = 0; i < NUM_PROC; i++) {
		fprintf(file_turnarounds, "Processo %d: %d\n", i + 100, list_proc[i].time_end - list_proc[i].time_start);
	}

	fclose(file_execucao);
	fclose(file_turnarounds);

	printf("Simulacao finalizada\n");
	printf("Verifique os arquivos de log para conferir os resultados:\n");
	printf("- log_execucao.txt\n");
	printf("- log_init_val.txt\n");
	printf("- log_turnarounds.txt\n");

	return 0;
}
