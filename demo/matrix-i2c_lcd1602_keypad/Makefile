#CROSS_COMPILE ?= arm-linux-gnueabihf-
CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc 
CFLAGS= -I./include/

TARGET	=  $(patsubst Matrix-%.c,matrix-%, $(wildcard *.c))
SRC 	=  $(wildcard *.c)
SRC 	+= $(wildcard *.h)


$(TARGET):$(SRC)
	$(CC) -o $@ $(CFLAGS) $^ ./lib/libfahw.a  -lm -Wall 

.PHONY: clean 
clean:
	rm -f $(TARGET)
