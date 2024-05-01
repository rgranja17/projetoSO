#include "../include/engine.h"
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
#include <time.h>
#include <errno.h>

#define MAX_PIPELINES 10
#define MAX_PROGRAM_ARGS 11

void __engine_execute_task(Task task_executing, char* outputPath, int logFile_fd){
    char *aux = strdup(task_executing.program);
    char *token = strtok(aux, " ");
    char *programa = token;
    char *argumentos[11];

    int i = 0;
    while (token != NULL && i < 10) {
        argumentos[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    argumentos[i] = NULL;

    struct timeval start, end;
    gettimeofday(&start, NULL);

    int status;
    pid_t pid = fork();
    if(pid == 0){
        char filename[10];
        snprintf(filename,sizeof(filename), "Task%d.log", task_executing.id);

        char* outputFile = malloc(sizeof(char*) * (sizeof(outputPath) + sizeof(filename)));
        strcpy(outputFile,outputPath);
        strcat(outputFile,filename);

        int outputFile_fd = open(outputFile, O_WRONLY | O_CREAT | O_APPEND, 0666);
        if (!outputFile_fd) {
            perror("Erro ao abrir tasks.log");
            _exit(0);
        }
        dup2(outputFile_fd, STDOUT_FILENO);
        execvp(programa, argumentos);
        perror("Erro ao executar o programa");
        _exit(1);
    }
    waitpid(pid,&status,0);

    gettimeofday(&end, NULL);
    long runtime = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000; // em ms

    task_executing.time = (int) runtime;
    task_executing.pid = getpid();

    char output_message[1024];
    snprintf(output_message, sizeof(output_message), "\n-------\nPid: %d (LocalID: %d);Time: %d ms;Arguments: %s\n-----\n",
             task_executing.pid,task_executing.id,
             task_executing.time, task_executing.program);

    ssize_t bytes_written = write(logFile_fd,&output_message,strlen(output_message));
    if(bytes_written <= 0){
        perror("Error writing on log file");
        _exit(0);
    }

    free(aux);
}

void __engine_execute_pipeline(Task task_executing, char* outputPath, int logFile_fd) {
    char* arguments[MAX_PIPELINES][MAX_PROGRAM_ARGS]; // Array para armazenar argumentos para cada programa
    char* program_names[MAX_PIPELINES]; // Array para armazenar nomes de programas
    int num_pipelines = 0;

    char* token1;
    char *aux = strdup(task_executing.program);
    token1 = strtok(aux,"|");

    while (token1 != NULL && num_pipelines < MAX_PIPELINES) {
        char *token = strtok(token1, " ");
        program_names[num_pipelines] = token; // Armazena o nome do programa

        int i = 0;
        while (token != NULL && i < MAX_PROGRAM_ARGS) {
            arguments[num_pipelines][i] = token; // Armazena os argumentos do programa
            token = strtok(NULL, " ");
            i++;
        }
        arguments[num_pipelines][i] = NULL; // Marca o final dos argumentos
        token1 = strtok(NULL, "|");
        num_pipelines++;
    }

    char filename[15];
    snprintf(filename, sizeof(filename), "Task%d.log", task_executing.id);

    char* outputFile = malloc(strlen(outputPath) + strlen(filename) + 1);
    strcpy(outputFile, outputPath);
    strcat(outputFile, filename);

    int outputFile_fd = open(outputFile, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if (outputFile_fd < 0) {
        perror("Erro ao abrir arquivo de saída");
        exit(EXIT_FAILURE);
    }

    int fd[MAX_PIPELINES - 1][2];

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < num_pipelines; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i == 0) { // primeiro
                pipe(fd[0]);
                dup2(fd[0][1], 1);
                close(fd[0][0]);
                close(fd[0][1]);

            }
            if (i < num_pipelines - 1 && i > 0) { // meio
                pipe(fd[i]);

                dup2(fd[i - 1][0], 0);
                close(fd[i - 1][0]);

                dup2(fd[i][1], 1);
                close(fd[i][0]);
                close(fd[i][1]);
            } else { // ultimo
                pipe(fd[num_pipelines-1]);
                dup2(fd[num_pipelines - 1][0], 0);
                dup2(outputFile_fd, 1);
                close(fd[num_pipelines - 1][1]);
                close(fd[num_pipelines - 1][0]);
            }

            execvp(program_names[i], arguments[i]);
            perror("Erro ao executar o programa");
            _exit(1);
        }
    }
    for (int i = 0; i < num_pipelines; i++) {
        wait(NULL);
    }

    gettimeofday(&end, NULL);
    long runtime = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000; // em ms

    task_executing.time = (int) runtime;
    task_executing.pid = getpid();

    char output_message[1024];
    snprintf(output_message, sizeof(output_message), "\n-------\nPid: %d (LocalID: %d);Time: %d ms;Arguments: %s\n-----\n",
             task_executing.pid, task_executing.id,
             task_executing.time, task_executing.program);

    ssize_t bytes_written = write(logFile_fd, &output_message, strlen(output_message));
    if (bytes_written <= 0) {
        perror("Error writing on log file");
        _exit(0);
    }

    free(aux);
    free(outputFile);
}