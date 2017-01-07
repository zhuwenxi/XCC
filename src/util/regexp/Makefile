BIN := regexp.o 
SRC := regexp_main.c 
OBJ := $(patsubst %.c, %.o, $(SRC))

include ../../../Makefile.common

all: $(BIN)
	@echo "make all in ./src/util/regexp"
clean: 
	@rm -rf *.o
$(BIN): $(OBJ)
	@ld -r -o $@ $^
