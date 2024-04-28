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

            // solicitar a tarefa ao servidor
            int client_server_fifo = open(FIFO_NAME,O_WRONLY);
            write(client_server_fifo,&tarefa,sizeof(Task));
            close(client_server_fifo);


            //receber o pid da tarefa
            int bytesRead = 0;
            int client_fifo = open(FIFO_NAME,O_RDONLY);
            bytesRead = read(client_fifo,buf,sizeof(buf));
            buf[bytesRead] = '\0';

            write(STDOUT_FILENO,buf,bytesRead);
            close(client_fifo);

        } if(strcmp(argv[3],"-p") == 0){
            printf("Flag -p ainda não disponivel");
            return 1;
        }

    } else if(strcmp(argv[1],"status") == 0) {
    strcpy(tarefa.comando,argv[1]);
    tarefa.tempo = 0;

    int client_servidor_fifo = open(FIFO_NAME,O_WRONLY);
    write(client_servidor_fifo,&tarefa,sizeof(Task));
    close(client_servidor_fifo);


    int bytesRead = 0;
    int client_fifo = open(FIFO_NAME,O_RDONLY);
    bytesRead = read(client_fifo,buf,sizeof(buf));
    buf[bytesRead] = '\0';

    write(STDOUT_FILENO,buf,bytesRead);
    close(client_fifo);

    } else if(strcmp(argv[1],"quit") == 0){
        strcpy(tarefa.comando,argv[1]);
        tarefa.tempo = 0;

        int client_servidor_fifo = open(FIFO_NAME,O_WRONLY);
        write(client_servidor_fifo,&tarefa,sizeof(Task));
        close(client_servidor_fifo);

    }

    return 0;
}