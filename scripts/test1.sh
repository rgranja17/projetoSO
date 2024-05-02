#!/bin/bash

#Este script testa a execução de tarefas individuais de uma vez.
#Cada tarefa é enviada separadamente para o servidor, permitindo testar a capacidade do servidor de lidar com várias tarefas concorrentes.
#operador & inicia cada script em segundo plano

cd ../bin

#envia tarefas para o servidor (executa uma tarefa de cada vez)
./client execute 2 -u "ls -l" & #listar diretorio
sleep 0.25
./client execute 5 -u "ps -aux" & #de forma recursiva com detalhes
sleep 0.25
./client execute 12 -u "../progs-TP23_24/void 12" & #executar programa teste void
sleep 0.25
./client execute 7 -u "find ." & #busca por arquivos em todos os sub
sleep 0.25
./client execute 3 -u "uname -a" & #exibir informacoes do sistema7
sleep 0.25
./client execute 13 -u "../progs-TP23_24/hello 12" & #executar programa teste hello
sleep 0.25

#esperar um pouco para as tarefas serem processadas
sleep 5

./client status &
