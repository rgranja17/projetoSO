#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H
#include <sys/types.h>
#define BUFFER_SIZE 1024

typedef struct task{
   pid_t id;
   char programa[100];
   int tempo; //ms
}Task;

#endif