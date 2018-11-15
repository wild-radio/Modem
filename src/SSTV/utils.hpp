/*
robot36 - encode and decode images using SSTV in Robot 36 mode
Written in 2011 by <Ahmet Inan> <xdsopl@googlemail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/


#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>
#include <time.h>

int64_t gcd(int64_t a, int64_t b);

float fclampf(float x, float min, float max);

float flerpf(float a, float b, float x);

char *string_time(char *fmt);
#endif

