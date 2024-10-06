#ifndef NOTE_CONVERSION_H
#define NOTE_CONVERSION_H
// --------

#include "note.h"

// --------

int Note_musical_init_at(Note* note, char symbol, int half);

int Convert_musical_to_pitch(char symbol, int half, int* pitch);

// --------
#endif

