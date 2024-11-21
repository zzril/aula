#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "config.h"
#include "error_codes.h"
#include "instrument.h"
#include "wave.h"

// --------

static int add_note(Instrument* instrument, Note* note, bool* done);
static bool is_buffer_full(Instrument* instrument);
static size_t update_granularity(Instrument* instrument, const Note* note);

// --------

static int add_note(Instrument* instrument, Note* note, bool* done) {

	size_t remaining_buffer_space;
	size_t samples_to_write;


	if(instrument == NULL || note == NULL || done == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(is_buffer_full(instrument)) {
		*done = true;
		return ERROR_CODE_INSTRUMENT_BUFFER_OVERFLOW;
	}

	*done = false;

	remaining_buffer_space = instrument->num_samples - instrument->buffer_position;
	samples_to_write = Note_get_length_in_samples(note);

	update_granularity(instrument, note);

	if(samples_to_write > remaining_buffer_space) {

		if(samples_to_write - remaining_buffer_space >= instrument->granularity / 2) {
			*done = true;
			return ERROR_CODE_INSTRUMENT_BUFFER_OVERFLOW;
		}

		samples_to_write = remaining_buffer_space;
	}

	if(!Note_is_rest(note)) {
		add_sine(instrument->buffer + instrument->buffer_position, samples_to_write, Config_get_framerate(), 1, Note_get_frequency(note), 0);
	}

	instrument->buffer_position += samples_to_write;

	if(is_buffer_full(instrument)) {
		*done = true;
	}

	return 0;
}

static bool is_buffer_full(Instrument* instrument) {
	return instrument->buffer_position >= instrument->num_samples || instrument->num_samples - instrument->buffer_position < instrument->granularity;
}

static size_t update_granularity(Instrument* instrument, const Note* note) {

	size_t note_granularity = Note_get_granularity_in_samples(note);

	if(note_granularity < instrument->granularity) {
		instrument->granularity = note_granularity;
	}

	return instrument->granularity;
}

// --------

int Instrument_init_at(Instrument* instrument, void* instrument_definition) {

	instrument->instrument_definition = instrument_definition;
	instrument->num_samples = Config_get_sample_frames_per_bar();

	instrument->buffer = reallocarray(NULL, instrument->num_samples, sizeof(float));
	if(instrument->buffer == NULL) {
		perror("malloc");
		return ERROR_CODE_MALLOC_FAILURE;
	}

	reset_buffer(instrument);

	return 0;
}

void reset_buffer(Instrument* instrument) {
	generate_silence(instrument->buffer, instrument->num_samples);
	instrument->buffer_position = 0;
	instrument->granularity = instrument->num_samples;
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
	bool provider_done = false;
	bool instrument_done = false;

	while((status = note_provider(arg, &note, &provider_done)) == 0 && !provider_done && !instrument_done) {

		int rv = add_note(instrument, &note, &instrument_done);
		if(rv != 0) {
			return rv;
		}
	}

	if(status != 0) {
		return ERROR_CODE_NOTE_COMPILER_ERROR;
	}

	if(!instrument_done) {
		return ERROR_CODE_BAR_TOO_SHORT;
	}

	if(!provider_done) {
		return ERROR_CODE_BAR_TOO_LONG;
	}

	return 0;
}

