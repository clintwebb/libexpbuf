## make file for libexpbuf

MAINFILE=libexpbuf.so
LIBVER=1.0.1
LIBFILE=$(MAINFILE).$(LIBVER)
SONAME=$(MAINFILE).1
LIBDIR=/usr/lib
INCDIR=/usr/include

MANPATH=/usr/share/man


all: $(LIBFILE)

LIBS=
ARGS=-Wall -g
REQH=

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

install_dev: expbuf.h 
	cp expbuf.h $(INCDIR)/
	pushd doc
	@for i in *.3; do gzip -c $$i > $(MANPATH)/man3/$$i.gz; done
	popd

install: $(LIBFILE)
	cp $(LIBFILE) $(LIBDIR)/
	@-test -e $(LIBDIR)/$(MAINFILE) && rm $(LIBDIR)/$(MAINFILE)
	ln -s $(LIBDIR)/$(LIBFILE) $(LIBDIR)/$(MAINFILE)
	nice ldconfig

clean:
	@-[ -e libexpbuf.o ] && rm libexpbuf.o
	@-[ -e libexpbuf.so* ] && rm libexpbuf.so*
	@-test -e lib-test && rm lib-test

