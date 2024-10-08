#include <math.h>

#include "config.h"
#include "note.h"

// --------

void Note_init_at(Note* note, int pitch, int length) {
	note->pitch = pitch;
	note->length = length;
}

float Note_get_frequency(const Note* note) {
	return BASE_FREQUENCY * (pow(2, (note->pitch) / 12.0));
}

float Note_get_absolute_length(const Note* note) {
	return pow(2, note->length);
}

int Note_get_length_in_samples(const Note* note) {
	return (int) (SAMPLE_FRAMES_PER_BAR * (pow(2, note->length)));
}

