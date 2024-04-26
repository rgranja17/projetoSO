#ifndef TASK_H
#define TASK_H

#include <sys/types.h>
#include <stdbool.h>


typedef struct task {
    int id;
    pid_t pid;
    char comando[10];
    char programa[100];
    char flag[4];
    int tempo; // ms
    bool ocupada; // Flag para indicar se a posição está ocupada ou não
} Task;

Task add_task(Task tarefa, Task* queue, int* waiting_tasks, int parallel_tasks);
Task getFaster(Task* queue, int waiting_tasks);
void remove_task(Task tarefa, Task* queue, int* waiting_tasks, int parallel_tasks);
int get_next_task_id();
char* getPendingTasks(Task* queue, int parallel_tasks, int waiting_tasks);
void getCompletedTasks(char* status_message);

#endif