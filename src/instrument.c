#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "config.h"
#include "error_codes.h"
#include "instrument.h"
#include "wave.h"

// --------

static int add_note(Instrument* instrument, Note* note, bool* done, bool* cut_off);
static bool is_buffer_full(Instrument* instrument);

// --------

static int add_note(Instrument* instrument, Note* note, bool* done, bool* cut_off) {

	size_t samples_to_write;
	size_t remaining_buffer_space;

	if(instrument == NULL || note == NULL || done == NULL || cut_off == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(is_buffer_full(instrument)) {
		*done = true;
		return ERROR_CODE_INSTRUMENT_BUFFER_OVERFLOW;
	}

	*done = false;
	*cut_off = false;

	remaining_buffer_space = instrument->num_samples - instrument->buffer_position;
	samples_to_write = Note_get_length_in_samples(note);

	if(samples_to_write > remaining_buffer_space) {
		samples_to_write = remaining_buffer_space;
		*cut_off = true;
	}

	if(samples_to_write == remaining_buffer_space) {
		*done = true;
	}

	if(!Note_is_rest(note)) {
		add_sine(instrument->buffer + instrument->buffer_position, samples_to_write, SAMPLE_FRAMES_PER_SECOND, 1, Note_get_frequency(note), 0);
	}

	instrument->buffer_position += samples_to_write;

	return 0;
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
		return ERROR_CODE_MALLOC_FAILURE;
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
	bool provider_done = false;
	bool instrument_done = false;
	bool cut_off = false;

	while((status = note_provider(arg, &note, &provider_done)) == 0 && !provider_done && !instrument_done) {

		int rv = add_note(instrument, &note, &instrument_done, &cut_off);

		if(rv != 0) {
			return rv;
		}

		if(cut_off) {
			fputs("WARNING: Instrument buffer full, cutting off note.\n", stderr);
		}
	}

	if(status != 0) {
		return status;
	}

	if(!instrument_done) {
		return ERROR_CODE_BAR_TOO_SHORT;
	}

	if(!is_buffer_full(instrument)) {
		fputs("WARNING: Instrument buffer not completely filled, making up with silence.\n", stderr);
	}

	if(!provider_done) {
		return ERROR_CODE_BAR_TOO_LONG;
	}

	return 0;
}

