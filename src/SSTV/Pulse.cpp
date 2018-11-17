#include <iostream>
#include <cstring>
#include "Pulse.hpp"

void close_pulse(struct pcm *pcm) {
	Pulse *pulse = (Pulse*)pcm;
	pa_simple_free(pulse->s);
	delete pulse;
}

void info_pulse(struct pcm *pcm) {
	//TODO: Fill this up
}

int rate_pulse(struct pcm *pcm) {
	Pulse *pulse = (Pulse*)pcm;
	return pulse->ss.rate;
}

int channels_pulse(struct pcm *pcm) {
	Pulse *pulse = (Pulse*)pcm;
	return pulse->ss.channels;
}

int read_pulse(struct pcm *pcm, short *buff, int frames) {
	int error;

	Pulse *pulse = (Pulse*)pcm;
	if (pa_simple_read(pulse->s, buff, sizeof(short) * frames, &error) < 0) {
		std::cout << "Error reading from pulseaudio: " << strerror(error) << std::endl;
	}

	return 1;
}

int write_pulse(struct pcm *pcm, short *buff, int frames) {
	int error;
	Pulse *pulse = (Pulse*)pcm;

	pulse->buffer[pulse->buffer_pointer] = buff[0];
	pulse->buffer_pointer++;

	if (pulse->buffer_pointer == BUFFER_SIZE) {
		if (pa_simple_write(pulse->s, pulse->buffer, sizeof(short) * pulse->buffer_pointer, &error) < 0) {
			std::cout << "Error writing to pulseaudio: " << strerror(error) << std::endl;
			return 0;
		}
		pulse->buffer_pointer = 0;
	}

	return 1;
}

int open_pulse_write(struct pcm **p, const char *name, int, int, float) {
	Pulse *pulse = new Pulse;
	pulse->ss = {PA_SAMPLE_S16LE, RATE, 1};
	if (!(pulse->s = pa_simple_new(NULL, name, PA_STREAM_PLAYBACK, NULL, "playback", &pulse->ss, NULL, NULL, NULL))) {
		std::cout << "Error opening the pulse connection" << std::endl;
		return 0;
	}
	pa_simple_flush(pulse->s, NULL);

	pulse->base.data = (void *)&pulse;
	pulse->base.close = close_pulse;
	pulse->base.rate = rate_pulse;
	pulse->base.info = info_pulse;
	pulse->base.channels = channels_pulse;
	pulse->base.rw = write_pulse;

	pulse->buffer_pointer = 0;

	*p = (pcm*) &pulse->base;

	return 1;
}

int open_pulse_read(struct pcm **p, const char *name) {
	Pulse *pulse = new Pulse;
	pulse->ss = {PA_SAMPLE_S16LE, RATE, 1};
	if (!(pulse->s = pa_simple_new(NULL, name, PA_STREAM_RECORD, NULL, "record", &pulse->ss, NULL, NULL, NULL))) {
		std::cout << "Error opening the pulse connection" << std::endl;
		return 0;
	}
	pa_simple_flush(pulse->s, NULL);

	pulse->base.data = (void *)&pulse;
	pulse->base.close = close_pulse;
	pulse->base.rate = rate_pulse;
	pulse->base.info = info_pulse;
	pulse->base.channels = channels_pulse;
	pulse->base.rw = read_pulse;

	pulse->buffer_pointer = 0;

	*p = (pcm*) &pulse->base;

	return 1;
}