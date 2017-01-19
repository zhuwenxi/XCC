OBJ := main.o 
HEADER := util/util.h

SUBOBJ := util/util.o
SUBDIR := util

XCC_OBJ := xcc.o

include ../Makefile.common

all: $(OBJ) 
	@cc -I$(XCC_INCLUDE) $(CFLAGS) -o $(XCC_OBJ) $(OBJ) $(SUBOBJ)
$(OBJ): $(HEADER)
clean:
	@rm -rf *.o
