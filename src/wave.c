#include <math.h>

#include "wave.h"

// --------

void generate_silence(float* buffer, size_t num_samples) {

	for(size_t t = 0; t < num_samples; t++)  {
		buffer[t] = 0;
	}

	return;
}

void add_sine(float* buffer, size_t num_samples, int sampling_rate, float amplitude, float frequency, float arg_off) {

	float factor = (2 * M_PI * frequency) / sampling_rate;
	float arg = arg_off;

	while(arg < 0) {
		arg += 2 * M_PI;
	}
	while(arg > 2 * M_PI) {
		arg -= 2 * M_PI;
	}

	for(size_t t = 0; t < num_samples; t++) {
		buffer[t] += amplitude * sinf(arg);
		arg += factor;
		while(arg > 2 * M_PI) {
			arg -= 2 * M_PI;
		}
	}

	return;
}

