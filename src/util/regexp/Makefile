BIN := regexp.o 

SRC := regexp_main.c production.c
HEADER := regexp.h production.h grammar.h production_token.def

OBJ := $(patsubst %.c, %.o, $(SRC))

include ../../../Makefile.common

all: $(BIN)
clean: 
	@rm -rf *.o
$(BIN): $(OBJ)
	@ld -r -o $@ $^
$(OBJ): $(SRC) $(HEADER) 
