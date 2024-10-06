#ifndef WAVE_H
#define WAVE_H
// --------

#include <stddef.h>

// --------

void generate_silence(float* buffer, size_t num_samples);

void add_sine(float* buffer, size_t num_samples, int sampling_rate, float amplitude, float frequency, float arg_off);

// --------
#endif

