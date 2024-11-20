#ifndef NOTE_COMPILER_H
#define NOTE_COMPILER_H
// --------

#include <stdbool.h>

#include "bar_token.h"
#include "note.h"

// --------

struct NoteCompiler;

// --------

typedef struct NoteCompiler NoteCompiler;

// --------

typedef enum NoteCompilerState {
	NOTE_COMPILER_STATE_EXPECTING_NOTE = 0,
	NUM_NOTE_COMPILER_STATES,
} NoteCompilerState;

typedef enum NoteCompilerErrorState {
	NOTE_COMPILER_ERROR_STATE_UNKNOWN_ERROR = 0,
	NOTE_COMPILER_ERROR_STATE_UNEXPECTED_CHARACTER,
} NoteCompilerErrorState;

struct NoteCompiler {
	char* filename;
	BarToken* bar;
	size_t position;
	NoteCompilerState state;
	NoteCompilerErrorState error_state;
	char symbol;
	bool finished;
	bool error;
};

// --------

int NoteCompiler_init_at(NoteCompiler* compiler, BarToken* bar, char* filename);

int NoteCompiler_get_next_note(void* compiler, Note* note, bool* finished);

// --------
#endif

