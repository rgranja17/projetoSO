#ifndef STATUS_H
#define STATUS_H

#include "../include/scheduler.h"

void __status_init_(int max_parallel_tasks);
void __status_add_task_(Task task_executing);
void __status_remove_task_(Task task_executed);
char* __status_get_executed_tasks_();
char* __status_get_executing_tasks_();
char* __status_get_schedule_tasks_();
char* __status_get_server_state();

#endif