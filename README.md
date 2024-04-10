Orquestrador de Tarefas

Este é um projeto de um serviço de orquestração de tarefas em um computador, desenvolvido como parte de um trabalho acadêmico para a disciplina de Sistemas Operativos.
Funcionalidades Principais:

    Execução de tarefas do usuário, individualmente ou em pipelines.
    Escalonamento e execução de tarefas pelo servidor.
    Redirecionamento de saída para arquivos correspondentes.
    Consulta de tarefas em execução.

Estrutura do Projeto:

    src/: Contém os arquivos-fonte do projeto.
    include/: Opcionalmente, contém arquivos de cabeçalho.
    obj/: Armazena os arquivos objetos gerados durante a compilação.
    bin/: Contém os executáveis gerados após a compilação.
    Makefile: Arquivo de script para compilar e construir o projeto.

Como Executar:

    Compile o servidor:
    $ make orchestrator

    Compile o cliente:  
    $ make client

Configurações Adicionais:

    O arquivo de Makefile pode ser ajustado conforme necessário para incluir outras dependências ou realizar outras operações específicas.
    Certifique-se de ter todas as bibliotecas necessárias instaladas no seu sistema para compilar o projeto corretamente.

Autor:
Rodrigo Miguel Granja Ferreira 


