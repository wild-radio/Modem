/*
robot36 - encode and decode images using SSTV in Robot 36 mode
Written in 2011 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/


#ifndef PCM_HPP
#define PCM_HPP

struct pcm {
	void (*close)(struct pcm *);
	void (*info)(struct pcm *);
	int (*rate)(struct pcm *);
	int (*channels)(struct pcm *);
	int (*rw)(struct pcm *, short *, int);
	void *data;
};

void close_pcm(pcm *);
void info_pcm(pcm *);
int rate_pcm(pcm *);
int channels_pcm(pcm *);
int read_pcm(pcm *, short *, int);
int write_pcm(pcm *, short *, int);
int open_pcm_read(pcm **, const char *);
int open_pcm_write(pcm **, const char *, int, int, float);

#endif

