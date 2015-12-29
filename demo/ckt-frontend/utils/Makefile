ROSS_COMPILE ?=
ifeq ($(CROSS_COMPILE),)
	$(error "Not set CROSS_COMPILE=";)
endif

CC      = $(CROSS_COMPILE)gcc

CFLAGS  = -Wall -Os
LDFLAGS = -lpthread

LIB		= libutils.so
CFLAGS  += -fPIC
LIBSRC  = $(wildcard *.c)
LIBSRC	+= $(wildcard *.h)

$(LIB): $(LIBSRC)
	$(CC) $(LDFLAGS) -shared -o $@ $^
	find . -name "*.so" -exec cp {} ../../../lib/cktlib/ \;

install:
	#install --mode=644 libutils.so $(INSTALLDIR)/lib

clean:
	rm -rf $(LIB)
