#ifndef INSTRUMENT_H
#define INSTRUMENT_H
// --------

#include <stddef.h>

#include "note.h"

// --------

struct Instrument;

// --------

typedef struct Instrument Instrument;

typedef int (*NoteProvider)(void*, Note*);

// --------

struct Instrument {
	void* instrument_definition;
	float* buffer;
	size_t num_samples;
	size_t buffer_position;
};

// --------

int Instrument_init_at(Instrument* instrument, void* instrument_definition);

void reset_buffer(Instrument* instrument);

void Instrument_destroy_at(Instrument* instrument);

int Instrument_add_notes_for_bar(Instrument* instrument, NoteProvider note_provider, void* arg);

// --------
#endif

