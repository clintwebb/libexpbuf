/*

    libexpbuf
    This is a small and simple library used to control a very simple expanding buffer.

    Copyright (C) 2022  Clinton Webb

    Contact:
        Clinton Webb <webb.clint@gmail.com>


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "expbuf.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#if (EXPBUF_VERSION != 0x00010430)
#error "Incorrect header version.  code and header versions must match."
#endif




// initialise an expbuf structure, assuming that it contains garbage to begin with.
expbuf_t * expbuf_init(expbuf_t *b, unsigned int size)
{
	expbuf_t *buf;
	
	if (b) {
		buf = b;
		buf->internally_created = 0;
	}
	else {
		// if a NULL is passed in, that means that we need to create the object ourselves (the safest path)
		buf = calloc(1, sizeof(*buf));
		assert(buf);
		if (buf) {
			buf->internally_created = 1;
		}
	}
	
	if (buf) {
		buf->length = 0;
		if (size > 0) {
			buf->data = (char *) malloc(size);
			if (buf->data == NULL) { buf->max = 0; }
			else { buf->max = size; }
		}
		else {
			buf->data = NULL;
			buf->max = 0;
		}
	}

	return(buf);
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
//
// if the object was created internally, returns a NULL.  If the object was not 
// created internally, then it will return the object so that it can be deleted 
// elsewhere.
expbuf_t * expbuf_free(expbuf_t *buf)
{
	assert(buf);

	assert(buf->internally_created == 0 || buf->internally_created == 1);
	assert(buf->length <= buf->max);
	
	if (buf->data) {
		assert(buf->max > 0);
		buf->max = 0;
		buf->length = 0;

		free(buf->data);
		buf->data = NULL;
	}
	else {
		assert(buf->length == 0);
		assert(buf->max == 0);
	}
	
	// if the buffer was created internally, then we need to make it free itself.
	if (buf->internally_created == 1) {
		free(buf);
		return(NULL);
	}
	else {
		return(buf);
	}
}



// add data to the end of our buffer, expanding it if necessary.
void expbuf_add(expbuf_t *buf, const void *data, unsigned int len)
{
	unsigned int avail;
	
	assert(buf);
	assert(data);
	assert(len > 0);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL && buf->max > 0));

	avail = buf->max - buf->length;
	if (avail < len) {
		// there was not enough space in the buffer to add the new data.
		
		// determine the new maximum size of the buffer.
		buf->max = buf->length + len;
		assert(buf->length <= buf->max);
		
		buf->data = (char*) realloc(buf->data, buf->max);
		assert(buf->data);
	}

	memmove(buf->data + buf->length, data, len);
	buf->length += len;
	assert(buf->length <= buf->max);
}


// add data to the buffer overwriting what is already there, expanding it if necessary.
void expbuf_set(expbuf_t *buf, const void *data, unsigned int len)
{
	assert(buf);
	assert(data);
	assert(len > 0);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL && buf->max > 0));

	buf->length = 0;

	if (buf->max < len) {
		// TODO: Do we want to shrink the buffer if we set data to it?
		buf->data = (char*) realloc(buf->data, len);
		assert(buf->data);
		buf->max = len;
	}

	memmove(buf->data, data, len);
	buf->length = len;
	assert(buf->length <= buf->max);
}


// purge some data from the begining of the buffer (presumably because it has
// been processed).  Moving the remaining data to the beginning of the buffer.
// TODO: Add some smarts to this, so that it will not move large chunks of data, and use an algorithm to determine whether to just move an offset, or move the actual data (will depend on multiple factors)
void expbuf_purge(expbuf_t *buf, unsigned int len) 
	assert(buf);
	assert(len > 0);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL && buf->max > 0));
	assert(len <= buf->length);

	if (len < buf->length) {
		buf->length -= len;
		assert(buf->length > 0);
		memmove(buf->data, buf->data+len, buf->length);
	}
	else {
		assert(buf->length == len);
		buf->length = 0;
	}
}

// If data is directly added to the buffer, then this will simply increase the marked length of the contents in the buffer.
// It will do some validation of the input (for example, if the available space is only 24 bytes, but it is indicating it is
// increasing by 32, then there is an error.
// Return the new length, no matter how it was processed.
unsigned int expbuf_inc(expbuf_t *buf, unsigned int len)
{
	assert(buf);
	assert(len > 0);
	assert(len <= BUF_AVAIL(buf));

	if (len <= BUF_AVAIL(buf)) {
		buf->length += len;
		assert(buf->length <= buf->max);
	}

	return(buf->length);
}



// The buffer expands as more data is added to it.  This function will shrink
// down, leaving 'extra' padding at the end. 
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


// make sure that the data is null terminated, with enough space, and then return a pointer to the data.  
char * expbuf_string(expbuf_t *buf) {
	assert(buf != NULL);
	assert(buf->length <= buf->max);
	assert((buf->data == NULL && buf->length == 0 && buf->max == 0) || (buf->data != NULL && buf->max > 0));

	// if there is no space allocated, or not enough space allocated, then we
	// add one char (or create a new one)
	if (buf->max == buf->length || buf->data == NULL) {
		buf->data = (char *) realloc(buf->data, buf->length + 1);
		buf->max++;


	}
	buf->data[buf->length] = '\0';
	return(buf->data);
}


// apply some formatted text to the buffer.  This will append to the existing buffer contents.
void expbuf_print(expbuf_t *buf, const char *format, ...)
{
  va_list ap;
  int redo;
  int n, avail;

	assert(buf);
	assert(format);

	avail = buf->max-buf->length;

	// process the string. Apply directly to the buildbuf.  If buildbuf is not
	// big enough, increase the size and do it again.
	redo = 1;
	while (redo) {
		va_start(ap, format);
		n = vsnprintf(buf->data+buf->length, avail, format, ap);
		va_end(ap);

		assert(n > 0);
		if (n > avail) {
			// there was not enough space, so we need to increase it, and try again.
			expbuf_shrink(buf, n + 1);
			assert(redo);
		}
		else {
			assert(n <= buf->max);
			buf->length += n;
			redo = 0;
		}
	}
}


// function that adds data from src expbuf to target expbuf.
void expbuf_addbuf(expbuf_t *target, expbuf_t *src)
{
	assert(target && src);
	expbuf_add(target, src->data, src->length);
}

// function that sets data from src expbuf to target expbuf, overwriting what was originally in target.
void expbuf_setbuf(expbuf_t *target, expbuf_t *src)
{
	assert(target && src);
	expbuf_set(target, src->data, src->length);
}

