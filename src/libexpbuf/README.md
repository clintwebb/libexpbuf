# libexpbuf
A simple library to provide a standard way of handling data buffers.
Commonly used when handing segments of data.  Doesn't do anything super fancy, just makes the functionality consistent and readable.

Note that the buffer is not special.  The actual memory allocated can be accessed and used just like normally allocated memory.  It is just useful in managing that memory.

It can typically be used in multiple ways.

Example where reading in some data from somewhere, and processing it doesn't always line up.  In other words, the amount of data
needed to be read before it can be processed is unknown.  So it can read in multiple chunks of data, and process what it can
(leaving what is remaining in the buffer) and then read in more.

```c
#include <expbuf.h>

int main(void) {
        expbuf_t *buffer;
        int len;

        // create a new buffer.  By specifying a NULL, the buffer structure will be created internally and therefore cleaned up when
        // expbuf_free() is called when it is no longer needed.   Since we will be using the buffer to load data, for this example
        // we will start it with an extra space of 1024.
        buffer = expbuf_init(NULL, 1024);

        len = read_data(BUF_OFFSET(buffer), BUF_AVAIL(buffer));
        while (len >= 0) {
                expbuf_inc(buffer, len);
                processed = process_data(BUF_DATA(buffer), BUF_LENGTH(buffer));
                expbuf_purge(buffer, processed);

                // we are going to read some data from somewhere, so we need to make sure there is enough free space in it.   This
                // means that if there is less than 1024 bytes available in the buffer, it will increase the size to at least that.
                // If it already has more than that available, then it will essentially do nothing.
                expbuf_atleast(buffer, 1024);

                len = read_data(BUF_OFFSET(buffer), BUF_AVAIL(buffer));
        }

        buffer = expbuf_free(buffer);
        assert(buffer == NULL);
        return(0);
}

```

