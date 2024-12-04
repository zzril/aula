#include <math.h>

#include "config.h"
#include "note.h"

// --------

static float get_absolute_granularity(const Note* note);

static size_t absolute_length_to_samples(float length);

// --------

static float get_absolute_granularity(const Note* note) {
	return pow(2, note->length - note->dots);
}

static size_t absolute_length_to_samples(float length) {
	return (size_t) (Config_get_sample_frames_per_bar() * length);
}

// --------

void Note_init_at(Note* note, int8_t pitch, int8_t length, uint8_t dots) {

	note->pitch = pitch;
	note->length = length;
	note->dots = dots;
	note->is_rest = false;

	return;
}

void Note_init_rest_at(Note* note, int8_t length, uint8_t dots) {

	note->pitch = INT8_MIN;
	note->length = length;
	note->dots = dots;
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
	return pow(2, (note->length + 1)) - pow(2, note->length - note->dots);
}

size_t Note_get_length_in_samples(const Note* note) {
	return absolute_length_to_samples(Note_get_absolute_length(note));
}

size_t Note_get_granularity_in_samples(const Note* note) {
	return absolute_length_to_samples(get_absolute_granularity(note));
}

