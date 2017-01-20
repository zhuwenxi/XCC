BIN := regexp.o 

SRC := regexp_main.c production.c
HEADER := regexp.h production.h grammar.h 

OBJ := $(patsubst %.c, %.o, $(SRC))

include ../../../Makefile.common

all: $(BIN)
clean: 
	@rm -rf *.o
$(BIN): $(OBJ)
	@ld -r -o $@ $^
$(OBJ): $(SRC) $(HEADER) 
