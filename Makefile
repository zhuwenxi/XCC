SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build
INCLUDE_DIR = include

SOURCE_FILE = main.c util.c linked_list.c
OBJ_FILE = $(patsubst	%.c, %.o, $(SOURCE_FILE))
OBJ_FILE_FULL_PATH = $(addprefix	$(BUILD_DIR)/, $(OBJ_FILE)) 
XCC = $(BUILD_DIR)/xcc


global: $(XCC)

$(XCC): $(OBJ_FILE_FULL_PATH)
	$(CC) -I$(INCLUDE_DIR) -o $@ $^
clean:
	@echo "clean-up done."
	@rm -rf build/*
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/%.h
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@
