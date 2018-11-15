#ifndef ROBOT36_DECODE_PULSE_HPP
#define ROBOT36_DECODE_PULSE_HPP


#include "pcm.hpp"
#include <pulse/simple.h>
#include <pulse/error.h>

#define RATE 44100

struct Pulse{
	struct pcm base;
	pa_simple *s;
	pa_sample_spec ss;
	short buffer[RATE];
	int buffer_pointer;
};

int open_pulse_read(struct pcm **, const char *);
int open_pulse_write(struct pcm **, const char *, int, int, float);

#endif //ROBOT36_DECODE_PULSE_HPP
