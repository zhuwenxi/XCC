BIN := regexp.o 

SRC := regexp.c production.c
HEADER := regexp.h production.h grammar.h production_token.def

U_FILES := $(patsubst %.c, %.u, $(SRC))

include ../../../Makefile.common

all: $(BIN)
clean: 
	@rm -rf *.o *.u
$(BIN): $(U_FILES)
	@ld -r -o $@ $^
$(U_FILES): $(HEADER) 
