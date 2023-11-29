#ifndef queue
#define queue

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "variables.h"
#include "process.h"


/* Fila de Processos */
QUEUE queue_high_priot;
QUEUE queue_low_priot;
QUEUE queue_io[3];

void initialize_queue() {
	queue_high_priot.arr = (int *) malloc(NUM_PROC * sizeof(int));
  int i;
	if(!queue_high_priot.arr){
		printf("Erro na alocação de memória - fila de alta prioridade\n");
		exit(1);
	}

	queue_low_priot.arr = (int *) malloc(NUM_PROC * sizeof(int));
	if(!queue_low_priot.arr){
		printf("Erro na alocação de memória -  fila de baixa prioridade\n");
		exit(1);
	}
	for( i = 0; i < NUM_IO; i++) {
		queue_io[i].arr = (int *) malloc(NUM_PROC * sizeof(int));
		if(!queue_io[i].arr) {
			printf("Erro na alocação de memória - fila de processos IO\n");
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

/* Imprime os valores dentro de uma fila */
void print_queue(QUEUE *q, FILE *file) {
	int i, count = 0;
	fprintf(file, "[ ");
	for(i = q->begin; count < q->size; count++, i = (i + 1) % q->arr_size) {
		if(i != q->begin) {
			fprintf(file, " | ");
		}
		fprintf(file, "%d", q->arr[i]);
	}
	fprintf(file, " ]\n");
}

#endif
