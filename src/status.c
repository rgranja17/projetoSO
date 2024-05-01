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

//inicializa a estrutura de dados, recebe o número maximo de tarefas paralelas, depois aloca memoria para as tarefas, inicia a ocupacao tudo a falso
void __status_init_(int max_parallel_tasks){ 
    executing_tasks = malloc(max_parallel_tasks * sizeof(Task));
    parallel_tasks = max_parallel_tasks;

    for(int i = 0; i < max_parallel_tasks; i++){
        executing_tasks[i].ocupation = false;
    }
}

// adicionar uma tarefa ao array

void __status_add_task_(Task task_executing){
    for(int i = 0; i < parallel_tasks; i++){
        if(!executing_tasks[i].ocupation){
            executing_tasks[i] = task_executing;
            executing_tasks[i].ocupation = true;
            break;
        }
    }
}

//remove a tarefa em especifico quando acaba de ser executada do array, e abre um ficheiro auxiliar e escreve a estrutura da tarefa executada
//para posteriormente mandar para o cliente as tarefas que ja estao executadas

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

// funcao que retorna as tarefas executas, abrindo o ficheiro auxiliar e ir lendo. interessa-nos apenas o tempo 

char* __status_get_executed_tasks_(){
    int fd = open(AUXILIAR_FILE, O_RDONLY);
    char buffer[2048];
    buffer[0] = '\0';

    Task tmp;
    char aux[256];
    while((read(fd,&tmp,sizeof(Task))) > 0){
        snprintf(aux, sizeof(aux), "%d %s %d\n", tmp.id, tmp.program,tmp.time);
        strncat(buffer, aux, 2048 - strlen(buffer) - 1); // buffer é a string do destino à qual é adicionada a string aux (uma so tarefa)
    }
    close(fd);
    char* result = strdup(buffer); // Aloca memória dinamicamente e copia o conteúdo de buffer
    return result;
}

//adiciona ao array auxiliar as tarefas que estao em execucao e retorna-as, esta a percorrer o array de tasks, buscando cada tarefa e concateneia 
//cada tarefa ao array auxiliar (executing_tasks_str)
char* __status_get_executing_tasks_(){
    char executing_tasks_str[1024];
    executing_tasks_str[0] = '\0';
    for(int i = 0; i < parallel_tasks; i++){
        char aux[256];
        snprintf(aux,sizeof(aux),"%d %s\n",executing_tasks[i].id,executing_tasks[i].program);
        strcat(executing_tasks_str,aux);
    }
    char* result = strdup(executing_tasks_str); // Aloca memória dinamicamente e copia o conteúdo de buffer
    return result;
}

// retorna as tarefas que estao à espera para serem executas 
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
    char* result = strdup(buffer); // Aloca memória dinamicamente e copia o conteúdo de buffer
    return result;
}

char* __status_get_server_state(){
    char status[4096];
    status[0] = '\0';

    strcat(status, "Executing:\n");
    snprintf(status + strlen(status), sizeof(status), "%s", __status_get_executing_tasks_());

    strcat(status, "Schedule:\n");
    snprintf(status + strlen(status), sizeof(status), "%s", __status_get_schedule_tasks_());

    strcat(status, "Executed:\n");
    snprintf(status + strlen(status), sizeof(status), "%s", __status_get_executed_tasks_());

    return status;
}