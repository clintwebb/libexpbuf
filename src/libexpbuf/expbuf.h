/*

	libexpbuf
	(c) Copyright Hyper-Active Systems, Australia

	Contact:
		Clinton Webb
		webb.clint@gmail.com

	This is a small and simple library used to control a very simple expanding buffer.

	It is released under GPL v2 or later license.  Details are included in the LICENSE file.

*/

#ifndef __EXPBUF_H
#define __EXPBUF_H



typedef struct 
{
	char *data;
	unsigned int length;
	unsigned int max;
} expbuf_t;


void expbuf_init(expbuf_t *buf, unsigned int size);
void expbuf_clear(expbuf_t *buf);
void expbuf_free(expbuf_t *buf);

void expbuf_add(expbuf_t *buf, void *data, unsigned int len);
void expbuf_set(expbuf_t *buf, void *data, unsigned int len);
void expbuf_purge(expbuf_t *buf, unsigned int len);
void expbuf_shrink(expbuf_t *buf, unsigned int extra);


#endif
