# project directory structure
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
INCLUDE_DIR = include

# sub directories in "src"
UTIL_DIR = util
SCANNER_DIR = scanner

# source files
SOURCE_FILE = $(UTIL_DIR)/util.c \
	$(UTIL_DIR)/linked_list.c \
	$(UTIL_DIR)/logger.c \
	$(UTIL_DIR)/array_list.c \
	$(UTIL_DIR)/string_buffer.c \
	$(UTIL_DIR)/stack.c \
	$(UTIL_DIR)/hash_table.c \
	$(UTIL_DIR)/regexp.c \
	$(UTIL_DIR)/context_free_grammar.c \
	$(UTIL_DIR)/LR_automata.c \
	$(SCANNER_DIR)/scanner.c
OBJ_FILE = $(patsubst %.c, %.o, $(SOURCE_FILE))
OBJ_FILE_FULL_PATH = $(addprefix $(BUILD_DIR)/, $(OBJ_FILE)) 
XCC_BIN = $(BUILD_DIR)/xcc

# test files
TEST_SOURCE_FILE = test_all.c \
	$(UTIL_DIR)/linked_list_test.c \
	$(UTIL_DIR)/array_list_test.c \
	$(UTIL_DIR)/logger_test.c \
	$(UTIL_DIR)/stack_test.c \
	$(UTIL_DIR)/hash_table_test.c \
	$(UTIL_DIR)/context_free_grammar_test.c
TEST_OBJ_FILE = $(patsubst %.c, %.o, $(TEST_SOURCE_FILE))
TEST_OBJ_FULL_PATH = $(addprefix $(BUILD_DIR)/, $(TEST_OBJ_FILE))
TEST_BIN = $(BUILD_DIR)/test_all

# config from "Makefile.config"
include ./Makefile.config

DEBUG ?= 0
ifeq ($(DEBUG), 0)
FLAGS = -DRELEASE
else 
FLAGS = -DDEBUG -gdwarf-2
endif

MTRACE ?= 0
ifeq ($(MTRACE), 0)
FLAGS += -DNO_MTRACE
else
FLAGS += -DMTRACE
endif

# FLAGS += -fPIC
FLAGS += -Iinclude/util

# compiler
CC = gcc $(FLAGS)

.PHONY: test


global: $(XCC_BIN)

$(XCC_BIN): $(OBJ_FILE_FULL_PATH) $(BUILD_DIR)/main.o
	$(CC) -I$(INCLUDE_DIR) -o $@ $^ -lm

test: $(TEST_OBJ_FULL_PATH) $(OBJ_FILE_FULL_PATH)
	$(CC) -I$(INCLUDE_DIR) -o $(TEST_BIN) $^ -lm

clean: 
	@echo "clean-up done."
	@rm -rf build/*
$(BUILD_SUB_DIR):
	@mkdir -p $@

# rules for obj files in "src"
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	@$(eval parent_dir=$(shell dirname $@))
	@mkdir -p $(parent_dir)
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@ -lm
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@ -lm

# rules for obj files in "test"
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(INCLUDE_DIR)/%.h
	@dirname $@ | xargs mkdir -p
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@ -lm

$(BUILD_DIR)/test_all.c: include/test.def
