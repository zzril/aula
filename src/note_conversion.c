#include <stdlib.h>

#include "note_conversion.h"

// --------

int Note_musical_init_at(Note* note, char symbol, int half) {

	int pitch;

	int rv = Convert_musical_to_pitch(symbol, half, &pitch);
	if(rv != 0) {
		return rv;
	}

	Note_init_at(note, pitch);
	return 0;
}

int Convert_musical_to_pitch(char symbol, int half, int* pitch) {

	if(pitch == NULL) {
		return 1;
	}

	if(half < -1 || 1 < half) {
		return 1;
	}

	switch(symbol) {
		case 'C':
			*pitch = -9;
			break;
		case 'D':
			*pitch = -7;
			break;
		case 'E':
			*pitch = -5;
			break;
		case 'F':
			*pitch = -4;
			break;
		case 'G':
			*pitch = -2;
			break;
		case 'A':
			*pitch = 0;
			break;
		case 'B':
			*pitch = 2;
			break;
		default:
			*pitch = 0;
			return 1;
	}

	*pitch += half;
	return 0;
}

