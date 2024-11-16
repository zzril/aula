#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "error_codes.h"
#include "track_lexer.h"

// --------

static int advance(AbstractLexer* lexer);
static int peek(AbstractLexer* lexer);

static int TrackLexer_get_next_bar_internal(AbstractLexer* lexer, BarToken* token);

// --------

static int advance(AbstractLexer* lexer) {

	if(lexer->finished | lexer->error) {
		return EOF;
	}

	if(((TrackLexer*) lexer)->track_position >= ((TrackLexer*) lexer)->track->content_length) {
		lexer->finished = true;
		return EOF;
	}

	lexer->symbol = ((char*) (((TrackLexer*) lexer)->track->content))[((TrackLexer*) lexer)->track_position];
	(((TrackLexer*) lexer)->track_position)++;
	(lexer->col)++;

	if(lexer->symbol == '\n') {
		lexer->col = 1;
		(lexer->line)++;
	}

	return (int) (lexer->symbol);
}

static int peek(AbstractLexer* lexer) {

	if(lexer->finished | lexer->error) {
		return EOF;
	}

	if(((TrackLexer*) lexer)->track_position >= ((TrackLexer*) lexer)->track->content_length) {
		return EOF;
	}

	return (int) ((char*) (((TrackLexer*) lexer)->track->content))[((TrackLexer*) lexer)->track_position];
}

static int TrackLexer_get_next_bar_internal(AbstractLexer* lexer, BarToken* token) {

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

		switch(((TrackLexer*) lexer)->state) {

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
						((TrackLexer*) lexer)->state = TRACK_LEXER_STATE_EXPECTING_BAR;

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

						((TrackLexer*) lexer)->state = TRACK_LEXER_STATE_EXPECTING_COMMENT;

						continue;

					default:
						append_current_symbol_to_buffer(lexer);
						continue;
				}

			case TRACK_LEXER_STATE_EXPECTING_COMMENT:

				switch(lexer->symbol) {

					case '\n':
					case '\r':
						((TrackLexer*) lexer)->state = TRACK_LEXER_STATE_EXPECTING_BAR;
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

// --------

int TrackLexer_init_at(TrackLexer* lexer, Token* track) {

	int status = AbstractLexer_init_at((AbstractLexer*) lexer, 16);
	if(status != 0) {
		return status;
	}

	if(track == NULL || track->type != TOKEN_TRACK || track->content == NULL) {
		((AbstractLexer*) lexer)->error = true;
		((AbstractLexer*) lexer)->finished = true;
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	lexer->state = TRACK_LEXER_STATE_START;
	lexer->track = track;
	lexer->track_position = 0;

	return 0;
}

void TrackLexer_destroy_at(TrackLexer* lexer) {

	if(lexer == NULL) {
		return;
	}

	free_buffer((AbstractLexer*) lexer);

	return;
}

int TrackLexer_get_next_bar(TrackLexer* lexer, BarToken* token) {
	return TrackLexer_get_next_bar_internal((AbstractLexer*) lexer, token);
}

