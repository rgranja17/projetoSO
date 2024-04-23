#ifndef INCLUDE_H
#define INCLUDE_H

typedef struct input{
    int time; // em ms
    char* flag; // flag
    char* command; // execute ou status
    char* program; // nome do programa a correr
    char** prog_arguments; // argumentos do programa a correr
    int prog_num_arguments; // nº argumentos do programa a correr
}Input;

Input* parse(char** argv, int argc);
void freeInput(Input* input);

#endif
