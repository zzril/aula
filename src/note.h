#ifndef NOTE_H
#define NOTE_H
// --------

#define BASE_FREQUENCY 440.0

// --------

struct Note;

// --------

typedef struct Note Note;

// --------

struct Note {
	int pitch;
	int length;
};

// --------

void Note_init_at(Note* note, int pitch);

float Note_get_frequency(const Note* note);
float Note_get_absolute_length(const Note* note);
int Note_get_length_in_samples(const Note* note);

// --------
#endif

