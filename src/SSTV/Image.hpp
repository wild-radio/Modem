/*
robot36 - encode and decode images using SSTV in Robot 36 mode
Written in 2011 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/


#ifndef IMG_H
#define IMG_H

#include <stdint.h>
#include <string>

struct Image {
	void (*close)(struct Image *);
	uint8_t *pixel;
	void *data;
	int width;
	int height;
};

void close_img(Image *);
int open_img_read(Image **, const char *);
int open_img_write(Image **, const char *, int, int);

#endif

