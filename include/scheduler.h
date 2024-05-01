#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <sys/types.h>
#include <stdbool.h>

#define MAX_QUEUE_SIZE 100

typedef struct task {
    int id;
    pid_t pid;
    char command[10];
    char program[300];
    char flag[3];
    int time;
    bool ocupation;
} Task;

void __scheduler_init__();
int __scheduler_add_task__(Task task_to_add);
void __scheduler_remove_task__(Task task_to_remove);
Task __schedule_get_task__();
void __scheduler_delete_queue__();
bool queue_empty();
int get_next_task_id();
Task* __scheduler_get_schedule_tasks();
int __scheduler_get_schedule_tasks_num();

#endif