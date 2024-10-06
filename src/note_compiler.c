#include <stdlib.h>

#include <string.h>

#include "note_conversion.h"
#include "note_compiler.h"

// --------

static int advance(NoteCompiler* compiler);

static bool is_ignorable(char symbol);

static int peek(NoteCompiler* compiler);

static int finish_note(NoteCompiler* compiler, Note* note);

// --------

static int advance(NoteCompiler* compiler) {

	if(compiler->position >= compiler->bar_length) {
		compiler->finished = true;
		return -1;
	}

	compiler->symbol = (compiler->bar)[compiler->position];
	(compiler->position)++;

	return (char) (compiler->symbol);
}

static bool is_ignorable(char symbol) {
	switch(symbol) {
		case ' ':
			return true;
		default:
			return false;
	}
}

static int peek(NoteCompiler* compiler) {

	if(compiler->position >= compiler->bar_length) {
		return -1;
	}

	return (char) ((compiler->bar)[compiler->position]);
}

static int finish_note(NoteCompiler* compiler, Note* note) {

	char symbol = compiler->symbol;
	int half = 0;
	int c = peek(compiler);

	if(c < 0) {
		compiler->finished = true;
	}

	if(!compiler->finished) {

		switch((char) c) {

			case '#':
				advance(compiler);
				half++;
				break;

			default:
				break;
		}
	}

	return Note_musical_init_at(note, symbol, half);
}

// --------

int NoteCompiler_init_at(NoteCompiler* compiler, char* bar, size_t length) {

	compiler->bar = bar;
	compiler->bar_length = 0;
	compiler->position = 0;
	compiler->symbol = '\0';
	compiler->finished = false;
	compiler->error = false;

	compiler->bar = bar;
	compiler->bar_length = length;

	return 0;
}

int NoteCompiler_get_next_note(void* compiler, Note* note) {

	NoteCompiler* comp = (NoteCompiler*) compiler;
	int status = 0;

	memset(note, 0, sizeof(Note));

	if((comp == NULL || note == NULL) || (comp->finished | comp->error)) {
		return 1;
	}

	do {
		advance(comp);
	}
	while(!comp->finished && is_ignorable(comp->symbol));

	if(comp->finished) {
		return 1;
	}

	switch(comp->symbol) {
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
			status = finish_note(comp, note);
			comp->error = status != 0;
			return status;
		default:
			comp->error = true;
			comp->finished = true;
			return 1;
	}
}

