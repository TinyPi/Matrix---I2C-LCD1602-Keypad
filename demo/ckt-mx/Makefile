#CROSS_COMPILE ?= arm-linux-gnueabihf-
CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc 
CFLAGS= -I./include/

TARGET	=  mx 
SRC 	=  $(wildcard *.c)
SRC 	+= $(wildcard *.h)


$(TARGET):$(SRC)
	$(CC) -o $@ $(CFLAGS) $^ ./lib/libfahw.a  -lm -lpthread -Wall 

.PHONY: clean install 
clean:
	rm -f $(TARGET)

install:
	install --mode=755 $(TARGET) $(INSTALLDIR)/usr/bin
