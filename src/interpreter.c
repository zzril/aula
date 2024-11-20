#include <stdlib.h>

#include <string.h>

#include "error_codes.h"
#include "config.h"
#include "instrument.h"
#include "interpreter.h"
#include "lexer.h"
#include "mixer.h"
#include "note_compiler.h"
#include "player.h"
#include "track_lexer.h"

// --------

static bool destroy_token(Token* token);
static bool destroy_bar_token(BarToken* bar);

static int play_track_token(const Interpreter* interpreter, Token* track);

static int play_bar_token(const Interpreter* interpreter, Player* player, Instrument* instrument, BarToken* bar);

// --------

static bool destroy_token(Token* token) {
	Token_destroy_at(token);
	return true;
}

static bool destroy_bar_token(BarToken* bar) {
	BarToken_destroy_at(bar);
	return true;
}
static int play_track_token(const Interpreter* interpreter, Token* track) {

	if(track == NULL || track->type != TOKEN_TRACK) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	TrackLexer lexer;
	BarToken bar;
	Player player;
	Instrument instrument;

	int status = 0;

	memset(&lexer, 0, sizeof(Lexer));
	memset(&bar, 0, sizeof(BarToken));
	memset(&player, 0, sizeof(Player));
	memset(&instrument, 0, sizeof(Instrument));

	status = TrackLexer_init_at(&lexer, track);
	if(status != 0) {
		return status;
	}

	status = Player_init_at(&player);
	if(status != 0) {
		TrackLexer_destroy_at(&lexer);
		return status;
	}

	status = Instrument_init_at(&instrument, NULL);
	if(status != 0) {
		Player_destroy_at(&player);
		TrackLexer_destroy_at(&lexer);
		return status;
	}

	while(status == 0 && destroy_bar_token(&bar) && !lexer.super.finished && (status = TrackLexer_get_next_bar(&lexer, &bar)) == 0) {
		status = play_bar_token(interpreter, &player, &instrument, &bar);
	}

	if(status == 0) {
		Player_finish(&player);
	}

	Instrument_destroy_at(&instrument);
	Player_destroy_at(&player);
	destroy_bar_token(&bar);
	TrackLexer_destroy_at(&lexer);

	return status;
}

static int play_bar_token(const Interpreter* interpreter, Player* player, Instrument* instrument, BarToken* bar) {

	NoteCompiler compiler;
	int status;

	Instrument* instruments[] = {instrument};

	if(bar == NULL || bar->content == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	NoteCompiler_init_at(&compiler, (char*) (bar->content), bar->content_length, interpreter->filename);

	status = Instrument_add_notes_for_bar(instrument, &NoteCompiler_get_next_note, &compiler);
	if(status != 0) {
		return status;
	}

	status = Mixer_fill_with_notes_from_instruments(&(player->mixer), instruments, 1);
	if(status != 0) {
		return status;
	}

	return Player_play_bar(player);
}

// --------

void Interpreter_init_at(Interpreter* interpreter) {

	interpreter->filename = "";
	interpreter->state = INTERPRETER_STATE_EXPECTING_KEYWORD;
	interpreter->error_state = INTERPRETER_ERROR_STATE_UNKNOWN_ERROR;
	interpreter->finished = false;
	interpreter->error = false;

	return;
}

int Interpreter_interpret(Interpreter* interpreter, FILE* stream) {

	Lexer lexer;
	Token token;

	int status = 0;

	memset(&lexer, 0, sizeof(Lexer));
	memset(&token, 0, sizeof(Token));

	status = Lexer_init_at(&lexer, stream);
	if(status != 0) {
		interpreter->error = true;
		interpreter->finished = true;
		return status;
	}

	while(!interpreter->finished && destroy_token(&token) && (status = Lexer_get_next_token(&lexer, &token)) == 0 && !lexer.super.finished) {

		if(token.type == TOKEN_COMMENT) {
			continue;
		}

		switch(interpreter->state) {

			case INTERPRETER_STATE_EXPECTING_KEYWORD:

				switch(token.type) {

					case TOKEN_KEYWORD_BPM:
						interpreter->state = INTERPRETER_STATE_EXPECTING_TEMPO;
						continue;

					case TOKEN_KEYWORD_TRACK:
						interpreter->state = INTERPRETER_STATE_EXPECTING_TRACK;
						continue;

					default:
						interpreter->error = true;
						interpreter->finished = true;
						interpreter->error_state = INTERPRETER_ERROR_STATE_UNEXPECTED_TOKEN;
						continue;
				}

			case INTERPRETER_STATE_EXPECTING_TEMPO:

				switch(token.type) {

					case TOKEN_LITERAL_INTEGER:
						if((status = Config_set_bpm(token.content.integer)) != 0) {
							interpreter->finished = true;
							interpreter->error = true;
							interpreter->error_state = INTERPRETER_ERROR_STATE_INTERNAL_ERROR;
							continue;
						}
						interpreter->state = INTERPRETER_STATE_EXPECTING_KEYWORD;
						continue;

					default:
						interpreter->error = true;
						interpreter->finished = true;
						interpreter->error_state = INTERPRETER_ERROR_STATE_UNEXPECTED_TOKEN;
						continue;
				}

			case INTERPRETER_STATE_EXPECTING_TRACK:

				switch(token.type) {

					case TOKEN_TRACK:

						if((status = play_track_token(interpreter, &token)) != 0) {
							interpreter->finished = true;
							interpreter->error = true;
							interpreter->error_state = INTERPRETER_ERROR_STATE_INTERNAL_ERROR;
							continue;
						}

						interpreter->finished = true;
						continue;

					default:
						interpreter->error = true;
						interpreter->finished = true;
						interpreter->error_state = INTERPRETER_ERROR_STATE_UNEXPECTED_TOKEN;
						continue;
				}

			default:
				interpreter->finished = true;
				interpreter->error = true;
				continue;
		}
	}

	if(lexer.super.error) {
		fprintf(stderr, "Error at %s:%u:%u\n", interpreter->filename, lexer.super.line, lexer.super.col);
		status = status != 0? status: ERROR_CODE_UNKNOWN_ERROR;
	}

	if(interpreter->error) {

		switch(interpreter->error_state) {

			case INTERPRETER_ERROR_STATE_UNEXPECTED_TOKEN:

				fprintf(stderr, "%s:%u:%u: Unexpected token: ", interpreter->filename, token.line, token.col);
				Token_print(&token, stderr);
				fputs("\n", stderr);

				break;

			case INTERPRETER_ERROR_STATE_INTERNAL_ERROR:

				fprintf(stderr, "%s:%u:%u: Internal interpreter error while processing: ", interpreter->filename, token.line, token.col);
				Token_print(&token, stderr);
				fputs("\n", stderr);

				break;

			default:
				fprintf(stderr, "Unknown interpreter error\n");
				break;
		}

		status = status != 0? status: ERROR_CODE_UNKNOWN_ERROR;
	}

	destroy_token(&token);

	return status;
}

int Interpreter_interpret_file(Interpreter* interpreter, char* filename) {

	FILE* stream = fopen(filename, "r");
	if(stream == NULL) {
		perror(filename);
		return ERROR_CODE_OPEN_FAILURE;
	}

	interpreter->filename = strdup(filename);

	int status = Interpreter_interpret(interpreter, stream);

	free(interpreter->filename);
	interpreter->filename = "";

	fclose(stream);

	return status;
}

