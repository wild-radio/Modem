/*
robot36 - encode and decode images using SSTV in Robot 36 mode
Written in 2011 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mmap_file.hpp"
#include "Image.hpp"

struct ppm {
	struct Image base;
	void *p;
	size_t size;
};

void close_ppm(struct Image *img)
{
	struct ppm *ppm = (struct ppm *)(img->data);
	munmap_file(ppm->p, ppm->size);
	delete ppm;
}

int open_ppm_read(struct Image **p, const char *name) {
	ppm *ppm = new struct ppm;
	ppm->base.close = close_ppm;
	ppm->base.data = (void *)ppm;

	if (!mmap_file_ro(&(ppm->p), name, &(ppm->size))) {
		fprintf(stderr, "couldnt open Image file %s\n", name);
		delete ppm;
		return 0;
	}

	char *chr = (char *)ppm->p;
	size_t index = 0;
	if (ppm->size < 10 || chr[index++] != 'P' || chr[index++] != '6') {
		fprintf(stderr, "unsupported Image file\n");
		munmap_file(ppm->p, ppm->size);
		delete ppm;
		return 0;
	}

	char buff[16];
	int integer[3];
	for (int n = 0; n < 3; n++) {
		for (; index < ppm->size; index++) {
			if (chr[index] >= '0' && chr[index] <= '9')
				break;
			if (chr[index] == '#')
				for (; index < ppm->size && chr[index] != '\n'; index++);
		}
		for (int i = 0; i < 16; i++)
			buff[i] = 0;
		for (int i = 0; index < ppm->size && i < 15; i++, index++) {
			buff[i] = chr[index];
			if (buff[i] < '0' || buff[i] > '9') {
				buff[i] = 0;
				break;
			}
		}
		if (index >= ppm->size) {
			fprintf(stderr, "broken Image file\n");
			munmap_file(ppm->p, ppm->size);
			delete ppm;
			return 0;
		}
		integer[n] = atoi(buff);
		index++;
	}

	if (0 == integer[0] || 0 == integer[1] || integer[2] != 255) {
		fprintf(stderr, "unsupported Image file\n");
		munmap_file(ppm->p, ppm->size);
		delete ppm;
		return 0;
	}
	ppm->base.width = integer[0];
	ppm->base.height = integer[1];

	ppm->base.pixel = (uint8_t *)ppm->p + index;

	*p = &(ppm->base);

	return 1;
}

int open_ppm_write(struct Image **p, const char *name, int width, int height) {
	auto *ppm = new struct ppm;
	ppm->base.close = close_ppm;
	ppm->base.data = (void *)ppm;

	char head[32];
	snprintf(head, 32, "P6 %d %d 255\n", width, height);
	ppm->size = strlen(head) + width * height * 3;

	if (!mmap_file_rw(&(ppm->p), name, ppm->size)) {
		fprintf(stderr, "couldnt open Image file %s\n", name);
		delete ppm;
		return 0;
	}

	memcpy(ppm->p, head, strlen(head));
	ppm->base.pixel = (uint8_t *)ppm->p + strlen(head);
	memset(ppm->base.pixel, 0, width * height * 3);

	*p = &(ppm->base);

	return 1;
}

