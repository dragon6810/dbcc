CC := clang -c
CC_FLAGS := -D_POSIX_C_SOURCE=199309L -Wall -O0 -g -std=c11 -MMD -MP -fsanitize=address

LD := clang
LD_FLAGS := -fsanitize=address

BIN_DIR := bin
OBJ_DIR := obj
SRC_DIR := src

BIN_FILE := $(BIN_DIR)/small-mnist-classifier
SRC_FILE := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILE := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILE))
CC_FLAGS := $(CC_FLAGS) -I$(SRC_DIR)

.PHONY: all mkdirs clean

all: mkdirs $(BIN_FILE)

mkdirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)

$(BIN_FILE): $(OBJ_FILE)
	$(LD) -o $(BIN_FILE) $(OBJ_FILE) $(LD_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@

-include $(OBJ_FILE:.o=.d)

clean:
	rm -r $(OBJ_DIR)
	rm -r $(BIN_DIR)