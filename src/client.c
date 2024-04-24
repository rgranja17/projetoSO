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

    /*
     */
    if (userInput == NULL) {
        printf("Uso: %s execute <tempo> <flag> <prog-a> [args]\n", argv[0]);
        printf("Uso: %s status\n", argv[0]);
        return 1; // Retorna 1 se a entrada estiver incorreta
    }

    Task task;
    Answer ans;

    task.time = userInput->time;
    task.command = userInput->command;
    task.program_args = userInput->prog_arguments;
    task.prog_num_args = userInput->prog_num_arguments;

    mkfifo(FIFO_NAME,0666);

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
    close(user_fifo);

    user_fifo = open(FIFO_NAME,O_RDONLY);
    read(user_fifo,&ans,sizeof(Answer));
    printf("%s",ans.result);

    close(user_fifo);

    freeInput(userInput);

    return 0;
}