#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "error_codes.h"
#include "track_lexer.h"

// --------

static int advance(TrackLexer* lexer);
static int peek(TrackLexer* lexer);

static void update_lineinfo(TrackLexer* lexer);

static void reset_buffer_info(TrackLexer* lexer);
static void free_buffer(TrackLexer* lexer);

static int append_char_to_buffer(TrackLexer* lexer, char c);
static int append_current_symbol_to_buffer(TrackLexer* lexer);
static int finalize_buffer(TrackLexer* lexer);

// --------

static int advance(TrackLexer* lexer) {

	if(lexer->finished | lexer->error) {
		return EOF;
	}

	if(lexer->track_position >= lexer->track->content_length) {
		lexer->finished = true;
		return EOF;
	}

	lexer->symbol = ((char*) (lexer->track->content))[lexer->track_position];
	(lexer->track_position)++;
	(lexer->col)++;

	if(lexer->symbol == '\n') {
		lexer->col = 1;
		(lexer->line)++;
	}

	return (int) (lexer->symbol);
}

static int peek(TrackLexer* lexer) {

	if(lexer->finished | lexer->error) {
		return EOF;
	}

	size_t read_position = lexer->track_position + 1;
 
	if(read_position >= lexer->track->content_length) {
		return EOF;
	}

	return (int) ((char*) (lexer->track->content))[read_position];
}

static void update_lineinfo(TrackLexer* lexer) {

	lexer->saved_line = lexer->line;
	lexer->saved_col = lexer->col;

	return;
}

static void reset_buffer_info(TrackLexer* lexer) {

	lexer->buffer = NULL;
	lexer->buffer_capacity = 0;
	lexer->buffer_length = 0;

	return;
}

static void free_buffer(TrackLexer* lexer) {

	free(lexer->buffer);
	reset_buffer_info(lexer);

	return;
}

static int append_char_to_buffer(TrackLexer* lexer, char c) {

	if(lexer == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(lexer->buffer == NULL) {

		if(lexer->buffer_capacity != 0 || lexer->buffer_length != 0) {
			lexer->error = true;
			lexer->finished = true;
			return ERROR_CODE_INVALID_STATE;
		}

		lexer->buffer = malloc(lexer->initial_buffer_capacity);
		if(lexer->buffer == NULL) {
			lexer->error = true;
			lexer->finished = true;
			return ERROR_CODE_MALLOC_FAILURE;
		}

		lexer->buffer_capacity = lexer->initial_buffer_capacity;
	}

	else if(lexer->buffer_capacity == 0 || lexer->buffer_length == 0) {
		lexer->error = true;
		lexer->finished = true;
		return ERROR_CODE_INVALID_STATE;
	}

	if(lexer->buffer_length >= lexer->buffer_capacity) {

		size_t new_capacity = lexer->buffer_capacity * 2;
		if(new_capacity <= lexer->buffer_capacity) {
			lexer->error = true;
			lexer->finished = true;
			return ERROR_CODE_INTEGER_OVERFLOW;
		}

		char* new_buffer = realloc(lexer->buffer, new_capacity);
		if(new_buffer == NULL) {
			lexer->error = true;
			lexer->finished = true;
			return ERROR_CODE_MALLOC_FAILURE;
		}

		lexer->buffer = new_buffer;
		lexer->buffer_capacity = new_capacity;
	}

	lexer->buffer[lexer->buffer_length] = c;

	size_t new_length = lexer->buffer_length + 1;

	if(new_length <= lexer->buffer_length) {
		lexer->error = true;
		lexer->finished = true;
		return ERROR_CODE_INTEGER_OVERFLOW;
	}

	lexer->buffer_length = new_length;

	return 0;
}

static int append_current_symbol_to_buffer(TrackLexer* lexer) {

	if(lexer == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	return append_char_to_buffer(lexer, lexer->symbol);
}

static int finalize_buffer(TrackLexer* lexer) {
	return append_char_to_buffer(lexer, (char) '\0');
}

// --------

int TrackLexer_init_at(TrackLexer* lexer, Token* track) {

	if(lexer == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(track == NULL || track->type != TOKEN_TRACK || track->content == NULL) {
		lexer->error = true;
		lexer->finished = true;
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	lexer->initial_buffer_capacity = 16;
	lexer->symbol = '\0';
	lexer->line = track->line;
	lexer->col = track->col;
	lexer->finished = false;
	lexer->error = false;
	lexer->state = TRACK_LEXER_STATE_START;
	lexer->track = track;
	lexer->track_position = 0;

	update_lineinfo(lexer);
	reset_buffer_info(lexer);

	return 0;
}

void TrackLexer_destroy_at(TrackLexer* lexer) {

	if(lexer == NULL) {
		return;
	}

	free_buffer(lexer);

	return;
}

int TrackLexer_get_next_bar(TrackLexer* lexer, BarToken* token) {

	if(lexer == NULL || lexer->finished || lexer->error) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(token == NULL) {
		lexer->error = true;
		lexer->finished = true;
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	memset(token, 0, sizeof(BarToken));

	int c;

	while((c = advance(lexer)) != EOF) {

		int status = 0;

		switch(lexer->state) {

			case TRACK_LEXER_STATE_START:

				switch(lexer->symbol) {

					case '|':

						if(advance(lexer) != (int) '|') {
							return ERROR_CODE_UNEXPECTED_CHARACTER;
						}

						if(peek(lexer) == (int) '|') {
							return ERROR_CODE_UNEXPECTED_FOLLOW_UP_CHARACTER;
						}

						update_lineinfo(lexer);
						lexer->state = TRACK_LEXER_STATE_EXPECTING_BAR;

						continue;

					default:
						lexer->error = true;
						lexer->finished = true;
						return ERROR_CODE_UNEXPECTED_CHARACTER;
				}

			case TRACK_LEXER_STATE_EXPECTING_BAR:

				switch(lexer->symbol) {

					case '|':

						status = finalize_buffer(lexer);
						if(status != 0) {
							return status;
						}

						BarToken_init_at(token, lexer->saved_line, lexer->saved_col);
						BarToken_set_content(token, lexer->buffer, lexer->buffer_length);
						reset_buffer_info(lexer);

						if(peek(lexer) == (int) '|') {
							lexer->finished = true;
						}

						update_lineinfo(lexer);

						return 0;

					case '/':

						if(advance(lexer) != (int) '/') {
							return ERROR_CODE_UNEXPECTED_CHARACTER;
						}

						lexer->state = TRACK_LEXER_STATE_EXPECTING_COMMENT;

						continue;

					default:
						append_current_symbol_to_buffer(lexer);
						continue;
				}

			case TRACK_LEXER_STATE_EXPECTING_COMMENT:

				switch(lexer->symbol) {

					case '\n':
					case '\r':
						lexer->state = TRACK_LEXER_STATE_EXPECTING_BAR;
						continue;

					default:
						continue;
				}

			default:
				lexer->error = true;
				lexer->finished = true;
				return ERROR_CODE_INVALID_STATE;
		}
	}

	lexer->finished = true;
	return 0;
}

