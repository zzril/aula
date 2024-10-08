#ifndef NOTE_COMPILER_H
#define NOTE_COMPILER_H
// --------

#include <stdbool.h>
#include <stddef.h>

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

struct NoteCompiler {
	char* bar;
	NoteCompilerState state;
	size_t bar_length;
	size_t position;
	char symbol;
	bool finished;
	bool error;
};

// --------

int NoteCompiler_init_at(NoteCompiler* compiler, char* bar, size_t length);

int NoteCompiler_get_next_note(void* compiler, Note* note);

// --------
#endif

