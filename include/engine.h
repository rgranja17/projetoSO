#ifndef ENGINE_H
#define ENGINE_H

#include "../include/scheduler.h"

#define SERVER_CLIENT_FIFO "../tmp/server_client_fifo"
#define CLIENT_SERVER_FIFO "../tmp/client_server_fifo"

void __engine_execute_task(Task task_executing, char* outputPath, int logFile_fd);

#endif