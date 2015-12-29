CROSS_COMPILE = arm-linux-
ifeq ($(CROSS_COMPILE),)
$(error "Not set CROSS_COMPILE=";)
endif
AS			= $(CROSS_COMPILE)as
LD			= $(CROSS_COMPILE)ld
CC			= $(CROSS_COMPILE)gcc
AR			= $(CROSS_COMPILE)ar
NM			= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
RANLIB		= $(CROSS_COMPILE)ranlib

export AS LD CC AR NM STRIP OBJCOPY OBJDUMP RANLIB CROSS_COMPILE

TOPDIR		= $(shell pwd)
INSTALLDIR	= $(TOPDIR)/install
SUBDIRS		= lib\
		demo
TARGET		= subdirs
PHONY		+= $(TARGET) $(SUBDIRS) %.clean

export INSTALLDIR TOPDIR

all: $(TARGET)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

%.clean:
	rm -rf $(INSTALLDIR)
	@(cd $(patsubst %.clean, %, $@) && $(MAKE) clean)

clean distclean: $(patsubst %, %.clean, $(SUBDIRS))

install:
	rm -fr $(INSTALLDIR)
	mkdir -p $(INSTALLDIR)/usr/bin
	make -C ./demo install 
	make -C ./lib install
	#(cd $(patsubst %.install, %, $@) && $(MAKE) install)

#install: $(patsubst %, %.install, $(SUBDIRS))

.PHONY: $(PHONY) install clean distclean
