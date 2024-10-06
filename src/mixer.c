#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "config.h"
#include "mixer.h"
#include "wave.h"

// --------

int Mixer_init_at(Mixer* mixer) {

	mixer->num_samples = SAMPLE_FRAMES_PER_BAR;

	mixer->buffer = reallocarray(NULL, mixer->num_samples, sizeof(float));
	if(mixer->buffer == NULL) {
		perror("malloc");
		return -1;
	}

	Mixer_clear_buffer(mixer);

	return 0;
}

int Mixer_clear_buffer(Mixer* mixer) {

	if(mixer == NULL || mixer->buffer == NULL) {
		return 1;
	}

	generate_silence(mixer->buffer, mixer->num_samples);

	return 0;
}

void Mixer_destroy_at(Mixer* mixer) {

	if(mixer == NULL) {
		return;
	}

	free(mixer->buffer);
	memset(mixer, 0, sizeof(Mixer));

	return;
}

int Mixer_add_notes_from_instrument(Mixer* mixer, Instrument* instrument) {

	if(mixer == NULL || mixer->buffer == NULL || instrument == NULL || instrument->buffer == NULL) {
		return 1;
	}

	if(mixer->num_samples != instrument->num_samples) {
		return 2;
	}

	for(size_t t = 0; t < mixer->num_samples; t++) {
		(mixer->buffer)[t] += (instrument->buffer)[t];
	}

	reset_buffer(instrument);

	return 0;
}

int Mixer_fill_with_notes_from_instruments(Mixer* mixer, Instrument** instruments, size_t num_instruments) {

	int status = Mixer_clear_buffer(mixer);

	for(size_t i = 0; status == 0 && i < num_instruments; i++) {
		status = Mixer_add_notes_from_instrument(mixer, instruments[i]);
	}

	return status;
}

