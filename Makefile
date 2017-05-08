SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build

SOURCE_FILE = main.c
OBJ_FILE = $(patsubst	%.c, %.o, $(SOURCE_FILE))
OBJ_FILE_FULL_PATH = $(addprefix	$(BUILD_DIR)/, $(OBJ_FILE)) 

global: xcc


xcc: $(OBJ_FILE_FULL_PATH)
	$(CC) -o $(BUILD_DIR)/xcc $^
clean:
	@echo "clean-up done."
	@rm -rf build/*
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) -c $< -o $@
