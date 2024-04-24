#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

typedef struct Task {
    int pid;
    int time;
    char* command;
    char** program_args;
    int prog_num_args;
} Task;

typedef struct answer {
    int pid;
    char* result;
} Answer;

#endif