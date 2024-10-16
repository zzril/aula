#include <stdlib.h>

#include "error_codes.h"
#include "note_conversion.h"

// --------

int Convert_musical_to_pitch(char symbol, int8_t octave, int8_t half, int8_t* pitch) {

	if(pitch == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(half < -1 || 1 < half) {
		return ERROR_CODE_INVALID_ARGUMENT;
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
			return ERROR_CODE_UNEXPECTED_CHARACTER;
	}

	*pitch += 12 * octave;
	*pitch += half;

	return 0;
}

