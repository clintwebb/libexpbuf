/*

	libexpbuf
	This is a small and simple library used to control a very simple expanding buffer.

	
	Copyright (C) Hyper-Active Systems, Australia
    Copyright (C) 2015  Clinton Webb

	Contact:
		Clinton Webb
		webb.clint@gmail.com


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Public License for more details.

    You should have received a copy of the GNU Lesser Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __EXPBUF_H
#define __EXPBUF_H


#define EXPBUF_VERSION 0x00010410
#define EXPBUF_VERSION_TEXT "v1.04.10"


typedef struct
{
	char *data;
	unsigned int length;
	unsigned int max;
	char internally_created;
} expbuf_t;


// See the man-pages for more detail about each function.


expbuf_t * expbuf_init(expbuf_t *buf, unsigned int size);
void expbuf_clear(expbuf_t *buf);
expbuf_t * expbuf_free(expbuf_t *buf);

void expbuf_add(expbuf_t *buf, const void *data, unsigned int len);
void expbuf_set(expbuf_t *buf, const void *data, unsigned int len);
void expbuf_purge(expbuf_t *buf, unsigned int len);

void expbuf_addbuf(expbuf_t *target, expbuf_t *src);
void expbuf_setbuf(expbuf_t *target, expbuf_t *src);

void expbuf_shrink(expbuf_t *buf, unsigned int extra);

char * expbuf_string(expbuf_t *buf);
void expbuf_print(expbuf_t *buf, const char *format, ...);


#define BUF_LENGTH(b)  ((b)->length)
#define BUF_MAX(b)     ((b)->max)
#define BUF_DATA(b)    ((b)->data)
#define BUF_OFFSET(b)  ((b)->data + (b)->length)
#define BUF_AVAIL(b)   ((b)->max - (b)->length)

#endif
