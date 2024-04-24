#include "../include/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_NAME "task_fifo"
#define BUFFER_SIZE 300

int main(int argc, char* argv[]) {
   Task tarefa;

   if(argc < 2) {
      printf("Uso: %s execute time 'prog  args...'\n",argv[0]);
      return 1;
   
   } else if(strcmp(argv[1],"execute") == 0) {
      if(strcmp(argv[3],"-u") == 0){
         tarefa.tempo = atoi(argv[2]);
         strcpy(tarefa.programa,argv[4]);

         // solicitar a tarefa ao servidor
         int client_server_fifo = open(FIFO_NAME,O_WRONLY);
         write(client_server_fifo,&tarefa,sizeof(Task));
         close(client_server_fifo);


         //receber o identificador unico da tarefa
         int bytesRead = 0;
         int server_client_fifo = open(FIFO_NAME,O_RDONLY);
         int identificador;
         bytesRead = read(server_client_fifo,&identificador,sizeof(identificador));
         close(server_client_fifo);

         printf("Identificar único da tarefa: %d", identificador);
      }
       
   } else if(strcmp(argv[1],"status") == 0) {
      //enviar o status ao servidor
      int client_servidor_fifo = open(FIFO_NAME,O_WRONLY);
      write(client_servidor_fifo,"status",sizeof(argv[1]));
      close(client_servidor_fifo);

      //recebe e exibe o status da tarefa ao servidor

      int bytesRead;
      int servidor_cliente_fifo = open(FIFO_NAME,O_RDONLY);
      char buf[BUFFER_SIZE];
      while((bytesRead = read(servidor_cliente_fifo,buf,BUFFER_SIZE))>0) {
      write(STDOUT_FILENO,buf,bytesRead);
      }
      close(servidor_cliente_fifo);
      
      }
   }
