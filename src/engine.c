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



void fillArray(char *array[],char *command, int* num_pipelines) {
    const char *delim = "|";
    char *token;
    int index = 0;

    // Get the first token
    token = strtok(command, delim);

    // Walk through other tokens
    while (token != NULL && index < MAX_PIPELINES) {
        // Remove leading and trailing whitespace
        while (*token == ' ')
            token++;
        int len = strlen(token);
        while (len > 0 && token[len - 1] == ' ')
            token[--len] = '\0';

        // Allocate memory for the token and copy it to the array
        array[index] = (char *)malloc((strlen(token) + 1) * sizeof(char));
        strcpy(array[index], token);

        // Get the next token
        token = strtok(NULL, delim);
        index++;
        (*num_pipelines)++;
    }

    // Fill remaining slots with NULL
    while (index < MAX_PIPELINES) {
        array[index] = NULL;
        index++;
    }
}


Task __engine_execute_task(Task task_executing,int logFile_fd){
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
        dup2(logFile_fd, STDOUT_FILENO);
        dup2(logFile_fd,STDERR_FILENO);
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
    snprintf(output_message, sizeof(output_message), "\nPid: %d (LocalID: %d);Time: %d ms;Arguments: %s\n-----\n\n",
             task_executing.pid,task_executing.id,
             task_executing.time, task_executing.program);

    ssize_t bytes_written = write(logFile_fd,&output_message,strlen(output_message));
    if(bytes_written <= 0){
        perror("Error writing on log file");
        _exit(0);
    }

    free(aux);
    return task_executing;
}

Task __engine_execute_pipeline(Task task_executing, int logFile_fd) {
    char* pipe_programs[MAX_PIPELINES];
    char* task_program_cpy = strdup(task_executing.program);
    int num_pipelines = 0;

    fillArray(pipe_programs,task_program_cpy,&num_pipelines);

    int fd[num_pipelines - 1][2];

    struct timeval start, end;
    gettimeofday(&start, NULL);

    for (int i = 0; i < num_pipelines; ++i) {
        char *aux = strdup(pipe_programs[i]);
        char *token = strtok(aux, " ");
        char *programa = token;
        char *argumentos[11];

        int j = 0;
        while (token != NULL && j < 10) {
            argumentos[j] = token;
            token = strtok(NULL, " ");
            j++;
        }
        argumentos[j] = NULL;

        if (i == 0) {
            pipe(fd[0]);
            if(fork() == 0) {

                dup2(fd[0][1], STDOUT_FILENO);
                dup2(logFile_fd,STDERR_FILENO);
                close(fd[0][0]);
                close(fd[0][1]);

                execvp(programa, argumentos);

                perror("Exec failed");
                _exit(1);
            }
            close(fd[0][1]);

        } else if (i < num_pipelines - 1) { // meio
            pipe(fd[i]);
            if(fork() == 0) {

                dup2(fd[i - 1][0], STDIN_FILENO);
                dup2(logFile_fd,STDERR_FILENO);
                close(fd[i - 1][0]);

                dup2(fd[i][1], STDOUT_FILENO);
                close(fd[i][0]);
                close(fd[i][1]);

                execvp(programa, argumentos);

                perror("Exec failed");
                _exit(1);
            }
            close(fd[i - 1][0]);
            close(fd[i][1]);


        } else if(i == num_pipelines - 1){ // ultimo
            if(fork() == 0) {

                dup2(fd[i - 1][0], STDIN_FILENO);
                dup2(logFile_fd,STDOUT_FILENO);
                dup2(logFile_fd,STDERR_FILENO);
                close(fd[i - 1][1]);
                close(fd[i - 1][0]);

                execvp(programa, argumentos);

                perror("Exec failed");
                _exit(1);
            }
            close(fd[i - 1][0]);
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
    snprintf(output_message, sizeof(output_message), "\nPid: %d (LocalID: %d);Time: %d ms;Arguments: %s\n---------------------\n\n",
             task_executing.pid, task_executing.id,
             task_executing.time, task_executing.program);

    ssize_t bytes_written = write(logFile_fd, &output_message, strlen(output_message));
    if (bytes_written <= 0) {
        perror("Error writing on log file");
        _exit(0);
    }

    return task_executing;
}