CC = gcc
CFLAGS = -Wall -g -I$(INCLUDE_DIR)
LDFLAGS =
TARGETS = $(BIN_DIR)/orchestrator $(BIN_DIR)/client
OBJ_DIR = obj
INCLUDE_DIR = include
BIN_DIR = bin
LOGS_DIR = logs
SRC_DIR = src

all: folders $(TARGETS)

folders:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(LOGS_DIR) $(INCLUDE_DIR) $(SRC_DIR)

$(BIN_DIR)/orchestrator: $(OBJ_DIR)/orchestrator.o
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN_DIR)/client: $(OBJ_DIR)/client.o
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/* $(LOGS_DIR)/*

log:
	@echo "Logging..." > $(LOGS_DIR)/tasks.log

