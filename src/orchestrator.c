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

#define FIFO_NAME "task_fifo"

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Uso: %s <output-folder> <parallel-tasks>\n", argv[0]);
        return 1;
    }

    int parallel_tasks = atoi(argv[2]);
    Task* queue = malloc(parallel_tasks * sizeof(Task));
    int waiting_tasks = 0;

    if (parallel_tasks == 0) {
        printf("Introduza uma capacidade de pelo menos 1 tarefa paralela\n");
        return 1;
    }
    char *outputPath = strdup(argv[1]); // deve ser introduzido /logs"

    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        return 1;
    }

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

    printf("Server running...\n");

    Task* completed_tasks = malloc(100 * sizeof(Task));

    while (1) { // ciclo para a opção -u
        Task* pendingTasks;
        int num_completed_tasks = 0;
        Task tarefa_execute;
        Task tarefa_read;
        int server_fifo = open(FIFO_NAME, O_RDONLY);

        if (server_fifo == -1) {
            perror("Erro ao abrir ao pipe");
            return 1;
        }

        if ((read(server_fifo, &tarefa_read, sizeof(Task))) <= 0) {
            perror("Erro ao ler do fifo");
            return 1;
        }
        close(server_fifo);

    if (strcmp(tarefa_read.comando, "status") == 0) {
    server_fifo = open(FIFO_NAME, O_WRONLY);
    char status[4096];
    pendingTasks = getPendingTasks(queue, parallel_tasks, waiting_tasks);

    // Write executing task
    snprintf(status + strlen(status), sizeof(status) - strlen(status), "Executing:\n");
    snprintf(status + strlen(status), sizeof(status) - strlen(status), "%d %s\n", tarefa_read.id, tarefa_read.programa);

    // Write scheduled tasks
    snprintf(status + strlen(status), sizeof(status) - strlen(status), "Scheduled:\n");
    for(int j = 0; j < waiting_tasks && pendingTasks[j].id != -1; j++){
        snprintf(status + strlen(status), sizeof(status) - strlen(status), "%d %s\n", pendingTasks[j].id, pendingTasks[j].programa);
    }

    // Write completed tasks
    snprintf(status + strlen(status), sizeof(status) - strlen(status), "Completed:\n");
    for (int i = 0; i < num_completed_tasks; i++) {
        snprintf(status + strlen(status), sizeof(status) - strlen(status), "%d %s %d ms\n", completed_tasks[i].id, completed_tasks[i].programa, completed_tasks[i].tempo);
    }       

    if (write(server_fifo, status, strlen(status)) <= 0) {
        perror("Erro ao escrever status no fifo");
        break;
    }
    free(pendingTasks);

    close(server_fifo);

    continue;
} else {
        tarefa_read = add_task(tarefa_read, queue, &waiting_tasks, parallel_tasks); //adiciona tarefa lida à queue

        server_fifo = open(FIFO_NAME, O_WRONLY);

        char idMsg[20];
        snprintf(idMsg,sizeof(idMsg),"ID tarefa: %d\n", tarefa_read.id);

        if (write(server_fifo, idMsg, strlen(idMsg)) <= 0) {
            perror("Erro ao escrever tarefa no fifo");
            break;
        }

        close(server_fifo);

        tarefa_execute = getFaster(queue, parallel_tasks);

        remove_task(tarefa_execute,queue, num_completed_tasks, &waiting_tasks,
                    parallel_tasks, completed_tasks); // remove da fila de espera a tarefa feita

        if(strcmp(tarefa_execute.comando,"quit") == 0) break;



    
    if(strcmp(tarefa_execute.comando,"execute") == 0){

            char *aux = strdup(tarefa_execute.programa);
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

            pid_t pid = fork();
            if (pid == 0) {
                if (programa != NULL) {
                    tarefa_execute.pid = getpid();

                    char filename[10];
                    snprintf(filename,sizeof(filename), "Task%d.log", tarefa_execute.id);

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
            } else if (pid > 0) {
                int status;
                waitpid(pid, &status, 0);

                gettimeofday(&end, NULL);
                long runtime = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000; // em ms

                tarefa_execute.tempo = (int) runtime;

                char logMsg[1024];
                snprintf(logMsg, sizeof(logMsg), "\n-------\nPid: %d (LocalID: %d);Time: %d ms;Arguments: %s\n-----\n", tarefa_execute.pid,tarefa_execute.id,
                         tarefa_execute.tempo, tarefa_execute.programa);

                if((write(logFile_fd,&logMsg,strlen(logMsg))) < 0){
                    perror("Erro a escrever no ficheiro");
                    return 1;
                }

            } else {
                perror("Erro ao criar o processo filho");
            }
            free(aux);
        }
    }
    }
    
    free(completed_tasks);
    unlink (FIFO_NAME);
    printf("Server closed!\n");
    return 0;
}