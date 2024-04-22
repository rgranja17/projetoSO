#include "../include/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define FIFO_NAME "task_fifo"

int main(int argc, char*argv[]) {
   if (argc < 4) {
      printf("Uso: %s <tempo> <prog-a> [args]\n",argv[0]);
      return 1;
   }
   // argumentos da linha de comandos
   int tempo = atoi(argv[1]);
   char *programa = argv[2];
   char *args = argv[3];

   mkfifo(FIFO_NAME,0666);

   int fd = open(FIFO_NAME,O_WRONLY);
   if(fd == -1) {
      perror("Erro ao abrir o pipe");
      return 1;
   }
   
   //Enviar solicitação ao servidor
   char mensagem[300];
   snprintf(mensagem,sizeof(mensagem), "%d %s %s", tempo,programa,args);
   write(fd,mensagem,strlen(mensagem));

   //receber resposta do servidor (identificador da tarefa)
   int identificador;
   read(fd,&identificador,sizeof(identificador));
   printf("Tarefa enviada. Identificador: %d\n",identificador);

   close(fd);
   return 0;
}