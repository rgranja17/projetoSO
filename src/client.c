#include "../include/client.h"
#include "../include/task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_NAME "task_fifo"
#define BUFFER_SIZE 4096

int main(int argc, char* argv[]) {
    Task tarefa;
    char buf[BUFFER_SIZE];

    if(argc < 2) {
        printf("Uso: %s execute time 'prog  args...'\n",argv[0]);
        return 1;

    } else if(strcmp(argv[1],"execute") == 0) {
        if(strcmp(argv[3],"-u") == 0){
            tarefa.tempo = atoi(argv[2]);
            strcpy(tarefa.comando,argv[1]);
            strcpy(tarefa.programa,argv[4]);
            strcpy(tarefa.flag,argv[3]);

            write_fifo(tarefa);
            printf("escrita fifo\n");

            int id = add_task();
            printf("ID da tarefa: %d\n",id);

        } if(strcmp(argv[3],"-p") == 0){
            printf("Flag -p ainda não disponivel");
            return 1;
        }

    } else if(strcmp(argv[1],"status") == 0) {
        // para fazer

    } else if(strcmp(argv[1],"quit") == 0){

        // para fazer
    }

    return 0;
}