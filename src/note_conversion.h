#ifndef NOTE_CONVERSION_H
#define NOTE_CONVERSION_H
// --------

#include <stdint.h>

#include "note.h"

// --------

int Convert_musical_to_pitch(char symbol, int8_t octave, int8_t half, int8_t* pitch);

// --------
#endif

