#ifndef ROBOT36_DECODE_ROBOT36_HPP
#define ROBOT36_DECODE_ROBOT36_HPP

#include <string>
#include "pcm.hpp"
#include "Image.hpp"
#include <complex>

class Robot36Encoder {
public:
	void transmit(std::string image_filename, std::string source = "default");
	void encode(std::string image_filename, std::string source = "default");
private:
	struct pcm *pcm;
	int rate;
	int channels;
	short *buff;
	double hz2rad;
	std::complex<float> nco;
	double hor_sync_len;
	double sync_porch_len;
	double porch_len;
	double seperator_len;
	double y_len;
	double uv_len;
	struct Image *img;

	//Constants
	const double sync_porch_sec = 0.003l;
	const double porch_sec = 0.0015l;
	const double y_sec = 0.088l;
	const double uv_sec = 0.044l;
	const double hor_sync_sec = 0.009l;
	const double seperator_sec = 0.0045l;

	int add_sample(float val);
	void add_freq(float freq);
	void hor_sync();
	void sync_porch();
	void porch();
	void even_seperator();
	void odd_seperator();
	void y_scan(int y);
	void v_scan(int y);
	void u_scan(int y);
};


#endif //ROBOT36_DECODE_ROBOT36_HPP
