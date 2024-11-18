#ifndef NOTE_H
#define NOTE_H
// --------

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// --------

#define BASE_FREQUENCY 440.0

// --------

struct Note;

// --------

typedef struct Note Note;

// --------

struct Note {
	int8_t pitch;
	int8_t length;
	bool is_rest;
};

// --------

void Note_init_at(Note* note, int8_t pitch, int8_t length);
void Note_init_rest_at(Note* note, int8_t length);

bool Note_is_rest(const Note* note);
float Note_get_frequency(const Note* note);
float Note_get_absolute_length(const Note* note);
size_t Note_get_length_in_samples(const Note* note);
size_t Note_get_granularity_in_samples(const Note* note);

// --------
#endif

