SUBDIRS			= $(wildcard matrix*) $(wildcard ckt*)
TARGET			= subdirs
PHONY			+= $(TARGET) $(SUBDIRS) %.clean

all: $(TARGET)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

%.clean:
	@(cd $(patsubst %.clean, %, $@) && $(MAKE) clean)

clean distclean: $(patsubst %, %.clean, $(SUBDIRS))

install: 
	mkdir -p $(INSTALLDIR)/usr/bin
	for demo_folder in $(wildcard matrix*); do \
		cd $$demo_folder; \
		install --mode=755 matrix* $(INSTALLDIR)/usr/bin; \
		cd ..;\
	done;  
	
	for demo_folder in $(wildcard ckt*); do \
		cd $$demo_folder; \
		install --mode=755 ckt* $(INSTALLDIR)/usr/bin; \
		cd ..;\
	done; 

	rm -fr $(INSTALLDIR)/usr/bin/*.c
	find . -name "*.so" -exec cp {} ../lib/cktlib/ \;
.PHONY: $(PHONY) install clean distclean
