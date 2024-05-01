#include "../include/orchestrator.h"
#include "../include/scheduler.h"
#include "../include/engine.h"
#include "../include/status.h"
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

#define SERVER_CLIENT_FIFO "../tmp/server_client_fifo"

int main(int argc, char** argv) {
    if(argc < 3){
        char error_message[30];
        snprintf(error_message,sizeof(error_message),"Use: %s <output-path> <parallel_tasks>\n",argv[0]);
        write(STDOUT_FILENO,error_message,strlen(error_message));
        return 1;
    }
    int max_parallel_tasks = atoi(argv[2]);
    if(max_parallel_tasks == 0){
        char error_message[] = "Min parallel tasks of zero\n";
        write(STDOUT_FILENO,error_message,strlen(error_message));
    }
    char outputPath[100];
    strcpy(outputPath,argv[1]);

    char output_log_file_path[256];
    snprintf(output_log_file_path, sizeof(output_log_file_path), "%s/Tasks.log", argv[1]);

    int logFile_fd = open(output_log_file_path, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (logFile_fd == -1) {
        perror("Erro ao abrir/criar o arquivo de log");
        return 1;
    }
    char header[] = "ID,Program,Arguments,Runtime\n";
    if (write(logFile_fd, header, strlen(header)) <= 0) {
        printf("Erro ao escrever no ficheiro log\n");
        return 1;
    }

    if (mkfifo(SERVER_CLIENT_FIFO, 0666) == -1) {
        if(errno != EEXIST){
            perror("Erro criação fifo\n");
        }
    }

    printf("Server running...\n");
    __scheduler_init__();
    __status_init_(max_parallel_tasks);
    int num_tasks_executing = 0;
    while(1){
        Task task_read;
        Task task_executing;
        int server_client_fifo = open(SERVER_CLIENT_FIFO, O_RDONLY);

        while((read(server_client_fifo,&task_read,sizeof(Task))) > 0){
            if(strcmp(task_read.flag,"C") == 0) {
                num_tasks_executing--;
                __status_remove_task_(task_read);
                close(server_client_fifo);
                continue;
            }
            if(strcmp(task_read.command,"status") == 0){ // se o utilizador mandou status manda de volta toda a informacao do servidor
                char buffer[4096];
                buffer[0] = '\0';
                strcat(buffer,__status_get_server_state());
                server_client_fifo = open(SERVER_CLIENT_FIFO, O_WRONLY);
                write(server_client_fifo,buffer,strlen(buffer));
                close(server_client_fifo);
                continue;
            }
            close(server_client_fifo);
            int id = get_next_task_id(); // se nao foi escrito status, gera o id da proxima tarefa e manda o identificador para o cliente
            task_read.id = id;

            __scheduler_add_task__(task_read);
            server_client_fifo = open(SERVER_CLIENT_FIFO,O_WRONLY);
            write(server_client_fifo,&id,sizeof(int));
            close(server_client_fifo);

            if((num_tasks_executing < max_parallel_tasks) && !queue_empty()){ // verifica se ha espaco para executar mais tarefas e a fila nao esta vazia 

                task_executing = __schedule_get_task__(); // se houver vai buscar a proxima tarefa
                __scheduler_remove_task__(task_executing); // remove a da fila de espera
                __status_add_task_(task_executing); // e adiciona ao status de tarefas em execucao

                pid_t pid = fork(); // fork para definitivamente executar a tarefa
                if(pid == 0){
                    if(strcmp(task_executing.flag,"-u") == 0) task_executing = __engine_execute_task(task_executing,outputPath,logFile_fd);
                    if(strcmp(task_executing.flag,"-p") == 0) task_executing = __engine_execute_pipeline(task_executing,outputPath,logFile_fd);

                    strcpy(task_executing.flag,"C");
                    server_client_fifo = open(SERVER_CLIENT_FIFO,O_WRONLY);
                    write(server_client_fifo,&task_executing,sizeof(Task));
                    close(server_client_fifo);

                    _exit(1);
                }
                num_tasks_executing++;
                continue;
            }
        }
        if((num_tasks_executing < max_parallel_tasks) && !queue_empty()){
            task_executing = __schedule_get_task__();
            __scheduler_remove_task__(task_executing);
            __status_add_task_(task_executing);

            pid_t pid = fork();
            if(pid == 0){
                if(strcmp(task_executing.flag,"-u") == 0) task_executing = __engine_execute_task(task_executing,outputPath,logFile_fd);
                if(strcmp(task_executing.flag,"-p") == 0) task_executing = __engine_execute_pipeline(task_executing,outputPath,logFile_fd);

                strcpy(task_executing.flag,"C");
                server_client_fifo = open(SERVER_CLIENT_FIFO,O_WRONLY);
                write(server_client_fifo,&task_executing,sizeof(Task));
                close(server_client_fifo);

                _exit(1);
            }
            num_tasks_executing++;
        }

    }
    return 0;
}