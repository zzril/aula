#ifndef MIXER_H
#define MIXER_H
// --------

#include "instrument.h"

// --------

struct Mixer;

// --------

typedef struct Mixer Mixer;

// --------

struct Mixer {
	float* buffer;
	size_t num_samples;
};

// --------

int Mixer_init_at(Mixer* mixer);

int Mixer_clear_buffer(Mixer* mixer);

void Mixer_destroy_at(Mixer* mixer);

int Mixer_add_notes_from_instrument(Mixer* mixer, Instrument* instrument);

int Mixer_fill_with_notes_from_instruments(Mixer* mixer, Instrument** instruments, size_t num_instruments);

// --------
#endif

