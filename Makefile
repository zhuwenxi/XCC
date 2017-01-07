SUBDIR := src 

include ./Makefile.common
all:
	@cp ./src/xcc.o ./bin/xcc
clean: 
	@rm ./bin/xcc
