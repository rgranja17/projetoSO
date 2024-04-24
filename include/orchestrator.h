#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H
#include <sys/types.h>
#define BUFFER_SIZE 1024

struct Task{
   pid_t id;
   char programa[100];
   int tempo; //ms
};

#endif