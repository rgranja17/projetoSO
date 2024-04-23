#include "../include/orchestrator.h"
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
    if(argc < 3){
        printf("Uso: %s <output-folder> <parallel-tasks>");
        return 1;
    }

    if (access(FIFO_NAME, F_OK) == 0) {
        if (unlink(FIFO_NAME) == -1) {
            perror("Erro ao remover o FIFO");
            return 1;
        }
    }

    int parallel_tasks = atoi(argv[2]);
    char* output_folder = argv[1];

    Task tasks;

    if(mkfifo(FIFO_NAME,0666) == -1){
        perror("mkfifo");
        _exit(EXIT_FAILURE);
    }

    printf("Servidor à espera de tarefas...\n");

    int fifo_server;
    while(1){
        fifo_server = open(FIFO_NAME, O_RDONLY);
        if(read(fifo_server,&tasks,sizeof(Task)) <= 0) continue;
        close(fifo_server);

        if(strcmp(tasks.program,"status") == 0){
            printf("Status");
            //Fazer codigo do status
        }
        if(strcmp(tasks.program,"execute") == 0){ // tarefa de execução do programa
            struct timeval start, end;
            gettimeofday(&start,NULL);

            int res = -1;
            pid_t pid = fork();
            if(pid == 0){
                if (tasks.program_args == NULL) {
                    // Se não houver argumentos, passa apenas o nome do programa e NULL como último elemento
                    char *args[] = {tasks.program, NULL};
                    execvp(tasks.program, args);
                } else {
                    execvp(tasks.program, tasks.program_args);
                }
                _exit(res);
            }
            int status;
            wait(&status);

            if(WIFEXITED(status)){
                if(WEXITSTATUS(status) == 255) { // caso de erro!!
                    //codigo de erro para o ficheiro dos erros
                    printf("\nErro no processo!\n");
                } else {
                    res = WEXITSTATUS(status); // programa executado com sucesso
                    printf("\nTarefa feita\n");
                }
            }
        }
        if(strcmp(tasks.program,"quit") == 0) break;
    }
    unlink(FIFO_NAME);
    return 0;
} 
   
