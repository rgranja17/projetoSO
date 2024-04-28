#include "../include/orchestrator.h"
#include "../include/task.h"
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

#define FIFO_NAME "task_fifo"
#define INTERNAL_FIFO "internal_fifo"

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Uso: %s <output-folder> <parallel-tasks>\n", argv[0]);
        return 1;
    }
    int parallel_tasks = atoi(argv[2]);

    if (parallel_tasks == 0) {
        printf("Introduza uma capacidade de pelo menos 1 tarefa paralela\n");
        return 1;
    }

    char *outputPath = strdup(argv[1]); // deve ser introduzido /logs"
    int server_status = 1;
    int waiting_tasks;
    initQueue();

    char outputFileLog[256];
    snprintf(outputFileLog, sizeof(outputFileLog), "%s/Tasks.log", argv[1]);

    // Abertura do arquivo de log
    int logFile_fd = open(outputFileLog, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (logFile_fd == -1) {
        perror("Erro ao abrir/criar o arquivo de log");
        return 1;
    }
    char header[] = "ID,Program,Arguments,Runtime\n";
    if (write(logFile_fd, header, strlen(header)) <= 0) {
        printf("Erro ao escrever no ficheiro log\n");
        return 1;
    }

    if (mkfifo(INTERNAL_FIFO, 0666) == -1) {
        if(errno != EEXIST){
            perror("Erro criação fifo");
        }
    }

    int internal_fifo = open(INTERNAL_FIFO, O_RDWR);
    if (internal_fifo == -1) {
        perror("Erro ao abrir internal_fifo");
        return 1;
    }
    write(internal_fifo, &server_status, sizeof(int));

    printf("Server running...\n");
    while(1){
        Task task_executing;
        waiting_tasks = getWaitingTasks();

        read(internal_fifo, &server_status, sizeof(int));

        //printf("Server status: %d\n", server_status);
        if(server_status && waiting_tasks > 0){
            task_executing = getFaster();
            remove_task(task_executing);

            server_status = 0;
            write(internal_fifo, &server_status, sizeof(int));
        }else {
            write(internal_fifo, &server_status, sizeof(int));
            continue;
        }

        int status;
        pid_t pid = fork();
        if(pid == 0){
            if(strcmp(task_executing.comando,"execute") == 0){
                printf("Executar tarefa %d\n",task_executing.id);
                char *aux = strdup(task_executing.programa);
                char *token = strtok(aux, " ");
                char *programa = token;
                char *argumentos[11]; // 10 argumentos + 1 para NULL

                int i = 0;
                while (token != NULL && i < 10) {
                    argumentos[i] = token;
                    token = strtok(NULL, " ");
                    i++;
                }
                argumentos[i] = NULL; // Terminate the argument list with NULL
                struct timeval start, end;
                gettimeofday(&start, NULL);

                pid_t pid1 = fork();
                if(pid1 == 0){
                    if (programa != NULL) {
                        printf("A fazer tarefa %d\n",task_executing.id);
                        char filename[10];
                        snprintf(filename,sizeof(filename), "Task%d.log", task_executing.id);

                        char* outputFile = malloc(sizeof(char*) * (sizeof(outputPath) + sizeof(filename)));
                        strcpy(outputFile,outputPath);
                        strcat(outputFile,filename);

                        int outputFile_fd = open(outputFile, O_WRONLY | O_CREAT | O_APPEND, 0666);
                        if (!outputFile_fd) {
                            perror("Erro ao abrir tasks.log");
                            return 1;
                        }
                        dup2(outputFile_fd, STDOUT_FILENO);
                        execvp(programa, argumentos);
                    }
                    perror("Erro ao executar o programa");
                    _exit(1);
                }
                waitpid(pid, &status, 0);

                gettimeofday(&end, NULL);
                long runtime = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000; // em ms

                task_executing.tempo = (int) runtime;
                task_executing.pid = getpid();

                char logMsg[1024];
                snprintf(logMsg, sizeof(logMsg), "\n-------\nPid: %d (LocalID: %d);Time: %d ms;Arguments: %s\n-----\n", task_executing.pid,task_executing.id,
                         task_executing.tempo, task_executing.programa);

                if((write(logFile_fd,&logMsg,strlen(logMsg))) < 0){
                    perror("Erro a escrever no ficheiro");
                    return 1;
                }

                printf("Tarefa %d feita\n", task_executing.id);
                free(aux);

                int new_status = 1;
                write(internal_fifo, &new_status, sizeof(int));
                _exit(1);
            }
        }

    }
    close(internal_fifo);
    freeQueue();
    printf("Server closed!\n");
    return 0;
}
