## make file for libexpbuf

PRODUCT := expbuf
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

# Need to be able to make 'man-pages' as well.  Not sure where to get the source for those... 

libexpbuf.o: libexpbuf.c expbuf.h $(REQH)
	gcc -c -fPIC libexpbuf.c -o $@ $(ARGS)

libexpbuf.a: libexpbuf.o
	@>$@
	@rm $@
	ar rcs $@ $^

$(LIBFILE): libexpbuf.o
	gcc -shared -Wl,-soname,$(SONAME) -o $@ $^


lib-test: lib-test.c $(REQH) $(INCDIR)/expbuf.h
	gcc lib-test.c -o $@ -lexpbuf

define PKG_CONFIG
prefix=$(DESTDIR)
includedir=$(INCDIR)
libdir=$(LIBDIR)

Name: $(LIBNAME)
Description: Library for handling expandable buffers.
URL: https://github.com/clintwebb/libexpbuf
Version: $(LIBVER)
Cflags: -I$${includedir}
Libs: -L$${libdir} -l$(LIBNAME)
endef
# Requires: librisp
# Cflags: -I$${includedir} -pthread
# Libs: -L$${libdir} $(LDFLAGS) `pkg-config --libs librisp`



$(LIBNAME).pc: Makefile
	$(file > $@,$(PKG_CONFIG))


install_dev: install expbuf.h $(LIBNAME).pc
	cp expbuf.h $(INCDIR)/
	cp -v $(LIBNAME).pc /usr/lib/pkgconfig/
	@for i in man3/*.3; do gzip -c $$i > $(MANPATH)/$$i.gz; done

install: $(LIBFILE)
	cp $(LIBFILE) $(LIBDIR)/
	@-test -e $(LIBDIR)/$(MAINFILE) && rm $(LIBDIR)/$(MAINFILE)
	ln -s $(LIBDIR)/$(LIBFILE) $(LIBDIR)/$(MAINFILE)

clean:
	@-[ -e $(LIBNAME).pc ] && rm $(LIBNAME).pc
	@-[ -e libexpbuf.o ] && rm libexpbuf.o
	@-[ -e libexpbuf.so* ] && rm libexpbuf.so*
	@-test -e lib-test && rm lib-test

