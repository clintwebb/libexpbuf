libexpbuf - Build and Install instructions.

Building and installing the library is fairly simple.
Assuming that you have obtained and extracted the source code.
(the latest can be downloaded from https://github.com/clintwebb/libexpbuf/)

As a regular user, issue the following command
	
	make

Which should provide an output something like:

	gcc -c -fPIC expbuf.c -o libexpbuf.o
	gcc -shared -Wl,-soname,libexpbuf.so.1 -o libexpbuf.so.1.0.1 libexpbuf.o
	
Then, as root (by sudo or su), issue the following command:
	
	sudo make install
	
Which will copy the files into the appropriate locations.  
After installation you can begin using it immediately.

The above is to install just the library (for other compiled applications to utilise), for development, will also need to install the headers and other components.

	sudo make install_dev

 
