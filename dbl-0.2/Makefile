CC := clang -c
CC_FLAGS := -D_POSIX_C_SOURCE=200809L -Wall -Werror -O3 -g -std=gnu89 -MMD -MP -fsanitize=address

LD := clang
LD_FLAGS := -fsanitize=address

BIN_DIR := bin
OBJ_DIR := obj
SRC_DIR := src

BIN_FILE := $(BIN_DIR)/dbc
SRC_FILE := $(shell find $(SRC_DIR) -name '*.c')
OBJ_FILE := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILE))
CC_FLAGS := $(CC_FLAGS) -I$(SRC_DIR)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	CC_FLAGS := $(CC_FLAGS) -D LINUX
endif
ifeq ($(UNAME_S), Darwin)
	CC_FLAGS := $(CC_FLAGS) -D APPLE
endif
UNAME_P := $(shell uname -p)
ifeq ($(UNAME_P),x86_64)
	CC_FLAGS := $(CC_FLAGS) -D AMD64
endif
ifneq ($(filter %86,$(UNAME_P)),)
	CC_FLAGS := $(CC_FLAGS) -D IA32
endif
ifneq ($(filter arm%,$(UNAME_P)),)
	CC_FLAGS := $(CC_FLAGS) -D ARM
endif

.PHONY: all mkdirs clean preprocessed

all: mkdirs $(BIN_FILE)
preprocessed: mkdirs $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.i, $(SRC_FILE))

mkdirs:
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)

$(BIN_FILE): $(OBJ_FILE)
	$(LD) -o $(BIN_FILE) $(OBJ_FILE) $(LD_FLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@

$(OBJ_DIR)/%.i: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -E $< -o $@

-include $(OBJ_FILE:.o=.d)

clean:
	rm -r $(OBJ_DIR)
	rm -r $(BIN_DIR)
