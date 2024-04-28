#ifndef TASK_H
#define TASK_H

#include <sys/types.h>
#include <stdbool.h>

#define MAX_QUEUE_SIZE 100
#define QUEUE_FIFO "queue_fifo"

typedef struct task {
    int id;
    pid_t pid;
    char comando[10];
    char programa[100];
    char flag[4];
    int tempo; // ms
    bool ocupada; // Flag para indicar se a posição está ocupada ou não
} Task;

int add_task();
Task getFaster();
void remove_task(Task tarefa);
int get_next_task_id();
int getWaitingTasks();
void freeQueue();
void initQueue();
void write_fifo(Task tarefa);

#endif