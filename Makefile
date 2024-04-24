CC = gcc
CFLAGS = -Iinclude -Wall -pedantic -O2 -g
LDFLAGS =
TARGETS = orchestrator client
OBJ_DIR = obj
BIN_DIR = bin
SRC_DIR = src
TMP_DIR = tmp

all: folders $(TARGETS)

orchestrator: $(BIN_DIR)/orchestrator
client: $(BIN_DIR)/client

folders:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(TMP_DIR)

$(BIN_DIR)/orchestrator: $(OBJ_DIR)/orchestrator.o
	$(CC) $(LDFLAGS) $^ -o $@

$(BIN_DIR)/client: $(OBJ_DIR)/client.o $(OBJ_DIR)/input.o
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/*
