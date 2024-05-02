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
    char buf[BUFFER_SIZE];

    if(argc < 2) {
        printf("Insufficient arguments. Use the help command for more info\n");
        return 1;

    } else if(strcmp(argv[1],"execute") == 0) {
        if(strcmp(argv[3],"-u") == 0 || strcmp(argv[3],"-p") == 0){
            tarefa.time = atoi(argv[2]);
            strcpy(tarefa.command,argv[1]);

            strcpy(tarefa.program, argv[4]);
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

        }

    } else if(strcmp(argv[1],"status") == 0) {
        strcpy(tarefa.command,argv[1]);

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
        bytes_read = read(server_client_fifo,buf, sizeof(buf));

        buf[bytes_read] = '\0';

        bytes_written = write(STDOUT_FILENO,buf,strlen(buf));

        if(bytes_read <= 0){
            perror ("Erro escrita fifo\n");
            return 1;
        }
        if(bytes_written <= 0){
            perror("Erro escrita no STDOUT\n");
            return 1;
        }

        close(server_client_fifo);

    } else if(strcmp(argv[1],"quit") == 0 && strcmp(argv[2],"-a") == 0){
        strcpy(tarefa.command,argv[1]);
        strcpy(tarefa.flag,argv[2]);
        strcpy(tarefa.program, argv[3]);

        char buffer[50];

        int server_client_fifo = open(SERVER_CLIENT_FIFO,O_WRONLY);
        int bytes_written = write(server_client_fifo,&tarefa,sizeof(Task));
        close(server_client_fifo);

        server_client_fifo = open(SERVER_CLIENT_FIFO,O_RDONLY);
        int bytes_read = read(server_client_fifo,buffer,sizeof(buffer));
        close(server_client_fifo);
        buffer[bytes_read] = '\0';

        printf("\n%s\n",buffer);
    }
    else if(strcmp(argv[1],"help") == 0){
        strcpy(tarefa.command,argv[1]);

        char buffer[4096];

        int server_client_fifo = open(SERVER_CLIENT_FIFO,O_WRONLY);
        int bytes_written = write(server_client_fifo,&tarefa,sizeof(Task));
        close(server_client_fifo);

        server_client_fifo = open(SERVER_CLIENT_FIFO,O_RDONLY);
        int bytes_read = read(server_client_fifo,buffer,sizeof(buffer));
        close(server_client_fifo);
        buffer[bytes_read] = '\0';

        if(bytes_read <= 0){
            perror ("Erro escrita fifo\n");
            return 1;
        }
        if(bytes_written <= 0) {
            perror("Erro escrita no STDOUT\n");
        }

        printf("\n%s\n",buffer);

    }
    return 0;
}