## make file for libexpbufpool


LIBS=
ARGS=-Wall -g -O2
REQH=

PRODUCT := expbufpool
LIBVER  := 1.0.1

LIBNAME  := lib$(PRODUCT)
MAINFILE := $(LIBNAME).so
LIBFILE  := $(MAINFILE).$(LIBVER)
SONAME   := $(MAINFILE).1
DESTDIR  :=
LIBDIR   := $(DESTDIR)/usr/lib
INCDIR   := $(DESTDIR)/usr/include
MANPATH  := $(DESTDIR)/usr/share/man

all: $(LIBFILE) $(LIBNAME).pc

LIBS :=
ARGS := -Wall -g
REQH :=

.PHONY: all install install_dev clean $(LIBNAME).pc




define PKG_CONFIG
prefix=$(DESTDIR)
includedir=$(INCDIR)
libdir=$(LIBDIR)

Name: $(LIBNAME)
Description: Library for handling a pool of expandable buffers.
URL: https://github.com/clintwebb/libexpbuf
Version: $(LIBVER)
Requires: libexpbuf
Cflags: -I$${includedir}
Libs: -L$${libdir} -l$(PRODUCT)
endef
# Requires: librisp
# Cflags: -I$${includedir} -pthread
# Libs: -L$${libdir} $(LDFLAGS) `pkg-config --libs librisp`



$(LIBNAME).pc: Makefile
	$(file > $@,$(PKG_CONFIG))




$(LIBNAME).o: expbufpool.c expbufpool.h $(REQH)
	gcc -c -fPIC expbufpool.c -o $@ $(ARGS)

$(LIBNAME).a: libexpbufpool.o
	@>$@
	@rm $@
	ar rcs $@ $^

$(LIBFILE): $(LIBNAME).o
	gcc -shared -Wl,-soname,$(SONAME) -o $@ $^


lib-test: lib-test.c $(REQH) /usr/include/expbuf.h
	gcc lib-test.c -g -o $@ `pkg-config --libs libexpbufpool`

install: $(LIBFILE)
	cp $(LIBFILE) /usr/lib/
	@-test -e /usr/lib/$(MAINFILE) && rm /usr/lib/$(MAINFILE)
	ln -s /usr/lib/$(LIBFILE) /usr/lib/$(MAINFILE)
	nice ldconfig
	@echo "Install complete."

install_dev: expbufpool.h $(LIBNAME).pc doc/expbuf_pool.3
	cp expbufpool.h /usr/include/
	cp -v $(LIBNAME).pc /usr/lib/pkgconfig/
	@for i in man3/*.3; do gzip -c $$i > $(MANPATH)/$$i.gz; done
	@echo "Install-Dev complete."

clean:
	@-[ -e libexpbufpool.o ] && rm libexpbufpool.o
	@-[ -e libexpbufpool.so* ] && rm libexpbufpool.so*
	@-test -e lib-test && rm lib-test
	@-[ -e $(LIBNAME).pc ] && rm $(LIBNAME).pc

