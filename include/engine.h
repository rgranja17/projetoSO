#ifndef ENGINE_H
#define ENGINE_H

#include "../include/scheduler.h"

void __engine_execute_task(Task task_executing, char* outputPath, int logFile_fd);
void __engine_execute_pipeline(Task task_executing, char* outputPath, int logFile_fd);

#endif