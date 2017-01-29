OBJ := ./regexp/regexp.o
DEPS := util.u
SUBDIR := regexp

UTIL_BIN:= util.o

include ../../Makefile.common

all: $(DEPS) 
	@ld -r -o $(UTIL_BIN) $(OBJ) $(DEPS)
util.u: util.h
clean:
	@rm -rf *.o *.u

