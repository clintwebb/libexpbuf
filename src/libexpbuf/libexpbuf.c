/*

    libexpbuf
    This is a small and simple library used to control a very simple expanding buffer.

    Copyright (C) Hyper-Active Systems, Australia
    Copyright (C) 2015  Clinton Webb

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


#if (EXPBUF_VERSION != 0x00010410)
#error "Incorrect header version.  code and header versions must match."
#endif




// initialise an expbuf structure, assuming that it contains garbage to begin with.
expbuf_t * expbuf_init(expbuf_t *b, unsigned int size)
{
	expbuf_t *buf;
	
	// if a NULL is passed in, that means that we need to create the object ourselves (the safest path)
	if (b) {
		buf = b;
		buf->internally_created = 0;
	}
	else {
		buf = calloc(1, sizeof(*buf));
		buf->internally_created = 1;
	}
	
	BUF_LENGTH(buf) = 0;
	if (size > 0) {
		BUF_DATA(buf) = (char *) malloc(size);
		if (BUF_DATA(buf) == NULL) { BUF_MAX(buf) = 0; }
		else { BUF_MAX(buf) = size; }
	}
	else {
		BUF_DATA(buf) = NULL;
		BUF_MAX(buf) = 0;
	}
	
	return(buf);
}

// clear out any data in the buffer.  This will reset the length pointer only.  It will not free any resources held.
void expbuf_clear(expbuf_t *buf)
{
	assert(buf);
	assert(BUF_LENGTH(buf) <= BUF_MAX(buf));
	assert((BUF_DATA(buf) == NULL && BUF_LENGTH(buf) == 0 && BUF_MAX(buf) == 0) || (BUF_DATA(buf) != NULL && BUF_MAX(buf) > 0));
	
	BUF_LENGTH(buf) = 0;
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

	assert(BUF_LENGTH(buf) <= BUF_MAX(buf));
	
	if (BUF_DATA(buf)) {
		assert(BUF_MAX(buf) > 0);
		BUF_MAX(buf) = 0;
		BUF_LENGTH(buf) = 0;

		free(BUF_DATA(buf));
		BUF_DATA(buf) = NULL;
	}
	else {
		assert(BUF_LENGTH(buf) == 0);
		assert(BUF_MAX(buf) == 0);
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
	assert(BUF_LENGTH(buf) <= BUF_MAX(buf));
	assert((BUF_DATA(buf) == NULL && BUF_LENGTH(buf) == 0 && BUF_MAX(buf) == 0) || (BUF_DATA(buf) != NULL && BUF_MAX(buf) > 0));

	avail = BUF_MAX(buf) - BUF_LENGTH(buf);
	if (avail < len) {
		// there was not enough space in the buffer to add the new data.
		
		// determine the new maximum size of the buffer.
		BUF_MAX(buf) = BUF_LENGTH(buf) + len;
		assert(BUF_LENGTH(buf) <= BUF_MAX(buf));
		
		BUF_DATA(buf) = (char*) realloc(BUF_DATA(buf), BUF_MAX(buf));
		assert(BUF_DATA(buf));
	}

	memmove(BUF_DATA(buf) + BUF_LENGTH(buf), data, len);
	BUF_LENGTH(buf) += len;
	assert(BUF_LENGTH(buf) <= BUF_MAX(buf));
}


// add data to the buffer overwriting what is already there, expanding it if necessary.
void expbuf_set(expbuf_t *buf, const void *data, unsigned int len)
{
	assert(buf);
	assert(data);
	assert(len > 0);
	assert(BUF_LENGTH(buf) <= BUF_MAX(buf));
	assert((BUF_DATA(buf) == NULL && BUF_LENGTH(buf) == 0 && BUF_MAX(buf) == 0) || (BUF_DATA(buf) != NULL && BUF_MAX(buf) > 0));

	BUF_LENGTH(buf) = 0;

	if (BUF_MAX(buf) < len) {
		// TODO: Do we really want to shrink the buffer if we set data to it?
		BUF_DATA(buf) = (char*) realloc(BUF_DATA(buf), len);
		assert(BUF_DATA(buf));
		BUF_MAX(buf) = len;
	}

	memmove(BUF_DATA(buf), data, len);
	BUF_LENGTH(buf) = len;
	assert(BUF_LENGTH(buf) <= BUF_MAX(buf));
}


// purge some data from the begining of the buffer (presumably because it has
// been processed).  Moving the remaining data to the beginning of the buffer.
void expbuf_purge(expbuf_t *buf, unsigned int len) {
	assert(buf);
	assert(len > 0);
	assert(BUF_LENGTH(buf) <= BUF_MAX(buf));
	assert((BUF_DATA(buf) == NULL && BUF_LENGTH(buf) == 0 && BUF_MAX(buf) == 0) || (BUF_DATA(buf) != NULL && BUF_MAX(buf) > 0));
	assert(len <= BUF_LENGTH(buf));

	if (len < BUF_LENGTH(buf)) {
		BUF_LENGTH(buf) -= len;
		assert(BUF_LENGTH(buf) > 0);
		memmove(BUF_DATA(buf), BUF_DATA(buf)+len, BUF_LENGTH(buf));
	}
	else {
		assert(BUF_LENGTH(buf) == len);
		BUF_LENGTH(buf) = 0;
	}
}

// The buffer expands as more data is added to it.  This function will shrink
// down, leaving 'extra' padding at the end. 
// this will increase memory usage also if the available space is less than 'extra'.
void expbuf_shrink(expbuf_t *buf, unsigned int extra)
{
	assert(buf);
	assert(BUF_LENGTH(buf) <= BUF_MAX(buf));
	assert((BUF_DATA(buf) == NULL && BUF_LENGTH(buf) == 0 && BUF_MAX(buf) == 0) || (BUF_DATA(buf) != NULL && BUF_MAX(buf) > 0));

	if (extra == 0 && BUF_LENGTH(buf) == 0 && BUF_DATA(buf) != NULL) {
		free(BUF_DATA(buf));
		BUF_DATA(buf) = NULL;
		BUF_MAX(buf) = 0;
	}
	else if ((BUF_MAX(buf) - BUF_LENGTH(buf)) != extra) {
		BUF_DATA(buf) = (char *) realloc(BUF_DATA(buf), BUF_LENGTH(buf)+extra);
		BUF_MAX(buf) = BUF_LENGTH(buf) + extra;
	}
	assert(BUF_LENGTH(buf) <= BUF_MAX(buf));
	assert((BUF_DATA(buf) == NULL && BUF_LENGTH(buf) == 0 && BUF_MAX(buf) == 0) || (BUF_DATA(buf) != NULL && BUF_MAX(buf) > 0));
}


// make sure that the data is null terminated, with enough space, and then return a pointer to the data.  
char * expbuf_string(expbuf_t *buf) {
	assert(buf != NULL);
	assert(BUF_LENGTH(buf) <= BUF_MAX(buf));
	assert((BUF_DATA(buf) == NULL && BUF_LENGTH(buf) == 0 && BUF_MAX(buf) == 0) || (BUF_DATA(buf) != NULL && BUF_MAX(buf) > 0));

	// if there is no space allocated, or not enough space allocated, then we
	// add one char (or create a new one)
	if (BUF_MAX(buf) == BUF_LENGTH(buf) || BUF_DATA(buf) == NULL) {
		BUF_DATA(buf) = (char *) realloc(BUF_DATA(buf), BUF_LENGTH(buf) + 1);
		BUF_MAX(buf)++;
	}
	BUF_DATA(buf)[BUF_LENGTH(buf)] = '\0';
	return(BUF_DATA(buf));
}


// apply some formatted text to the buffer.  This will over-write whatever is already in there.
void expbuf_print(expbuf_t *buf, const char *format, ...)
{
  va_list ap;
  int redo;
  int n, avail;

	assert(buf);
	assert(format);

	// process the string. Apply directly to the buildbuf.  If buildbuf is not
	// big enough, increase the size and do it again.
	redo = 1;
	while (redo) {
		avail = BUF_MAX(buf)-BUF_LENGTH(buf);
		va_start(ap, format);
		n = vsnprintf(BUF_DATA(buf)+BUF_LENGTH(buf), avail, format, ap);
		va_end(ap);

		assert(n > 0);
		if (n > avail) {
			// there was not enough space, so we need to increase it, and try again.
			expbuf_shrink(buf, n + 1);
			assert(redo);
		}
		else {
			assert(n <= BUF_MAX(buf));
			BUF_LENGTH(buf) += n;
			redo = 0;
		}
	}
}


// function that adds data from src expbuf to target expbuf.
void expbuf_addbuf(expbuf_t *target, expbuf_t *src)
{
	assert(target && src);
	expbuf_add(target, BUF_DATA(src), BUF_LENGTH(src));
}

// function that sets data from src expbuf to target expbuf, overwriting what was originally in target.
void expbuf_setbuf(expbuf_t *target, expbuf_t *src)
{
	assert(target && src);
	expbuf_set(target, BUF_DATA(src), BUF_LENGTH(src));
}

