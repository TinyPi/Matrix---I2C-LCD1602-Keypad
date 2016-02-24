CROSS_COMPILE ?= arm-linux-gnueabihf-
#CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

SUBDIRS = utils/
CFLAGS= -I./include/
CFLAGS+=-I../../lib/includes/

LFLAGS= -L../../lib/  -lfahw
LFLAGS+= -L./utils/ -lutils

LDFLAGS= -lpthread

TARGET	=  $(patsubst ckt_%.c,ckt_%, $(wildcard *.c))
SRC 	=  $(wildcard *.c unpack/*.c control/*.c module/*.c analysis/*.c)

OBJS	= $(patsubst %.c, %.o, $(SRC))

all:
	mkdir include
	find . -name "*.h" -exec cp {} include/ \;
	$(MAKE) $(TARGET)

$(TARGET):$(OBJS)
	cd $(SUBDIRS);$(MAKE)
	$(CC) $(LDFLAGS) -o $@ $(LFLAGS) $^ -lm -Wall

%.o:%.c
	$(CC) $(LDFLAGS) -o $@ $(CFLAGS) $^ -lm -Wall -c

clean:
	rm -f $(TARGET) $(OBJS)
	rm -fr include/*
	rm -fr include
	cd $(SUBDIRS);$(MAKE) clean

install:
	install --mode=755 $(TARGET) $(INSTALLDIR)/usr/bin
	echo $(TEST)
	cd $(SUBDIRS); $(MAKE) install
