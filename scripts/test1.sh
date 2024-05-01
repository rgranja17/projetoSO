#!/bin/bash

#Este script testa a execução de tarefas individuais de uma vez. 
#Cada tarefa é enviada separadamente para o servidor, permitindo testar a capacidade do servidor de lidar com várias tarefas concorrentes.
#operador & inicia cada script em segundo plano

cd ../bin
#inicia o servidor 
./orchestrator ../logs/ 10 &

# espera um pouco para o servidor inicializar completamente
sleep 1

#envia tarefas para o servidor (executa uma tarefa de cada vez)
./client execute 2 -u "ls -l" & #listar diretorio
./client execute 5 -u "ps -aux" & #de forma recursiva com detalhes
./client execute 7 -u "find ." & #busca por arquivos em todos os sub
./client execute 3 -u "uname -a" & #exibir informacoes do sistema7


#esperar um pouco para as tarefas serem processadas
sleep 5

./client status &

./client quit &