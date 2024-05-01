#!/bin/bash

#Este script testa a execução de tarefas em pipeline. Uma única tarefa envolvendo um pipeline de comandos é enviada para o servidor.

#inicia o servidor 
./orchestrator ../logs/ 10

sleep 1

#envia tarefas para o servidor (executa tarefas em pipeline)
./client execute 30 -p "ls -l | grep root | wc -l" 

./client status

sleep 5

./client status

./client quit