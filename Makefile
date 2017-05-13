# project directory structure
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
INCLUDE_DIR = include

# sub directories in "src"
UTIL_DIR = util
ALL_SUB_DIR = $(UTIL_DIR)
BUILD_SUB_DIR = $(addprefix	$(BUILD_DIR)/, $(ALL_SUB_DIR))

# source files
SOURCE_FILE = $(UTIL_DIR)/util.c $(UTIL_DIR)/linked_list.c $(UTIL_DIR)/logger.c
OBJ_FILE = $(patsubst	%.c, %.o, $(SOURCE_FILE))
OBJ_FILE_FULL_PATH = $(addprefix	$(BUILD_DIR)/, $(OBJ_FILE)) 
XCC_BIN = $(BUILD_DIR)/xcc

# test files
TEST_SOURCE_FILE = test_all.c $(UTIL_DIR)/linked_list_test.c $(UTIL_DIR)/logger_test.c
TEST_OBJ_FILE = $(patsubst %.c, %.o, $(TEST_SOURCE_FILE))
TEST_OBJ_FULL_PATH = $(addprefix	$(BUILD_DIR)/, $(TEST_OBJ_FILE))
TEST_BIN = $(BUILD_DIR)/test_all

# config from "Makefile.config"
include ./Makefile.config

DEBUG ?= 0
ifeq ($(DEBUG), 0)
FLAGS = -DRELEASE
else 
FLAGS = -DDEBUG
endif

# compiler
CC = gcc $(FLAGS)

.PHONY: test


global: $(XCC_BIN)

$(XCC_BIN): $(BUILD_SUB_DIR) $(OBJ_FILE_FULL_PATH) $(BUILD_DIR)/main.o
	$(CC) -I$(INCLUDE_DIR) -o $@ $(OBJ_FILE_FULL_PATH) $(BUILD_DIR)/main.o

test: $(BUILD_SUB_DIR) $(TEST_OBJ_FULL_PATH) $(OBJ_FILE_FULL_PATH)
	$(CC) -I$(INCLUDE_DIR) -o $(TEST_BIN) $(TEST_OBJ_FULL_PATH) $(OBJ_FILE_FULL_PATH)

clean: 
	@echo "clean-up done."
	@rm -rf build/*
$(BUILD_SUB_DIR):
	@mkdir -p $@


# rules for obj files in "src"
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@

# rules for obj files in "test"
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@

$(BUILD_DIR)/test_all.c: $(INCLUDE_DIR)/test.def
