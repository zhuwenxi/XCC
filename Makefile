SUBDIR := src 

XCC_BIN := xcc
XCC_OBJ := $(addsuffix .o, $(XCC_BIN))

BIN_DIR := bin
SRC_DIR := src

# For test.
TEST_U := test/test.u test/linked_list.u
TEST_BIN := bin/test 
SUB_OBJ := src/util/util.o

include ./Makefile.common

all:
	@cp $(SRC_DIR)/$(XCC_OBJ) $(BIN_DIR)/$(XCC_BIN)
clean: 
	@rm -rf $(BIN_DIR)/* test/*.u test/*.o
test: $(TEST_U) all
	@echo "making test"
	@cc -I$(XCC_INCLUDE) $(CFLAGS) -o $(TEST_BIN) $(TEST_U) $(SUB_OBJ)
test/test.u: test/test.h test/test.def
