SUBDIRS			= $(wildcard matrix*) $(wildcard ckt*)
TARGET			= subdirs
PHONY			+= $(TARGET) $(SUBDIRS) %.clean
PHONY			+= $(TARGET) $(SUBDIRS) %.install

all: $(TARGET)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

%.clean:
	@(cd $(patsubst %.clean, %, $@) && $(MAKE) clean)

clean distclean: $(patsubst %, %.clean, $(SUBDIRS))

%.install:
	(cd $(patsubst %.install, %, $@) && $(MAKE) install)
#	rm -fr $(INSTALLDIR)/usr/bin/*.c
#	find . -name "*.so" -exec cp {} ../lib/cktlib/ \;

install: $(patsubst %, %.install, $(SUBDIRS))

.PHONY: $(PHONY) install clean distclean
