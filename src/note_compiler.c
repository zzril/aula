#include <stdlib.h>

#include <string.h>

#include "note_conversion.h"
#include "note_compiler.h"

// --------

static int advance(NoteCompiler* compiler);

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

static int peek(NoteCompiler* compiler) {

	if(compiler->position >= compiler->bar_length) {
		return -1;
	}

	return (char) ((compiler->bar)[compiler->position]);
}

static int finish_note(NoteCompiler* compiler, Note* note) {

	int status = 0;
	int8_t octave = 0;
	int8_t half = 0;
	int8_t pitch = 0;
	int8_t length = -2;
	bool frequence_parsing_finished = false;
	bool note_finished = false;

	char symbol = compiler->symbol;
	bool rest = compiler->symbol == ';';
	int c = peek(compiler);

	if(c < 0) {
		compiler->finished = true;
	}

	if(!rest) {

		if(!compiler->finished) {

			switch(c) {

				case ' ':
				case '\t':
					note_finished = true;
					break;

				case '#':
					advance(compiler);
					half++;
					break;

				case 'b':
					advance(compiler);
					half--;
					break;

				default:
					break;
			}
		}

		while(!frequence_parsing_finished && !compiler->finished && (c = peek(compiler)) >= 0) {

			switch(c) {

				case '+':
					advance(compiler);
					octave++;
					break;

				case '-':
					advance(compiler);
					octave--;
					break;

				default:
					frequence_parsing_finished = true;
					break;
			}
		}
	}

	while(!note_finished && !compiler->finished && (c = peek(compiler)) >= 0) {

		switch(c) {

			case 'o':
				advance(compiler);
				length++;
				continue;

			case '^':
				advance(compiler);
				length--;
				continue;

			default:
				note_finished = true;
				break;
		}
	}

	if(rest) {
		Note_init_rest_at(note, length);
		return 0;
	}

	status = Convert_musical_to_pitch(symbol,octave, half, &pitch);
	if(status != 0) {
		return status;
	}

	Note_init_at(note, pitch, length);

	return 0;
}

// --------

int NoteCompiler_init_at(NoteCompiler* compiler, char* bar, size_t length) {

	compiler->bar = bar;
	compiler->state = NOTE_COMPILER_STATE_EXPECTING_NOTE;
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

	if((comp == NULL || note == NULL) || (comp->finished | comp->error)) {
		return 1;
	}

	memset(note, 0, sizeof(Note));

	while(advance(comp) != -1) {

		switch(comp->state) {

			case NOTE_COMPILER_STATE_EXPECTING_NOTE:

				switch(comp->symbol) {

					case ' ':
					case '\n':
					case '\r':
					case '\t':
						continue;

					case 'A':
					case 'B':
					case 'C':
					case 'D':
					case 'E':
					case 'F':
					case 'G':
					case ';':
						status = finish_note(comp, note);
						comp->error = status != 0;
						return status;

					default:
						comp->error = true;
						comp->finished = true;
						return 1;
				}
			default:
				comp->error = true;
				comp->finished = true;
				return 1;
		}
	}

	comp->error = true;
	comp->finished = true;
	return 1;
}

