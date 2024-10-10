#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "config.h"
#include "instrument.h"
#include "wave.h"

// --------

static int add_note(Instrument* instrument, Note* note);
static bool is_buffer_full(Instrument* instrument);

// --------

static int add_note(Instrument* instrument, Note* note) {

	size_t samples_to_write;
	size_t remaining_buffer_space;
	bool overflow;

	if(note == NULL) {
		return 2;
	}

	if(is_buffer_full(instrument)) {
		return 3;
	}

	overflow = false;

	remaining_buffer_space = instrument->num_samples - instrument->buffer_position;
	samples_to_write = Note_get_length_in_samples(note);

	if(samples_to_write > remaining_buffer_space) {
		samples_to_write = remaining_buffer_space;
		overflow = true;
	}

	if(!Note_is_rest(note)) {
		add_sine(instrument->buffer + instrument->buffer_position, samples_to_write, SAMPLE_FRAMES_PER_SECOND, 1, Note_get_frequency(note), 0);
	}

	instrument->buffer_position += samples_to_write;

	return overflow? 1: 0;
}

static bool is_buffer_full(Instrument* instrument) {
	return instrument->buffer_position >= instrument->num_samples;
}

// --------

int Instrument_init_at(Instrument* instrument, void* instrument_definition) {

	instrument->instrument_definition = instrument_definition;
	instrument->num_samples = SAMPLE_FRAMES_PER_BAR;

	instrument->buffer = reallocarray(NULL, instrument->num_samples, sizeof(float));
	if(instrument->buffer == NULL) {
		perror("malloc");
		return -1;
	}

	reset_buffer(instrument);

	return 0;
}

void reset_buffer(Instrument* instrument) {
	generate_silence(instrument->buffer, instrument->num_samples);
	instrument->buffer_position = 0;
	return;
}

void Instrument_destroy_at(Instrument* instrument) {

	if(instrument == NULL) {
		return;
	}

	free(instrument->buffer);
	memset(instrument, 0, sizeof(Instrument));

	return;
}

int Instrument_add_notes_for_bar(Instrument* instrument, NoteProvider note_provider, void* arg) {

	Note note;
	int status = 0;

	while((status = note_provider(arg, &note)) == 0) {

		int rv = add_note(instrument, &note);

		if(rv != 0) {
			if(rv == 1) {
				fputs("WARNING: Instrument buffer full, cutting off remaining note(s)\n", stderr);
				return 0;
			}
			else {
				return rv;
			}
		}
	}

	if(!is_buffer_full(instrument)) {
		return status;
	}

	return 0;
}

