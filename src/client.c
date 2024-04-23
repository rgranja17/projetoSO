#include "../include/client.h"
#include "../include/input.h"
#include "../include/orchestrator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_NAME "task_fifo"

int main(int argc, char*argv[]) {

    Input* userInput = parse(argv, argc);
    if (userInput == NULL) {
        printf("Uso: %s execute <tempo> <flag> <prog-a> [args]\n", argv[0]);
        printf("Uso: %s status\n", argv[0]);
        return 1; // Retorna 1 se a entrada estiver incorreta
    }

    Task task;
    task.program = userInput->program;
    task.time = userInput->time;
    task.program_args = userInput->prog_arguments;
    task.prog_num_args = userInput->prog_num_arguments;

    int user_fifo = open(FIFO_NAME, O_WRONLY);

    if (user_fifo < 0) {
        printf("Erro ao abrir o fifo do cliente\n");
        freeInput(userInput);
        return 1;
    }

    if (write(user_fifo, &task, sizeof(task)) < 0) {
        printf("Erro ao enviar a tarefa\n");
        freeInput(userInput);
        close(user_fifo);
        return 1;
    }

    freeInput(userInput);
    close(user_fifo);
    return 0;
}