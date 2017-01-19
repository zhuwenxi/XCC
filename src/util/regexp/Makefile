BIN := regexp.o 

SRC := regexp_main.c
HEADER := regexp.h

OBJ := $(patsubst %.c, %.o, $(SRC))

include ../../../Makefile.common

all: $(BIN)
clean: 
	@rm -rf *.o
$(BIN): $(OBJ)
	@ld -r -o $@ $^
$(OBJ): $(SRC) $(HEADER) 
