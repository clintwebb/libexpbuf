## make file for libexpbufpool

MAINFILE=libexpbufpool.so
LIBVER=1.0.1
LIBFILE=$(MAINFILE).$(LIBVER)
SONAME=$(MAINFILE).1

MANPATH=/usr/local/man


all: $(LIBFILE)


LIBS=
ARGS=-Wall -g -O2
REQH=

# Need to be able to make 'man-pages' as well.  Not sure where to get the source for those... 

libexpbufpool.o: expbufpool.c expbufpool.h $(REQH)
	gcc -c -fPIC expbufpool.c -o $@ $(ARGS)

libexpbufpool.a: libexpbufpool.o
	@>$@
	@rm $@
	ar rcs $@ $^

$(LIBFILE): libexpbufpool.o
	gcc -shared -Wl,-soname,$(SONAME) -o $@ $^


lib-test: lib-test.c $(REQH) /usr/include/expbuf.h
	gcc lib-test.c -o $@ -lexpbuf -lexpbufpool

install: $(LIBFILE) expbufpool.h 
	cp expbufpool.h /usr/include/
	cp $(LIBFILE) /usr/lib/
	@-test -e /usr/lib/$(MAINFILE) && rm /usr/lib/$(MAINFILE)
	ln -s /usr/lib/$(LIBFILE) /usr/lib/$(MAINFILE)
	nice ldconfig
	@echo "Install complete."

man-pages: doc/expbuf_add.3
	cp expbuf.h /usr/include/
	cp $(LIBFILE) /usr/lib/
	@-test -e /usr/lib/$(MAINFILE) && rm /usr/lib/$(MAINFILE)
	ln -s /usr/lib/$(LIBFILE) /usr/lib/$(MAINFILE)
	nice ldconfig
	@mkdir tmp.install
	@cp doc/* tmp.install/
	@gzip tmp.install/expbuf_add.3
	cp tmp.install/expbuf_add.3.gz $(MANPATH)/man3/
	@rm -r tmp.install	
	@echo "Man-pages Install complete."

clean:
	@-[ -e libexpbufpool.o ] && rm libexpbufpool.o
	@-[ -e libexpbufpool.so* ] && rm libexpbufpool.so*
	@-test -e lib-test && rm lib-test

