#ifndef CLIENT_H
#define CLIENT_H
#include <unistd.h>


typedef struct task{
   pid_t id;
   char programa[100];
   int tempo; //ms
} Task;

#endif
