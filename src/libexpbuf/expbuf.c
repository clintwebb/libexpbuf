/*

	libexpbuf
	(c) Copyright Hyper-Active Systems, Australia

	Contact:
		Clinton Webb
		webb.clint@gmail.com

*/


#include "expbuf.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>




// initialise an expbuf structure, assuming that it contains garbage to begin with.
void expbuf_init(expbuf_t *buf, unsigned int size)
{
	if (size > 0) {
		buf->data = (char *) malloc(size);
		if (buf->data == NULL) {
			buf->length = 0;
			buf->max = 0;
		}
		else {
			buf->length = size;
			buf->max = size;
		}
	}
	else {
		buf->data = NULL;
		buf->length = 0;
		buf->max = 0;
	}
}

// clear out any data in the buffer.  This will reset the length pointer only.  It will not free any resources held.
void expbuf_clear(expbuf_t *buf)
{
	assert(buf);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL && buf->max > 0));
	
	buf->length = 0;
}

// this will clear out any data in the buffer, and free the resources
// allocated.  After calling this function, it should be safe to
// deallocate the expbuf_t object.
void expbuf_free(expbuf_t *buf)
{
	assert(buf);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL && buf->max > 0));
	if (buf->data) {
		free(buf->data);
		buf->data = NULL;
		buf->length = 0;
		buf->max = 0;
	}
}



// add data to the end of our buffer, expanding it if necessary.
void expbuf_add(expbuf_t *buf, void *data, unsigned int len)
{
	unsigned int avail;
	
	assert(buf);
	assert(data);
	assert(len > 0);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL && buf->max > 0));

	avail = buf->max - buf->length;
	if (avail < len) {
		buf->data = (char*) realloc(buf->data, (buf->length + len));
		assert(buf->data);
		buf->max = buf->length + len;
		assert(buf->length <= buf->max);	
	}

	memmove(buf->data + buf->length, data, len);
	buf->length += len;
	assert(buf->length <= buf->max);
}


// add data to the buffer overwriting what is already there, expanding it if necessary.
void expbuf_set(expbuf_t *buf, void *data, unsigned int len)
{
	
	assert(buf);
	assert(data);
	assert(len > 0);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL && buf->max > 0));

	buf->length = 0;

	if (buf->max < len) {
		buf->data = (char*) realloc(buf->data, len);
		assert(buf->data);
		buf->max = len;
	}

	memmove(buf->data, data, len);
	buf->length = len;
	assert(buf->length <= buf->max);
}


// purge some data from the begining of the buffer (presumably because it has been processed).  Moving the remaining data 
// to the beginning of the buffer. 
void expbuf_purge(expbuf_t *buf, unsigned int len) {
	assert(buf);
	assert(len > 0);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL && buf->max > 0));
	assert(len <= buf->length);

	if (len < buf->length) {
		buf->length -= len;
		memmove(buf->data, buf->data+len, buf->length);
	}
	else {
		assert(buf->length == len);
		len = 0;
	}
}

// The buffer expands as more data is added to it.  This function will shrink down, leaving 'extra' padding at the end.
// this will increase memory usage also if the available space is less than 'extra'.
void expbuf_shrink(expbuf_t *buf, unsigned int extra)
{
	assert(buf);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL && buf->max > 0));

	if (extra == 0 && buf->length == 0 && buf->data != NULL) {
		free(buf->data);
		buf->data = NULL;
		buf->max = 0;
	}
	else if ((buf->max - buf->length) != extra) {
		buf->data = (char *) realloc(buf->data, buf->length+extra);
		buf->max = buf->length + extra;
	}
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL && buf->max > 0));
}



