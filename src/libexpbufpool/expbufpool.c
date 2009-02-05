/*
 

Implements a pool of expanding buffers.   It is a very simple pooling system.  When a process needs a buffer, one is provided.  When the process has finished 
with it, the buffer is returned, but the memory that was allocated to it is not free'd.  

The idea is that it will reduce the amount of time that is used in continuously allocating and de-allocating chunks of memory, and a stable number of buffers 
with the maximum amount of memory needed will be allocated and re-used.

*/


#include "expbufpool.h"

#include <assert.h>
#include <stdlib.h>


//-----------------------------------------------------------------------------
// Initialise the buffer list.  It can be assumed that the list contains
// garbage.
void expbuf_pool_init(expbuf_pool_t *list, int max)
{
	assert(list != NULL);
	assert(max >= 0);
	
	list->ready.list = NULL;
	list->ready.entries = 0;
	
	list->used.list = NULL;
	list->used.entries = 0;

	list->max = max;
}


//-----------------------------------------------------------------------------
// Free the resources that are referenced by the list.
void expbuf_pool_free(expbuf_pool_t *list)
{
	assert(list != NULL);

	assert((list->ready.list == NULL && list->ready.entries == 0) || (list->ready.list != NULL && list->ready.entries > 0));
	assert((list->used.list == NULL && list->used.entries == 0) || (list->used.list != NULL && list->used.entries > 0));

	// first we will go thru the 'used' list and make sure it is empty.
	if (list->used.list != NULL) {
		assert(list->used.entries > 0);
		while(list->used.entries > 0) {
			list->used.entries --;
			assert(list->used.list[list->used.entries] == NULL);
		}
		free(list->used.list);
		list->used.list = NULL;
		assert(list->used.entries == 0);
	}

	// now we go thru the 'ready' list and remove all the buffers in it (which
	// technically, at this point, there should not be any empty slots in the
	// list.)
	if (list->ready.list != NULL) {
		assert(list->ready.entries > 0);
		while(list->ready.entries > 0) {
			list->ready.entries --;
			assert(list->ready.list[list->ready.entries] != NULL);
			expbuf_free(list->ready.list[list->ready.entries]);
			free(list->ready.list[list->ready.entries]);
		}

		free(list->ready.list);
		list->ready.list = NULL;
		assert(list->ready.entries == 0);
	}
}



//-----------------------------------------------------------------------------
// Looks at our list of buffers to find a free one so that
expbuf_t * expbuf_pool_new(expbuf_pool_t *list, int amount)
{
	expbuf_t *buff, *tmp;
	int best;
	int i;
	
	assert(list != NULL);
	assert(amount >= 0);

	buff = NULL;
	best = -1;
	if (list->ready.entries > 0) {

		// go through the list.  If we find a perfect match, then stop looking.
		for (i=0; i<list->ready.entries && buff == NULL; i++) {

			// to make it easier to keep track, we'll grab a pointer to the object we are currently looking at.
			tmp = list->ready.list[i];
			if (tmp != NULL) {
				assert(tmp->length == 0);
				
				if (amount <= 0 || tmp->max == amount) {
					// if no specific amount was specified, we will just choose the first
					// in the list.  If the buffer is the exact size we are looking for,
					// then we choose it.
					buff = tmp;
					list->ready.list[i] = NULL;
				}
				else if (best < 0) {
					// we haven't got a 'best' yet, so this one would be it.
					best = i;
				}
				else {
					// we have to determine if this entry is better than the one we have already chosen as 'best'.
					assert(best >= 0);
					assert(best < list->ready.entries);
					if (list->ready.list[best]->max < amount && tmp->max > list->ready.list[best]->max) {
						best = i;
					}
					else if (list->ready.list[best]->max > amount && tmp->max < list->ready.list[best]->max && tmp->max > amount) {
						best = i;
					}
				}
			}
		}

		if (buff == NULL && best >= 0) {
			buff = list->ready.list[best];
			list->ready.list[best] = NULL;
		}
	}

	if (buff == NULL) {
		// didn't find a suitable entry, so we need to create one.
		assert(best < 0);
		buff = (expbuf_t *) malloc(sizeof(expbuf_t));
		assert(amount >= 0);
		expbuf_init(buff, amount);
	}

	// add it to our 'used' list.
	assert(buff);
	best = -1;
	for (i=0; i<list->used.entries && best < 0; i++) {
		if (list->used.list[i] == NULL) {
			list->used.list[i] = buff;
			best = i;
		}
	}

	// if we didn't find a suitable slot.
	if (best < 0) {
		assert(buff != NULL);
		list->used.list = (expbuf_t **) realloc(list->used.list, sizeof(expbuf_t *) * (list->used.entries + 1));
		list->used.list[list->used.entries] = buff;
		list->used.entries ++;
	}

	return(buff);
}


//-----------------------------------------------------------------------------
// Returns a previously provided buffer to the pool.  The buffer must have been
// allocated through the pool.   If the buffer being returned is larger than
// the pre-specified max for the pool, then it will be shrunk.  If max is zero,
// and the buffer is unusually large, then you should shrink it yourself before
// returning it.
//
//	** Should we free the buffer if it is larger than the max, or merely shrink
//	   it?  I fear that we would likely end up fragmenting memory much quicker
//	   if we only shrink it.
// 
void expbuf_pool_return(expbuf_pool_t *list, expbuf_t *buffer)
{
	int i;
	int found;
	
	assert(list != NULL);
	assert(buffer != NULL);

	assert(list->used.entries > 0 && list->used.list != NULL);
	assert((list->ready.entries == 0 && list->ready.list == NULL) || (list->ready.entries > 0 && list->ready.list != NULL));

	// check that the buffer is empty.
	assert(buffer->length == 0);

	assert(list->max >= 0);
	if (list->max > 0 && buffer->max > list->max) {
		expbuf_shrink(buffer, list->max);
	}

	// check that the returned buffer is in the 'used' list.
	// remove entry from 'used' list.
	found = -1;
	for (i=0; i<list->used.entries && found < 0; i++) {
		if (list->used.list[i] == buffer) {
			found = i;
			list->used.list[i] = NULL;
		}
	}
	assert(found >= 0);

	if (found >= 0) {
		// check that the returned buffer is not already in the 'ready' list.
		// add buffer to the 'ready' list.
		found = -1;
		for (i=0; i<list->ready.entries; i++) {
			assert(list->ready.list[i] != buffer);
			if (found < 0) {
				if (list->ready.list[i] == NULL) {
					found = i;
					list->ready.list[i] = buffer;
				}
			}
		}

		if (found < 0) {
			// didn't find an empty slot in the 'ready' list, so we need to make one.
			list->ready.list = (expbuf_t **) realloc(list->ready.list, sizeof(expbuf_t *) * (list->ready.entries + 1));
			list->ready.list[list->ready.entries] = buffer;
			list->ready.entries ++;
		}
	}
}






