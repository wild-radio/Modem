#include <iostream>
#include "Robot36Encoder.hpp"
#include "limits.h"
#include "utils.hpp"
#include "yuv.hpp"
#include "../MiniModem/PTT.hpp"


void Robot36Encoder::encode(std::string image_filename, std::string source) {

	open_img_read(&img, image_filename.c_str());

	if (!open_pcm_write(&pcm, source.c_str(), 48000, 1, 37.5))
		return;

	rate = rate_pcm(pcm);
	channels = channels_pcm(pcm);

	sync_porch_len = rate * sync_porch_sec;
	porch_len = rate * porch_sec;
	y_len = rate * y_sec;
	uv_len = rate * uv_sec;
	hor_sync_len = rate * hor_sync_sec;
	seperator_len = rate * seperator_sec;


	buff = (short *)malloc(sizeof(short)*channels);

	info_pcm(pcm);

	hz2rad = (2.0 * M_PI) / rate;
	nco = 1i * 0.7;
	enum { N = 13 };
	float seq_freq[N] = { 1900.0, 1200.0, 1900.0, 1200.0, 1300.0, 1300.0, 1300.0, 1100.0, 1300.0, 1300.0, 1300.0, 1100.0, 1200.0 };
	float seq_time[N] = { 0.3, 0.01, 0.3, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03 };


	for (int ticks = 0; ticks < (int)(0.3 * rate); ticks++)
		add_sample(0.0);

	for (int i = 0; i < N; i++)
		for (int ticks = 0; ticks < (int)(seq_time[i] * rate); ticks++)
			add_freq(seq_freq[i]);

	for (int y = 0; y < img->height; y++) {
		// EVEN LINES
		hor_sync();
		sync_porch();
		y_scan(y);
		even_seperator();
		porch();
		v_scan(y);
		// ODD LINES
		y++;
		hor_sync();
		sync_porch();
		y_scan(y);
		odd_seperator();
		porch();
		u_scan(y);
	}

	for (int i = 0; i < RATE; i++)
		this->add_sample(0.0);

	this->sendAudio();
	close_pcm(pcm);
	close_img(img);
}

int Robot36Encoder::add_sample(float val) {
	for (int i = 0; i < channels; i++)
		buff[i] = (float)SHRT_MAX * val;
	buffer[buffer_pointer] = buff[0];
	buffer_pointer++;
}

void Robot36Encoder::add_freq(float freq) {
	add_sample(nco.real());
	nco *= std::exp((std::complex<float>)(freq * hz2rad * 1i));
}


void Robot36Encoder::hor_sync() {
	for (int ticks = 0; ticks < hor_sync_len; ticks++)
		add_freq(1200.0);
}

void Robot36Encoder::sync_porch() {
	for (int ticks = 0; ticks < sync_porch_len; ticks++)
		add_freq(1500.0);
}

void Robot36Encoder::porch() {
	for (int ticks = 0; ticks < porch_len; ticks++)
		add_freq(1900.0);
}

void Robot36Encoder::even_seperator() {
	for (int ticks = 0; ticks < seperator_len; ticks++)
		add_freq(1500.0);
}

void Robot36Encoder::odd_seperator() {
	for (int ticks = 0; ticks < seperator_len; ticks++)
		add_freq(2300.0);
}

void Robot36Encoder::y_scan(int y) {
	for (int ticks = 0; ticks < y_len; ticks++) {
		float xf = fclampf((320.0 * ticks) / (float)y_len, 0.0, 319.0);
		int x0 = xf;
		int x1 = fclampf(x0 + 1, 0.0, 319.0);
		int off0 = 3 * y * img->width + 3 * x0;
		int off1 = 3 * y * img->width + 3 * x1;
		float R0 = linear(img->pixel[off0 + 0]);
		float G0 = linear(img->pixel[off0 + 1]);
		float B0 = linear(img->pixel[off0 + 2]);
		float R1 = linear(img->pixel[off1 + 0]);
		float G1 = linear(img->pixel[off1 + 1]);
		float B1 = linear(img->pixel[off1 + 2]);
		uint8_t R = srgb(flerpf(R0, R1, xf - (float)x0));
		uint8_t G = srgb(flerpf(G0, G1, xf - (float)x0));
		uint8_t B = srgb(flerpf(B0, B1, xf - (float)x0));
		add_freq(1500.0 + 800.0 * Y_RGB(R, G, B) / 255.0);
	}
}

