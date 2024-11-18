#include <math.h>

#include "config.h"
#include "note.h"

// --------

void Note_init_at(Note* note, int8_t pitch, int8_t length) {

	note->pitch = pitch;
	note->length = length;
	note->is_rest = false;

	return;
}

void Note_init_rest_at(Note* note, int8_t length) {

	note->pitch = INT8_MIN;
	note->length = length;
	note->is_rest = true;

	return;
}

bool Note_is_rest(const Note* note) {
	return note->is_rest;
}

float Note_get_frequency(const Note* note) {

	if(note->is_rest) {
		return 0;
	}

	return BASE_FREQUENCY * (pow(2, (note->pitch) / 12.0));
}

float Note_get_absolute_length(const Note* note) {
	return pow(2, note->length);
}

size_t Note_get_length_in_samples(const Note* note) {
	return (size_t) (Config_get_sample_frames_per_bar() * Note_get_absolute_length(note));
}

size_t Note_get_granularity_in_samples(const Note* note) {
	return Note_get_length_in_samples(note);
}

