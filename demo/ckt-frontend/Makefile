CROSS_COMPILE ?= arm-linux-gnueabihf-
#CROSS_COMPILE ?= arm-linux-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

SUBDIRS = utils/

CFLAGS= -I./include/
CFLAGS+=-I../../lib/includes/
CFLAGS+= -I./utils/
CFLAGS+=-I./from/
CFLAGS+=-I./to/
CFLAGS+=-I./analysisif/

LFLAGS= -L../../lib/  -lfahw
LFLAGS+= -L./utils/ -lutils

LDFLAGS= -lpthread

TARGET	=  $(patsubst ckt_%.c,ckt_%, $(wildcard *.c))
SRC 	=  $(wildcard *.c from/*.c to/*.c analysisif/*.c)

OBJS	= $(patsubst %.c, %.o, $(SRC))

$(TARGET):$(OBJS)
	cd $(SUBDIRS);$(MAKE)
	$(CC) $(LDFLAGS) -o $@ $(LFLAGS) $^ -lm -Wall

%.o:%.c
	$(CC) $(LDFLAGS) -o $@ $(CFLAGS) $^ -lm -Wall -c

clean:
	rm -f $(TARGET) $(OBJS)
	cd $(SUBDIRS);$(MAKE) clean

install:
	install --mode=755 $(TARGET) $(INSTALLDIR)/usr/bin
	cd $(SUBDIRS); $(MAKE) install