void Robot36Encoder::v_scan(int y) {
	for (int ticks = 0; ticks < uv_len; ticks++) {
		float xf = fclampf((160.0 * ticks) / (float)uv_len, 0.0, 159.0);
		int x0 = xf;
		int x1 = fclampf(x0 + 1, 0.0, 159.0);
		int evn0 = 3 * y * img->width + 6 * x0;
		int evn1 = 3 * y * img->width + 6 * x1;
		int odd0 = 3 * (y + 1) * img->width + 6 * x0;
		int odd1 = 3 * (y + 1) * img->width + 6 * x1;
		float R0 = (linear(img->pixel[evn0 + 0]) + linear(img->pixel[odd0 + 0]) + linear(img->pixel[evn0 + 3]) + linear(img->pixel[odd0 + 3])) / 4;
		float G0 = (linear(img->pixel[evn0 + 1]) + linear(img->pixel[odd0 + 1]) + linear(img->pixel[evn0 + 4]) + linear(img->pixel[odd0 + 4])) / 4;
		float B0 = (linear(img->pixel[evn0 + 2]) + linear(img->pixel[odd0 + 2]) + linear(img->pixel[evn0 + 5]) + linear(img->pixel[odd0 + 5])) / 4;
		float R1 = (linear(img->pixel[evn1 + 0]) + linear(img->pixel[odd1 + 0]) + linear(img->pixel[evn1 + 3]) + linear(img->pixel[odd1 + 3])) / 4;
		float G1 = (linear(img->pixel[evn1 + 1]) + linear(img->pixel[odd1 + 1]) + linear(img->pixel[evn1 + 4]) + linear(img->pixel[odd1 + 4])) / 4;
		float B1 = (linear(img->pixel[evn1 + 2]) + linear(img->pixel[odd1 + 2]) + linear(img->pixel[evn1 + 5]) + linear(img->pixel[odd1 + 5])) / 4;
		uint8_t R = srgb(flerpf(R0, R1, xf - (float)x0));
		uint8_t G = srgb(flerpf(G0, G1, xf - (float)x0));
		uint8_t B = srgb(flerpf(B0, B1, xf - (float)x0));
		add_freq(1500.0 + 800.0 * V_RGB(R, G, B) / 255.0);
	}
}

void Robot36Encoder::u_scan(int y) {
	for (int ticks = 0; ticks < uv_len; ticks++) {
		float xf = fclampf((160.0 * ticks) / (float)uv_len, 0.0, 159.0);
		int x0 = xf;
		int x1 = fclampf(x0 + 1, 0.0, 159.0);
		int evn0 = 3 * (y - 1) * img->width + 6 * x0;
		int evn1 = 3 * (y - 1) * img->width + 6 * x1;
		int odd0 = 3 * y * img->width + 6 * x0;
		int odd1 = 3 * y * img->width + 6 * x1;
		float R0 = (linear(img->pixel[evn0 + 0]) + linear(img->pixel[odd0 + 0]) + linear(img->pixel[evn0 + 3]) + linear(img->pixel[odd0 + 3])) / 4;
		float G0 = (linear(img->pixel[evn0 + 1]) + linear(img->pixel[odd0 + 1]) + linear(img->pixel[evn0 + 4]) + linear(img->pixel[odd0 + 4])) / 4;
		float B0 = (linear(img->pixel[evn0 + 2]) + linear(img->pixel[odd0 + 2]) + linear(img->pixel[evn0 + 5]) + linear(img->pixel[odd0 + 5])) / 4;
		float R1 = (linear(img->pixel[evn1 + 0]) + linear(img->pixel[odd1 + 0]) + linear(img->pixel[evn1 + 3]) + linear(img->pixel[odd1 + 3])) / 4;
		float G1 = (linear(img->pixel[evn1 + 1]) + linear(img->pixel[odd1 + 1]) + linear(img->pixel[evn1 + 4]) + linear(img->pixel[odd1 + 4])) / 4;
		float B1 = (linear(img->pixel[evn1 + 2]) + linear(img->pixel[odd1 + 2]) + linear(img->pixel[evn1 + 5]) + linear(img->pixel[odd1 + 5])) / 4;
		uint8_t R = srgb(flerpf(R0, R1, xf - (float)x0));
		uint8_t G = srgb(flerpf(G0, G1, xf - (float)x0));
		uint8_t B = srgb(flerpf(B0, B1, xf - (float)x0));
		add_freq(1500.0 + 800.0 * U_RGB(R, G, B) / 255.0);
	}
}

void Robot36Encoder::transmit(std::string image_filename, std::string source) {
	this->encode(image_filename, source);
}

void Robot36Encoder::sendAudio() {
	auto ptt = new PTT();
	ptt->push();
	write_pcm(pcm, buffer, buffer_pointer);
	ptt->release();
}
