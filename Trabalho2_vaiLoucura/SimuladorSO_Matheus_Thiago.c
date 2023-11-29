/**
	Thiago Henrique Neves Coelho - 116016400
	Matheus Henrique Panno Guimaraes - 116036387
**/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

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

	char type[100]; // Tipo de IO: "disk", "printer" ou "mag_tape"
	int time_exec; // Tempo que o IO demora para ser totalmente realizado
	int queue_priority; // Indica para qual fila de prioridade vai um processo que termina de fazer esse tipo de IO

} IO;

typedef struct _process{

	int pid;
	int ppid;
	int priority;
	int status; // O status pode ser NEW, READY, RUNNING, BLOCKED OU EXIT. Terá valor 0 para processos ainda nao criados
	IO **io; // Vetor de ponteiros para cada IO que o processo executara
	int num_io; // Quantidade de IO's do processo
	int time_start; // Tempo que o processo comeca
	int time_srvc; // Tempo de servico total do processo
	int *time_io; // Tempo de inicio de cada IO
	int time_srvc_count; // Contador de tempo ja executado do processo
	int curr_io_idx; // Caso o processo esteja em IO, essa variavel guarda o indice de qual IO o processo esta
	int io_return_time; // Indica em qual unidade de tempo o processo voltara do IO
	int time_end; // Tempo que o processo finalizou. Utilizado para calcular o turnaround

} PROCESS;

typedef struct _queue{

	int begin;
	int end;
	int size; // Quantidade de elementos na fila
	int *arr; // Vetor de elementos da fila
	int arr_size; // Tamanho maximo da fila

} QUEUE;

/* Variaveis dos tipos de IO */
IO disk;
IO printer;
IO mag_tape;


/* Lista de processos */
PROCESS *list_proc;


/* Fila de Processos */
QUEUE queue_high_priot;
QUEUE queue_low_priot;
QUEUE queue_io[3];


/* Funcao que inicializa as filas */
void initialize_queue() {
	queue_high_priot.arr = (int *) malloc(NUM_PROC * sizeof(int));
	if(!queue_high_priot.arr){
		printf("Erro ao alocar memoria para a fila de alta prioridade\n");
		exit(1);
	}

	queue_low_priot.arr = (int *) malloc(NUM_PROC * sizeof(int));
	if(!queue_low_priot.arr){
		printf("Erro ao alocar memoria para a fila de baixa prioridade\n");
		exit(1);
	}

	for(int i = 0; i < NUM_IO; i++) {
		queue_io[i].arr = (int *) malloc(NUM_PROC * sizeof(int));
		if(!queue_io[i].arr) {
			printf("Erro ao alocar memoria para a fila de processos do IO\n");
			exit(1);
		}
		queue_io[i].begin = 0;
		queue_io[i].end = 0;
		queue_io[i].size = 0;
		queue_io[i].arr_size = NUM_PROC;
	}

	queue_high_priot.begin = queue_low_priot.begin = 0;
	queue_high_priot.end = queue_low_priot.end = 0;
	queue_high_priot.size = queue_low_priot.size = 0;
	queue_high_priot.arr_size = queue_low_priot.arr_size = NUM_PROC;
}

/* Retira e retorna o primeiro elemento da fila */
/* Retorna -1 se a fila estiver vazia */
int queue_pop(QUEUE *q) {
	if(q->size == 0) {
		return -1;
	}

	int ret = q->arr[q->begin];
	q->begin = (q->begin + 1) % q->arr_size;
	q->size--;
	return ret;
}

/* Retorna o primeiro elemento da fila */
/* Retorna -1 se a fila estiver vazia */
int queue_front(QUEUE *q) {
	if(q->size == 0) {
		return -1;
	}
	return q->arr[q->begin];
}

/* Insere um elemento na fila */
void queue_push(QUEUE *q, int elem) {
	/* Da erro se tentar inserir um numero de elementos maior que o tamanho total da fila */
	assert(q->size < q->arr_size);

	q->arr[q->end] = elem;
	q->end = (q->end + 1) % q->arr_size;
	q->size++;
}

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
	list_proc = (PROCESS *)malloc(NUM_PROC*sizeof(PROCESS));
	if(!list_proc){
		printf("Erro ao alocar memoria para lista de processos\n");
		exit(1);
	}
	int i, j;

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
				case 0: //DISK
					list_proc[i].io[j] = &disk;
					break;
				case 1: //PRINTER
					list_proc[i].io[j] = &printer;
					break;
				case 2: //MAG_TAPE
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

