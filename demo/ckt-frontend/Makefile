CROSS_COMPILE ?= arm-linux-gnueabihf-
#CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc 

SUBDIRS = utils/

CFLAGS= -I./include/
CFLAGS+=-I../../lib/includes/
CFLAGS+= -I./utils/

LFLAGS= -L../../lib/  -lfahw
LFLAGS+= -L./utils/ -lutils

TARGET	=  $(patsubst ckt_%.c,ckt_%, $(wildcard *.c))
SRC 	=  $(wildcard *.c)
SRC 	+= $(wildcard *.h)

$(TARGET):$(SRC)
	cd $(SUBDIRS);$(MAKE)
	$(CC) -o $@ $(LFLAGS) $(CFLAGS) $^ -lm -Wall

clean:
	rm -f $(TARGET)
	cd $(SUBDIRS);$(MAKE) clean

install:
	cd $(SUBDIRS); $(MAKE) install
