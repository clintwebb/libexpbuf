// simple tool to test that the 


#include <assert.h>
#include <expbufpool.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	expbuf_pool_t bufpool;
	expbuf_t *aa, *bb, *cc;
	
	// create the pool.
	printf("Initialising pool.\n");
	expbuf_pool_init(&bufpool, 256);

	// get a new buffer from the pool.
	printf("Getting new AA buffer\n");
	aa = expbuf_pool_new(&bufpool, 0);
	assert(aa != NULL);
	assert(aa->length == 0);
	
	// assign some data to the buffer.
	printf("Adding data to AA buffer\n");
	expbuf_add(aa, "hello", 5);
	
	// get another buffer from the pool.
	printf("Getting new BB buffer\n");
	bb = expbuf_pool_new(&bufpool, 0);
	assert(bb != NULL);
	assert(bb->length == 0);

	// assign more data.
	printf("Adding data to BB buffer\n");
	expbuf_add(aa, "bye", 3);
	
	// return a buffer to the pool.
	printf("Clearing AA buffer and returning to pool\n");
	expbuf_clear(aa);
	expbuf_pool_return(&bufpool, aa);
	aa = NULL;

	// free the resources in the pool.
	printf("Clearing BB buffer and returning to pool\n");
	expbuf_clear(bb);
	expbuf_pool_return(&bufpool, bb);
	bb = NULL;

	// now get a buffer that had been previously returned.
	printf("Getting new AA buffer\n");
	assert(aa == NULL);
	aa = expbuf_pool_new(&bufpool, 0);
	assert(aa != NULL);
	assert(aa->length == 0);

	// assign more data.
	printf("Adding data to AA buffer\n");
	expbuf_add(aa, "bye", 3);
	
	// return a buffer to the pool.
	printf("Clearing AA buffer and returning to pool\n");
	expbuf_clear(aa);
	expbuf_pool_return(&bufpool, aa);
	aa = NULL;

	printf("Freeing buffer pool resources.\n");
	assert(aa == NULL && bb == NULL);
	expbuf_pool_free(&bufpool);
	
	return(0);
}
