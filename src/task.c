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
#include <string.h>

int next_task_id = 1;

int get_next_task_id() {
    return next_task_id++;
}

Task add_task(Task tarefa, Task* queue, int* waiting_tasks, int parallel_tasks) {
    int task_id = get_next_task_id(); // Obter o próximo ID único
    tarefa.id = task_id; // Atribuir o ID à tarefa

    for(int i = 0; i < parallel_tasks; i++) {
        if(!queue[i].ocupada) {
            queue[i] = tarefa;
            queue[i].ocupada = true; // Marca a posição como ocupada
            (*waiting_tasks)++;
            break;
        }
    }
    return tarefa;
}

void remove_task(Task tarefa, Task* queue, int num_completed, int* waiting_tasks, int parallel_tasks, Task* removed_tasks) {
    num_completed = 0;
    for(int i = 0; i < parallel_tasks; i++) {
        if(queue[i].ocupada && queue[i].id == tarefa.id) {
            queue[i].ocupada = false; // Marca a posição como vazia
            (*waiting_tasks)--;
            removed_tasks[num_completed] = queue[i];
            num_completed++;
            break;
        }
    }
}

Task getFaster(Task* queue, int parallel_tasks) {
    Task faster = queue[0]; // é garantido que apos um add a primeira posição da queue é preenchida

    for(int i = 1; i < parallel_tasks; i++){
        if(queue[i].ocupada && queue[i].tempo < faster.tempo){
            faster = queue[i];
        }
    }

    return faster;
}

Task* getPendingTasks(Task* queue, int parallel_tasks, int waiting_tasks) {
    Task* pendingTasks = malloc(waiting_tasks * sizeof(Task));
    int j = 0;
    for (int i = 0; i < parallel_tasks; i++) {
        if (queue[i].ocupada) {
            pendingTasks[j] = queue[i];
            j++;
        }
    }
    return pendingTasks;
}


