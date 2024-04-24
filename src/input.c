#include "../include/input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Input* parse(char** argv, int argc){ // parse a funcionar
    Input* input = malloc(sizeof(Input));

    if(argc < 2) return NULL;
    if(argc == 2 && (strcmp(argv[1],"status") == 0) || (strcmp(argv[1],"quit") == 0)){
        input->command = strdup(argv[1]);
        input->time = 0;
        input->prog_arguments = NULL;
        input->prog_num_arguments = 0;
        input->flag = NULL;
        return input;
    }
    if(strcmp(argv[1],"execute") == 0){
        input->command = strdup(argv[1]);
        input->time = atoi(argv[2]);
        if(input->time == 0) return NULL;
        if((strcmp(argv[3],"-u") == 0) || (strcmp(argv[3],"-p") == 0)){
            input->flag = strdup(argv[3]);
        } else {
            return NULL;
        }
        char* aux = strdup(argv[4]);
        input->prog_num_arguments = 0;

        // Conta o número de argumentos do programa
        char* token;
        while((token = strsep(&aux, " ")) != NULL){
            input->prog_num_arguments++;
        }

        // Aloca memória para os argumentos do programa
        input->prog_arguments = malloc((input->prog_num_arguments +1 ) * sizeof(char*));
        // Copia os argumentos do programa

        char* aux2 = strdup(argv[4]);
        token = strsep(&aux2, " ");
        input->prog_arguments[0] = strdup(token);
        for(int i = 1; i < input->prog_num_arguments; i++){
            input->prog_arguments[i] = strsep(&aux2, " ");
        }
        free(aux);
        free(aux2);
        return input;
    }
    return NULL;
}

void freeInput(Input* input){
    free(input);
}
