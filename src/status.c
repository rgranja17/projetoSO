#include "../include/status.h"
#include "../include/scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>

#define AUXILIAR_FILE "../tmp/tasks_done"

Task* executing_tasks;
int parallel_tasks;

void __status_init_(int max_parallel_tasks){
    executing_tasks = malloc(max_parallel_tasks * sizeof(Task));
    parallel_tasks = max_parallel_tasks;

    for(int i = 0; i < max_parallel_tasks; i++){
        executing_tasks[i].ocupation = false;
    }
}

void __status_add_task_(Task task_executing){
    for(int i = 0; i < parallel_tasks; i++){
        if(!executing_tasks[i].ocupation){
            executing_tasks[i] = task_executing;
            executing_tasks[i].ocupation = true;
            break;
        }
    }
}

void __status_remove_task_(Task task_executed){
    for(int i = 0; i < parallel_tasks; i++){
        if(executing_tasks[i].id == task_executed.id){
            executing_tasks[i].ocupation = false;
            break;
        }
    }
    int fd = open(AUXILIAR_FILE, O_CREAT | O_APPEND | O_WRONLY, 0644);
    ssize_t bytes_written;
    bytes_written = write(fd,&task_executed,sizeof(Task));
    if(bytes_written <= 0){
        perror("Erro de escrita no ficheiro auxiliar");
    }
    close(fd);
}

char* __status_get_executed_tasks_(){
    int fd = open(AUXILIAR_FILE, O_RDONLY);
    char buffer[2048];
    buffer[0] = '\0';

    Task tmp;
    char aux[512];
    while((read(fd,&tmp,sizeof(Task))) > 0){
        snprintf(aux, sizeof(aux), "%d %s %dms\n", tmp.id, tmp.program,tmp.time);
        strncat(buffer, aux, 2048 - strlen(buffer) - 1);
    }
    close(fd);
    char* result = strdup(buffer);
    return result;
}

char* __status_get_executing_tasks_(){
    char executing_tasks_str[1024];
    executing_tasks_str[0] = '\0';
    for(int i = 0; i < parallel_tasks; i++){
        if(executing_tasks[i].ocupation){
            char aux[256];
            snprintf(aux,sizeof(aux),"%d %s\n",executing_tasks[i].id,executing_tasks[i].program);
            strcat(executing_tasks_str,aux);
        }

    }
    char* result = strdup(executing_tasks_str);
    return result;
}

char* __status_get_schedule_tasks_(){
    Task* pending_tasks = __scheduler_get_schedule_tasks();
    int schedule_tasks = __scheduler_get_schedule_tasks_num();
    char buffer[1024];
    buffer[0] = '\0';

    char aux[256];
    for(int i = 0; i < schedule_tasks; i++){
        snprintf(aux,sizeof(aux),"%d %s\n",pending_tasks[i].id,pending_tasks[i].program);
        strcat(buffer,aux);
    }
    char* result = strdup(buffer);
    return result;
}

char* __status_get_server_state(){
    // Obtenha os strings de tarefas
    char* executing_tasks_str = __status_get_executing_tasks_();
    char* schedule_tasks_str = __status_get_schedule_tasks_();
    char* executed_tasks_str = __status_get_executed_tasks_();

    // Calcule o comprimento total necessário para o status
    ssize_t total_length = strlen(executing_tasks_str) + strlen(schedule_tasks_str) + strlen(executed_tasks_str) + 50;

    // Aloque memória para o status
    char* status = malloc(total_length * sizeof(char));

    // Verifique se a alocação foi bem-sucedida
    if (status == NULL) {
        perror("Erro: Falha ao alocar memória para status");
        exit(EXIT_FAILURE);
    }

    // Escreva o status no buffer
    int written = snprintf(status, total_length, "\nExecuting:\n%s\nSchedule:\n%s\nExecuted:\n%s\n", executing_tasks_str, schedule_tasks_str, executed_tasks_str);

    // Verifique se houve erro ao escrever no buffer
    if (written < 0) {
        perror("Erro ao criar a string de status");
        free(status);
        exit(EXIT_FAILURE);
    }

    // Libere a memória alocada para os strings de tarefas
    free(executing_tasks_str);
    free(schedule_tasks_str);
    free(executed_tasks_str);

    return status;
}