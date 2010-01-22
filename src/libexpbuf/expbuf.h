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


#define EXPBUF_VERSION 0x00010220
#define EXPBUF_VERSION_TEXT "v1.02.20"


typedef struct
{
	char *data;
	unsigned int length;
	unsigned int max;
	char internally_created;
} expbuf_t;


expbuf_t * expbuf_init(expbuf_t *buf, unsigned int size);
void expbuf_clear(expbuf_t *buf);
void expbuf_free(expbuf_t *buf);

void expbuf_add(expbuf_t *buf, const void *data, unsigned int len);
void expbuf_set(expbuf_t *buf, const void *data, unsigned int len);
void expbuf_purge(expbuf_t *buf, unsigned int len);

void expbuf_shrink(expbuf_t *buf, unsigned int extra);

char * expbuf_string(expbuf_t *buf);
void expbuf_print(expbuf_t *buf, const char *format, ...);


#define BUF_LENGTH(b)  ((b)->length)
#define BUF_MAX(b)     ((b)->max)
#define BUF_DATA(b)    ((b)->data)

#endif
