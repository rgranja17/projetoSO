#include "../include/input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Input* parse(char** argv, int argc){
    Input* input = malloc(sizeof(Input));

    if(argc < 2) return NULL;
    if(argc == 2 && (strcmp(argv[1],"status") == 0 || strcmp(argv[1],"quit") == 0)){
        input->command = strdup(argv[1]);
        input->time = 0;
        input->prog_arguments = malloc(sizeof(char*));
        input->prog_arguments[0] = strdup(argv[1]);
        input->prog_num_arguments = 1;
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
        char* token1 = strsep(&aux, " ");
        if (token1 == NULL || strcmp(token1, "") == 0) {
            return NULL;
        }
        input->program = strdup(token1);

        input->prog_num_arguments = 1;
        char* token;
        while((token = strsep(&aux, " ")) != NULL){
            input->prog_num_arguments++;
        }

        input->prog_arguments = malloc((input->prog_num_arguments + 1) * sizeof(char*));

        // Copia os argumentos do programa
        char* aux2 = strdup(argv[4]);
        input->prog_arguments[0] = strdup(token1); // O primeiro argumento é o nome do programa
        for(int i = 1; i < input->prog_num_arguments; i++){
            input->prog_arguments[i] = strsep(&aux2, " ");
        }
        input->prog_arguments[input->prog_num_arguments] = NULL; // Terminador nulo no final do array

        free(aux);
        free(aux2);
        return input;
    }
    return NULL;
}



void freeInput(Input* input){
    if (input == NULL) return;

    if (input->flag != NULL) {
        free(input->flag);
        input->flag = NULL;
    }
    if (input->command != NULL) {
        free(input->command);
        input->command = NULL;
    }
    if (input->program != NULL) {
        free(input->program);
        input->program = NULL;
    }
    if (input->prog_arguments != NULL) {
        for (int i = 0; i < input->prog_num_arguments; i++) {
            if (input->prog_arguments[i] != NULL) {
                free(input->prog_arguments[i]);
                input->prog_arguments[i] = NULL;
            }
        }
        free(input->prog_arguments);
        input->prog_arguments = NULL;
    }
    free(input);
}

