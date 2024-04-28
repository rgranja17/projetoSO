#include "../include/task.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <string.h>


int next_task_id = 1;
int waiting_tasks = 0;
int queue_fifo;
Task* queue;

void write_fifo(Task tarefa){
    queue_fifo = open(QUEUE_FIFO,O_WRONLY);
    write(queue_fifo,&tarefa,sizeof(Task));
    close(queue_fifo);
}

int get_next_task_id() {
    return next_task_id++;
}

void initQueue(){
    if(mkfifo(QUEUE_FIFO,0666) == -1){
        if(errno != EEXIST){
            perror("Erro criação fifo");
        }
    }
    queue_fifo = open(QUEUE_FIFO,O_RDWR);
    queue = malloc(MAX_QUEUE_SIZE * sizeof(Task));

    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        queue[i].ocupada = false;
    }
}

int add_task(){
    Task task_to_add;

    printf("inicio add\n");
    queue_fifo = open(QUEUE_FIFO, O_RDONLY);
    read(queue_fifo,&task_to_add,sizeof(Task));
    close(queue_fifo);

    printf("leitura fifo\n");
    printf("%s || %s\n", task_to_add.comando,task_to_add.programa); // check

    int task_id = get_next_task_id();
    task_to_add.id = task_id;

    printf("%d\n", task_to_add.id); // check

    for(int i = 0; i < MAX_QUEUE_SIZE; i++) {
        printf("Teste1\n");
        if(!queue[i].ocupada) {
            printf("Teste2\n");
            queue[i] = task_to_add;
            printf("Teste3\n");
            queue[i].ocupada = true;
            waiting_tasks++;
            break;
        }
    }
    return task_to_add.id;
}

void remove_task(Task tarefa) {
    for(int i = 0; i < MAX_QUEUE_SIZE; i++) {
        if(queue[i].ocupada && queue[i].id == tarefa.id) {
            queue[i].ocupada = false; // Marca a posição como vazia
            waiting_tasks--;
            break;
        }
    }
}

int getWaitingTasks(){
    return waiting_tasks;
}

Task getFaster() {
    Task faster;
    faster.tempo = INT_MAX;
    faster.id = -1; // Defina um ID inválido para indicar que nenhuma tarefa foi encontrada ainda

    for (int i = 0; i < MAX_QUEUE_SIZE; i++) {
        if (queue[i].ocupada && queue[i].tempo < faster.tempo) {
            faster = queue[i];
        }
    }

    return faster;
}

void freeQueue(){
    free(queue);
    close(queue_fifo);
    unlink(QUEUE_FIFO);
}



