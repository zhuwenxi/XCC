SUBDIR := src 

xcc: all
	@cp ./src/xcc.o ./bin/xcc	
include ./Makefile.common
