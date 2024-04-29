#include "../include/orchestrator.h"
#include "../include/scheduler.h"
#include "../include/engine.h"
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


int main(int argc, char** argv) {
    if(argc < 3){
        char error_message[30];
        snprintf(error_message,sizeof(error_message),"Use: %s <output-path> <parallel_tasks>\n",argv[0]);
        write(STDOUT_FILENO,error_message,strlen(error_message));
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
            perror("Erro criação fifo");
        }
    }
    if (mkfifo(CLIENT_SERVER_FIFO, 0666) == -1) {
        if(errno != EEXIST){
            perror("Erro criação fifo");
        }
    }

    printf("Server running...\n");
    __scheduler_init__();
    int num_tasks_executing = 0;
    while(1){
        Task task_read;
        Task task_executing;
        int server_client_fifo = open(SERVER_CLIENT_FIFO, O_WRONLY);
        int client_server_fifo = open(CLIENT_SERVER_FIFO, O_RDONLY);

        if(client_server_fifo == -1 || server_client_fifo == -1){
            close(server_client_fifo);
            close(client_server_fifo);
            break;
        }

        if(num_tasks_executing < max_parallel_tasks){
            ssize_t bytes_read;
            ssize_t bytes_written;

            bytes_read = read(client_server_fifo,&task_read,sizeof(Task));
            if(bytes_read == 0){ // eof do read porque não ha clientes a abrir extremo de escrita
                if(!queue_empty()){ // arranjar uma forma melhor de fazer esta verificação
                    task_executing = __schedule_get_task__();

                    pid_t pid = fork();
                    if(pid == 0){
                        __engine_execute_task(task_executing,outputPath,logFile_fd);
                        _exit(1);
                    }
                    close(server_client_fifo);
                    close(client_server_fifo);
                    continue;
                } else {
                    close(server_client_fifo);
                    close(client_server_fifo);
                    continue;
                }
            }
            if(bytes_read < 0){
                perror("Error reading task\n");
                close(server_client_fifo);
                close(client_server_fifo);
                break;
            }
            int id = __scheduler_add_task__(task_read);
            bytes_written = write(server_client_fifo,&id,sizeof(int));
            if(bytes_written <= 0){
                perror("Error reading task\n");
                close(server_client_fifo);
                close(client_server_fifo);
                break;
            }

            task_executing = __schedule_get_task__();
            __scheduler_remove_task__(task_executing);

            close(server_client_fifo);
            close(client_server_fifo);

            pid_t pid = fork();
            if(pid == 0){
                __engine_execute_task(task_executing,outputPath,logFile_fd);
                _exit(1);
            }
            num_tasks_executing++;
        } else{
            ssize_t bytes_read;
            ssize_t bytes_written;

            if(client_server_fifo == -1 || server_client_fifo == -1){
                close(server_client_fifo);
                close(client_server_fifo);
                break;
            }

            bytes_read = read(client_server_fifo,&task_read,sizeof(Task));

            if(bytes_read == 0){
                close(server_client_fifo);
                close(client_server_fifo);
                continue;
            }
            if(bytes_read < 0){
                perror("Error reading task\n");
                close(server_client_fifo);
                close(client_server_fifo);
                break;
            }
            int id = __scheduler_add_task__(task_read);

            bytes_written = write(server_client_fifo,&id,sizeof(int));
            if(bytes_written <= 0){
                perror("Error reading task\n");
                close(server_client_fifo);
                close(client_server_fifo);
                break;
            }

            close(server_client_fifo);
            close(client_server_fifo);
        }
    }
    return 0;
}
