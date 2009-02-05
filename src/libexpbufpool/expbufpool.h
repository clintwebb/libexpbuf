#ifndef __EXPBUFPOOL_H
#define __EXPBUFPOOL_H

#include <expbuf.h>



typedef struct {
	struct {
		expbuf_t **list;
		int entries;
	} ready, used;
	int max;
} expbuf_pool_t;


void expbuf_pool_init(expbuf_pool_t *list, int max);
void expbuf_pool_free(expbuf_pool_t *list);

expbuf_t * expbuf_pool_new(expbuf_pool_t *list, int amount);
void expbuf_pool_return(expbuf_pool_t *list, expbuf_t *buffer);


#endif


