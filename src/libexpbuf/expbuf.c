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



/*
typedef struct 
{
	char *data;
	unsigned int length;
	unsigned int max;
} expbuf_t;
*/

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

// clear out any data in the buffer.  THis should reset everything to 'empty'
void expbuf_clear(expbuf_t *buf)
{
	assert(buf);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL));
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
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL));

	avail = buf->max - buf->length;
	if (avail < len) {
		buf->data = (char*) realloc(buf->data, (buf->length + len));
		assert(buf->data);
		buf->max = buf->length + len;
		assert(buf->length <= buf->max);	
	}

	memmov(buf->data + buf->length, data, len);
	buf->length += len;
	assert(buf->length <= buf->max);
}


// purge some data from the begining of the buffer (presumably because it has been processed).  Moving the remaining data 
// to the beginning of the buffer. 
void expbuf_purge(expbuf_t *buf, unsigned int len) {
	assert(buf);
	assert(len > 0);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL));
	assert(len <= buf->length);

	memmov(buf->data, buf->data+len, buf->length-len);
	buf->length -= len;
}

// The buffer expands as more data is added to it.  This function will shrink down, leaving 'extra' padding at the end.
void expbuf_shrink(expbuf_t *buf, unsigned int extra)
{
	assert(buf);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL));

	if ((buf->max - buf->length) != extra) {
		buf->data = (char *) realloc(buf->data, buf->length+extra);
		buf->max = buf->length + extra;
	}
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL));
}



