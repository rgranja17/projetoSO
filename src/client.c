#include "../include/client.h"
#include "../include/scheduler.h"
#include "../include/engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFFER_SIZE 4096

#define SERVER_CLIENT_FIFO "../tmp/server_client_fifo"

int main(int argc, char* argv[]) {
    Task tarefa;
    //char buf[BUFFER_SIZE];

    if(argc < 2) {
        printf("Uso: %s execute time 'prog  args...'\n",argv[0]);
        return 1;

    } else if(strcmp(argv[1],"execute") == 0) {
        if(strcmp(argv[3],"-u") == 0){
            tarefa.time = atoi(argv[2]);
            strcpy(tarefa.command,argv[1]);
            strcpy(tarefa.program,argv[4]);
            strcpy(tarefa.flag,argv[3]);
            int id;

            int server_client_fifo = open(SERVER_CLIENT_FIFO, O_WRONLY);
            ssize_t bytes_written;
            bytes_written = write(server_client_fifo,&tarefa,sizeof(Task));
            if(bytes_written <= 0){
                perror ("Erro escrita fifo\n");
                return 1;
            }
            close(server_client_fifo);
            
            server_client_fifo = open(SERVER_CLIENT_FIFO, O_RDONLY);
            ssize_t bytes_read;
            bytes_read = read(server_client_fifo,&id, sizeof(int));
            if(bytes_read <= 0){
                perror ("Erro escrita fifo\n");
                return 1;
            }

            close(server_client_fifo);

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