/* Imprime os valores dentro de uma fila */
void print_queue(QUEUE *queue, FILE *file) {
	int i, count = 0;
	fprintf(file, "[ ");
	for(i = queue->begin; count < queue->size; count++, i = (i + 1) % queue->arr_size) {
		if(i != queue->begin) {
			fprintf(file, " | ");
		}
		fprintf(file, "%d", queue->arr[i]);
	}
	fprintf(file, " ]\n");
}

/* Chama todas as funcoes de inicializacao */
void init() {
	initialize_io();
	initialize_process();
	initialize_queue();
}

/* Funcao que mapeia o pid do processo ao seu indice na lista de processos */
int pid_to_idx(int pid) {
	return pid - 100;
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

int main() {
	srand(time(NULL));
	init();
	print_val();

	FILE *file_execucao = fopen("log_execucao.txt", "w");

	int scal_proc = 0; // Quantidade de processos que ja foram escalonados
	int time_unit = 0; // Variavel contadora que informa qual unidade de tempo esta sendo executada
	int exec_pid = -1; // PID do processo que esta sendo executado no momento
	int same_proc_count = 0; // Variavel contadora que informa ha quanto tempo um mesmo processo esta executando
	int i;

	/* Loop principal */
	while(!finished()) {
		fprintf(file_execucao, "Tempo %d: ", time_unit);
		if(exec_pid == -1)
			fprintf(file_execucao, "Nenhum processo esta em execucao\n\n");
		else
			fprintf(file_execucao, "O processo de pid = %d esta em execucao\n\n", exec_pid);

		/* Contabiliza o tempo de execucao do processo atual e verifica se ele terminou sua execucao */
		if(exec_pid != -1) {
			int idx = pid_to_idx(exec_pid);
			same_proc_count++;
			list_proc[idx].time_srvc_count++;
			if(list_proc[idx].time_srvc_count == list_proc[idx].time_srvc) {
				fprintf(file_execucao, "- Processo de pid = %d terminou sua execucao\n", exec_pid);
				list_proc[idx].time_end = time_unit;
				list_proc[idx].status = EXIT;
				exec_pid = -1;
			}
		}

		/* Coloca na fila os processos que iniciarem nesta unidade de tempo */
		while(scal_proc < NUM_PROC && list_proc[scal_proc].time_start == time_unit) {
			list_proc[scal_proc].status = NEW;
			fprintf(file_execucao, "- Processo de pid = %d foi criado agora e foi colocado na fila de alta prioridade\n", list_proc[scal_proc].pid);
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
						fprintf(file_execucao, "- Processo de pid = %d voltou de um IO do tipo %s e foi colocado na fila de alta prioridade\n", list_proc[idx].pid, curr_io.type);
						queue_push(&queue_high_priot, list_proc[idx].pid);
					}
					else {
						fprintf(file_execucao, "- Processo de pid = %d voltou de um IO do tipo %s e foi colocado na fila de baixa prioridade\n", list_proc[idx].pid, curr_io.type);
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
					fprintf(file_execucao, "- Processo de pid = %d sofreu uma interrupcao de IO do tipo %s e foi bloqueado\n", exec_pid, list_proc[idx].io[i]->type);
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
			fprintf(file_execucao, "- Processo de pid = %d executou o maximo de unidades de tempo seguidas (%d) e foi colocado na fila de baixa prioridade\n", exec_pid, TIMESLICE);
			queue_push(&queue_low_priot, exec_pid);
			exec_pid = -1;
		}

		/* Coloca um processo em execucao, se for possivel */
		if(exec_pid == -1) {
			same_proc_count = 0;
			/* Busca um processo na fila de alta prioridade */
			exec_pid = queue_pop(&queue_high_priot);
			if(exec_pid != -1) {
				fprintf(file_execucao, "- Processo de pid = %d saiu da fila de alta prioridade e foi colocado em execucao\n", exec_pid);
				list_proc[pid_to_idx(exec_pid)].status = RUNNING;
			}
			/* Se a fila de alta prioridade estiver vazia, busca um processo na fila de baixa prioridade */
			else {
				exec_pid = queue_pop(&queue_low_priot);
				if(exec_pid != -1) {
					fprintf(file_execucao, "- Processo de pid = %d saiu da fila de baixa prioridade e foi colocado em execucao\n", exec_pid);
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


