# project directory structure
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
INCLUDE_DIR = include

# source files
SOURCE_FILE = util.c linked_list.c
OBJ_FILE = $(patsubst	%.c, %.o, $(SOURCE_FILE))
OBJ_FILE_FULL_PATH = $(addprefix	$(BUILD_DIR)/, $(OBJ_FILE)) 
XCC_BIN = $(BUILD_DIR)/xcc

# test files
TEST_SOURCE_FILE = test_all.c linked_list_test.c
TEST_OBJ_FILE = $(patsubst %.c, %.o, $(TEST_SOURCE_FILE))
TEST_OBJ_FULL_PATH = $(addprefix	$(BUILD_DIR)/, $(TEST_OBJ_FILE))
TEST_BIN = $(BUILD_DIR)/test_all

.PHONY: test


global: $(XCC_BIN)

$(XCC_BIN): $(OBJ_FILE_FULL_PATH) $(BUILD_DIR)/main.o
	$(CC) -I$(INCLUDE_DIR) -o $@ $^
test: $(TEST_OBJ_FULL_PATH) $(OBJ_FILE_FULL_PATH)
	$(CC) -I$(INCLUDE_DIR) -o $(TEST_BIN) $^
clean:
	@echo "clean-up done."
	@rm -rf build/*


# rules for obj files in "src"
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@

# rules for obj files in "test"
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@
