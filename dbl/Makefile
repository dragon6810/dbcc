CC := gcc -c
CC_FLAGS := -Wall -g

LD := gcc
LD_FLAGS :=

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

SRC_FILE := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILE := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILE))
BIN_FILE := $(BIN_DIR)/dbl

.PHONY: all mkdirs clean

all: mkdirs $(BIN_FILE)

mkdirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

$(BIN_FILE): $(OBJ_FILE)
	$(LD) $(OBJ_FILE) -o $(BIN_FILE)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CC_FLAGS) $< -o $@

clean:
	rm -r $(OBJ_DIR)
	rm -r $(BIN_DIR)
