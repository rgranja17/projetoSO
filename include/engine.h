#ifndef ENGINE_H
#define ENGINE_H

#include "../include/scheduler.h"

Task __engine_execute_task(Task task_executing,int logFile_fd);
Task __engine_execute_pipeline(Task task_executing,int logFile_fd);

#endif