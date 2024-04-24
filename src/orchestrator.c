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

int main() {
   
   //se ainda nao existir cria o pipe
   mkfifo(FIFO_NAME, 0666);

   int fd = open(FIFO_NAME,O_RDONLY);
   if (fd == -1) {
      perror("Erro ao abrir ao pipe");
      return 1;
   }

   int logFile = open("/logs/tasks.log", O_WRONLY | O_CREAT | O_APPEND,0666);
   if(!logFile) {
      perror("Erro ao abrir tasks.log");
      close(fd);
      return 1;
   }
   char header[] = "ID,Program,Arguments,Runtime\n";
   write(logFile,header,strlen(header));


   struct Task tasks[300];
   int num_tasks = 0;

   while(1) {
      char mensagem[300];
      if(read(fd,mensagem,sizeof(mensagem)) <= 0){
         continue;
      }

      int tempo;
      char programa[100], argumentos[100];
      sscanf(mensagem, "%d %s %s",&tempo,programa,argumentos);

      struct timeval start, end;
      gettimeofday(&start,NULL);


      //criar processo filho para executar o programa
      pid_t pid = fork();
      if(pid == 0) {
         execlp(programa,programa,argumentos,NULL);
         perror("Erro ao executar o programa.");
         _exit(1);
      } else if (pid > 0) {
         int status;
         waitpid(pid,&status,0); //espera o processo filho terminar

         gettimeofday(&end,NULL);
         long runtime = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000; // em ms

         fprintf(logFile, "%d,%s,%s,%ld\n",pid,programa,argumentos,runtime);
         fflush(logFile);
         
         tasks[num_tasks].id = pid;
         strcpy(tasks[num_tasks].programa,programa);
         tasks[num_tasks].tempo = tempo;
         num_tasks++;

         //nao esquecer de enviar o identificador da tarefa de volta para o cliente
         write(fd,&pid,sizeof(pid));

         } else {
         perror("Erro ao criar o processo filho");
      }

   }
/*  ESTA IMPLEMENTACAO É QUANDO TIVERMOS VARIAS TAREFAS, TEMOS APENAS PARA 1 A FUNCIONAR
   //aguardar processos filhos
   for(int i = 0; i<num_tasks;i++) {
     int status;
     pid_t filho_pid = waitpid(tasks[i].id,&status,0);
     if (filho_pid == -1) {
      perror("Erro na espera pelo processo filho");
     } else {
      if(WIFEXITED(status)) {
         int exitStatus = WEXITSTATUS(status); // obter o codigo de saida do filho 
         //calcula o tempo da execucao da tarefa (se necessario)
         //registrar informacoes da tarefa concluida no arquivo log
         fprintf(logFile, "Tarefa Concluída - ID: %d, Programa: %s, Tempo de execução: %d ms, Código de saída: %d\n", tasks[i].id,tasks[i].programa,tasks[i].tempo,exitStatus);
         fflush(logFile);
      } else {
         //procesos filho terminou de forma anormal (zombie ex)
         //registrar uma mensagem de erro no log
         fprintf(logFile, "Erro: Tarefa com ID %d terminou de forma anormal.\n", tasks[i].id);
         fflush(logFile);
         }
      }
      
   }
   fclose(logFile);
*/
   return 0;   
}