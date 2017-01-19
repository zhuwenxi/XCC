OBJ := ./regexp/regexp.o
SUBDIR := regexp

UTIL_BIN:= util.o

include ../../Makefile.common

all: 
	@ld -r -o $(UTIL_BIN) $(OBJ)
clean:
	@rm -rf *.o

