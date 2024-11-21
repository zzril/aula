#include <stdlib.h>

#include <string.h>

#include "error_codes.h"
#include "error_messages.h"
#include "note_conversion.h"
#include "note_compiler.h"

// --------

#define END_OF_BAR EOF

// --------

static int advance(NoteCompiler* compiler);

static int peek(NoteCompiler* compiler);

static int finish_note(NoteCompiler* compiler, Note* note);

// --------

static int advance(NoteCompiler* compiler) {

	if(compiler->position >= compiler->bar->content_length) {
		compiler->finished = true;
		return END_OF_BAR;
	}

	compiler->symbol = (compiler->bar->content)[compiler->position];
	(compiler->position)++;

	return (char) (compiler->symbol);
}

static int peek(NoteCompiler* compiler) {

	if(compiler->position >= compiler->bar->content_length) {
		return END_OF_BAR;
	}

	return (char) ((compiler->bar->content)[compiler->position]);
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

int NoteCompiler_init_at(NoteCompiler* compiler, BarToken* bar, char* filename) {

	if(compiler == NULL || bar == NULL || bar->content == NULL || bar->content_length == 0) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	compiler->bar = bar;
	compiler->position = 0;
	compiler->state = NOTE_COMPILER_STATE_EXPECTING_NOTE;
	compiler->error_state = NOTE_COMPILER_ERROR_STATE_UNKNOWN_ERROR;
	compiler->symbol = '\0';
	compiler->finished = false;
	compiler->error = false;

	compiler->filename = filename == NULL? "": filename;

	return 0;
}

int NoteCompiler_get_next_note(void* compiler, Note* note, bool* finished) {

	NoteCompiler* comp = (NoteCompiler*) compiler;
	int status = 0;

	if(finished != NULL) {
		*finished = false;
	}

	if(comp == NULL || note == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(comp->finished | comp->error) {
		return ERROR_CODE_INVALID_STATE;
	}

	memset(note, 0, sizeof(Note));

	while(advance(comp) != END_OF_BAR) {

		switch(comp->state) {

			case NOTE_COMPILER_STATE_EXPECTING_NOTE:

				switch(comp->symbol) {

					case '\0':
					case '\t':
					case '\n':
					case '\r':
					case ' ':
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
						comp->error_state = NOTE_COMPILER_ERROR_STATE_UNEXPECTED_CHARACTER;
						return ERROR_CODE_UNEXPECTED_CHARACTER;
				}
			default:
				comp->error = true;
				comp->finished = true;
				return ERROR_CODE_INVALID_STATE;
		}
	}

	if(finished != NULL) {
		*finished = true;
	}
	comp->finished = true;
	return 0;
}

int NoteCompiler_print_error(const NoteCompiler* compiler, FILE* stream) {

	int status = 0;

	if(compiler == NULL || !(compiler->error)) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(compiler->bar == NULL || compiler->bar->content == NULL) {
		return ERROR_CODE_INVALID_STATE;
	}

	fprintf(stream, "%s:%u:%u: ", compiler->filename, compiler->bar->line, compiler->bar->col);

	switch(compiler->error_state) {

		case NOTE_COMPILER_ERROR_STATE_UNEXPECTED_CHARACTER:

			fputs(get_error_message(ERROR_CODE_UNEXPECTED_CHARACTER), stream);
			fputs(":\n", stream);

			BarToken_print(compiler->bar, stream);

			if(fprintf(stream, "\n%*s^", (int) (compiler->position - 1), " ") < 0) {
				return ERROR_CODE_UNKNOWN_SYSTEM_ERROR;
			}

			break;

		default:

			fputs("Unknown error when reading bar:\n", stream);
			BarToken_print(compiler->bar, stream);

			break;
	}

	fputs("\n", stream);

	return status;
}

