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


void __engine_execute_task(Task task_executing, char* outputPath, int logFile_fd){
    char *aux = strdup(task_executing.program);
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