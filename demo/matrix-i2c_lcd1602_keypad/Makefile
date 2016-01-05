CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc 

TARGET	=  $(patsubst Matrix-%.c,matrix-%, $(wildcard *.c))
SRC 	=  $(wildcard *.c)
SRC 	+= $(wildcard *.h)

$(TARGET):$(SRC)
	$(CC) -o $@ -L ./lib/ -I ./include/ $^ -lfahw -lm -Wall -lpthread

.PHONY: clean 
clean:
	rm -f $(TARGET)
