## make file for libexpbuf

MAINFILE=libexpbuf.so
LIBVER=1.0.1
LIBFILE=$(MAINFILE).$(LIBVER)
SONAME=$(MAINFILE).1

all: $(LIBFILE)


LIBS=
ARGS=
REQH=

# Need to be able to make 'man-pages' as well.  Not sure where to get the source for those... 

libexpbuf.o: expbuf.c expbuf.h $(REQH)
	gcc -c -fPIC expbuf.c -o $@ $(ARGS)

libexpbuf.a: libexpbuf.o
	@>$@
	@rm $@
	ar rcs $@ $^

$(LIBFILE): libexpbuf.o
	gcc -shared -Wl,-soname,$(SONAME) -o $@ $^


lib-test: lib-test.c $(REQH) /usr/include/expbuf.h
	gcc lib-test.c -o $@ -lexpbuf

install: $(LIBFILE) expbuf.h doc/expbuf_add.3  doc/expbuf_clear.3  doc/expbuf_free.3  doc/expbuf_init.3  doc/expbuf_purge.3  doc/expbuf_shrink.3  doc/expbuf_t.3  doc/libexpbuf.3
	cp expbuf.h /usr/include/
	cp $(LIBFILE) /usr/lib/
	@-test -e /usr/lib/$(MAINFILE) && rm /usr/lib/$(MAINFILE)
	ln -s /usr/lib/$(LIBFILE) /usr/lib/$(MAINFILE)
	nice ldconfig
	@mkdir tmp.install
	@cp doc/* tmp.install/
	@gzip tmp.install/expbuf_add.3
	@gzip tmp.install/expbuf_clear.3
	@gzip tmp.install/expbuf_free.3
	@gzip tmp.install/expbuf_init.3
	@gzip tmp.install/expbuf_purge.3
	@gzip tmp.install/expbuf_shrink.3
	@gzip tmp.install/expbuf_t.3
	@gzip tmp.install/libexpbuf.3
	cp tmp.install/expbuf_add.3.gz /usr/man/man3/
	cp tmp.install/expbuf_clear.3.gz /usr/man/man3/
	cp tmp.install/expbuf_free.3.gz /usr/man/man3/
	cp tmp.install/expbuf_init.3.gz /usr/man/man3/
	cp tmp.install/expbuf_purge.3.gz /usr/man/man3/
	cp tmp.install/expbuf_shrink.3.gz /usr/man/man3/
	cp tmp.install/expbuf_t.3.gz /usr/man/man3/
	cp tmp.install/libexpbuf.3.gz /usr/man/man3/
	@rm -r tmp.install	
	@echo "Install complete."

clean:
	@-[ -e libexpbuf.o ] && rm libexpbuf.o
	@-[ -e libexpbuf.so* ] && rm libexpbuf.so*
	@-test -e lib-test && rm lib-test

