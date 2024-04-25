#ifndef TASK_H
#define TASK_H

#include <sys/types.h>
#include <stdbool.h>


typedef struct task {
    int id;
    pid_t pid;
    char programa[100];
    int tempo; // ms
    bool ocupada; // Flag para indicar se a posição está ocupada ou não
} Task;

void add_task(Task tarefa, Task* queue, int* waiting_tasks, int parallel_tasks);
Task getFaster(Task* queue, int waiting_tasks);
void remove_task(Task tarefa, Task* queue, int* waiting_tasks, int parallel_tasks);
int get_next_task_id();

#endif