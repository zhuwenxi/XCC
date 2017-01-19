SUBDIR := src 

XCC_BIN := xcc
XCC_OBJ := $(addsuffix .o, $(XCC_BIN))

BIN_DIR := bin
SRC_DIR := src

include ./Makefile.common

all:
	@cp $(SRC_DIR)/$(XCC_OBJ) $(BIN_DIR)/$(XCC_BIN)
clean: 
	@rm -rf $(BIN_DIR)/*
