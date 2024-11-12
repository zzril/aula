#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "error_codes.h"
#include "track_lexer.h"

// --------

static int advance(TrackLexer* lexer);

static int fill_buffer_until(TrackLexer* lexer, char** buffer, size_t initial_capacity, size_t* length, char endchar, bool skip_first);

// --------

static int advance(TrackLexer* lexer) {

	int c = getc(lexer->stream);
	if(c == EOF) {
		lexer->finished = true;
		return c;
	}

	lexer->symbol = (char) c;
	(lexer->col)++;

	if(lexer->symbol == '\n') {
		lexer->col = 1;
		(lexer->line)++;
	}

	return c;
}

static int fill_buffer_until(TrackLexer* lexer, char** buffer, size_t initial_capacity, size_t* length, char endchar, bool skip_first) {

	int c;
	size_t capacity = initial_capacity;

	if(*buffer != NULL || initial_capacity == 0 || *length != 0) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	*buffer = malloc(initial_capacity);

	if(*buffer == NULL) {
		return ERROR_CODE_MALLOC_FAILURE;
	}

	if(!skip_first) {
		(*buffer)[*length] = lexer->symbol;
		(*length)++;
	}

	while((c = advance(lexer)) != EOF) {

		if(lexer->symbol == endchar) {
			(*buffer)[*length] = '\0';
			return 0;
		}

		if(*length >= capacity - 1) {

			char* new_buffer;

			capacity *= 2;

			new_buffer = realloc(*buffer, capacity);
			if(new_buffer == NULL) {
				return ERROR_CODE_MALLOC_FAILURE;
			}

			*buffer = new_buffer;
		}

		(*buffer)[*length] = lexer->symbol;
		(*length)++;
	}

	(*buffer)[*length] = '\0';

	lexer->finished = true;
	lexer->error = true;

	return ERROR_CODE_UNEXPECTED_EOF;
}

// --------

int TrackLexer_init_at(TrackLexer* lexer, FILE* stream) {

	lexer->stream = stream;
	lexer->state = TRACK_LEXER_STATE_EXPECTING_NEW_TOKEN;
	lexer->symbol = '\0';

	if(lexer->stream == NULL) {
		lexer->line = 0;
		lexer->col = 0;
		lexer->finished = true;
		lexer->error = true;
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	lexer->line = 1;
	lexer->col = 1;

	lexer->finished = false;
	lexer->error = false;

	return 0;
}

int TrackLexer_get_next_token(TrackLexer* lexer, Bar* bar) {

	if(lexer == NULL || (lexer->finished | lexer->error)) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(bar == NULL) {
		lexer->finished = true;
		lexer->error = true;
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	memset(bar, 0, sizeof(Bar));

	int c;

	while((c = advance(lexer)) != EOF) {

		int status = 0;
		char* buffer = NULL;
		size_t length = 0;

		switch(lexer->state) {

			case TRACK_LEXER_STATE_EXPECTING_NEW_TOKEN:

				switch(lexer->symbol) {

					case ' ':
					case '\n':
					case '\r':
					case '\t':
						continue;

					case '|':

						lexer->state = TRACK_LEXER_STATE_EXPECTING_TRACK_START;
						continue;

					case '/':
						lexer->state = TRACK_LEXER_STATE_EXPECTING_COMMENT;
						continue;

					default:
						lexer->error = true;
						lexer->finished = true;
						return ERROR_CODE_UNEXPECTED_CHARACTER;
				}

			case TRACK_LEXER_STATE_EXPECTING_TRACK_START:

				if(lexer->symbol != '|') {
					lexer->error = true;
					lexer->finished = true;
					return ERROR_CODE_UNEXPECTED_CHARACTER;
				}

				lexer->state = TRACK_LEXER_STATE_EXPECTING_BAR;
				Bar_init_at(bar, BAR_TRACK_START, lexer->line, lexer->col);
				return 0;

			case TRACK_LEXER_STATE_EXPECTING_BAR:

				switch(lexer->symbol) {

					case ' ':
					case '\n':
					case '\r':
					case '\t':
						continue;

					case '|':
						lexer->state = TRACK_LEXER_STATE_EXPECTING_NEW_TOKEN;
						Bar_init_at(bar, BAR_TRACK_END, lexer->line, lexer->col);
						return 0;

					case '/':
						lexer->state = TRACK_LEXER_STATE_EXPECTING_BAR_COMMENT;
						continue;

					default:
						break;
					}

				status = fill_buffer_until(lexer, &buffer, 16, &length, '|', false);
				if(status != 0) {
					return status;
				}

				Bar_init_at(bar, BAR_BAR, lexer->line, lexer->col - length);
				Bar_set_content(bar, buffer, length);

				buffer = NULL;
				length = 0;

				return 0;

			case TRACK_LEXER_STATE_EXPECTING_COMMENT:
			case TRACK_LEXER_STATE_EXPECTING_BAR_COMMENT:

				if(!(lexer->symbol == '/')) {
					return ERROR_CODE_UNEXPECTED_CHARACTER;
				}

				status = fill_buffer_until(lexer, &buffer, 16, &length, lexer->state == TRACK_LEXER_STATE_EXPECTING_BAR_COMMENT? '|': '\n', true);
				if(status != 0) {
					return status;
				}

				Bar_init_at(bar, BAR_COMMENT, lexer->line, lexer->col - length);
				Bar_set_content(bar, buffer, length);

				buffer = NULL;
				length = 0;

				lexer->state = lexer->state == TRACK_LEXER_STATE_EXPECTING_BAR_COMMENT? TRACK_LEXER_STATE_EXPECTING_BAR: TRACK_LEXER_STATE_EXPECTING_NEW_TOKEN;

				return 0;

			default:
				lexer->error = true;
				lexer->finished = true;
				return ERROR_CODE_INVALID_STATE;
		}
	}

	lexer->finished = true;
	return 0;
}

