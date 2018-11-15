#ifndef ROBOT36_DECODE_ROBOT36DECODER_HPP
#define ROBOT36_DECODE_ROBOT36DECODER_HPP

#include <string>
#include <complex>
#include "buffer.hpp"
#include "pcm.hpp"
#include "Image.hpp"
#include "ddc.hpp"

class Robot36Decoder {
public:
	void decoder(std::string image_name, std::string source = "default");
private:
	struct pcm *pcm;

	float rate;
	int channels;
	int64_t factor_L;
	int64_t factor_M;
	int out;
	float dstep;
	std::complex<float> cnt_last = -1i;
	std::complex<float> dat_last = -1i;
	std::complex<float> *cnt_q;
	std::complex<float> *dat_q;
	struct ddc *cnt_ddc;
	struct ddc *dat_ddc;
	struct buffer *buffer;
	int cnt_delay;
	int dat_delay;
	short *pcm_buff;
	int init = 0;

	int break_ticks = 0;
	int leader_ticks = 0;
	int ticks = -1;
	int got_break = 0;
	float dat_avg = 1900.0;

	int demodulate(float *cnt_freq, float *dat_freq, float *drate);

	void process_line(uint8_t *pixel, uint8_t *y_pixel, uint8_t *uv_pixel, int y_width, int uv_width, int width, int height,
	                  int n);

	int cal_header(float cnt_freq, float dat_freq, float drate);

	int vis_code(int *reset, int *code, float cnt_freq, float drate);

	int decode(int *reset, Image **img, std::string img_name, float cnt_freq, float dat_freq, float drate);
};


#endif //ROBOT36_DECODE_ROBOT36DECODER_HPP
