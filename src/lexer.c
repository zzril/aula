#include <stdlib.h>

#include <string.h>

#include "error_codes.h"
#include "lexer.h"

// --------

static int advance(Lexer* lexer);
static int peek(Lexer* lexer);

static void update_lineinfo(Lexer* lexer);

static void reset_buffer_info(Lexer* lexer);
static void free_buffer(Lexer* lexer);

static int append_char_to_buffer(Lexer* lexer, char c);
static int append_current_symbol_to_buffer(Lexer* lexer);
static int finalize_buffer(Lexer* lexer);

static bool verify_keyword(Lexer* lexer, char* keyword);

// --------

static int advance(Lexer* lexer) {

	if(lexer->finished | lexer->error) {
		return EOF;
	}

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

static int peek(Lexer* lexer) {

	if(lexer->finished | lexer->error) {
		return EOF;
	}

	int c = getc(lexer->stream);
	if(c == EOF) {
		lexer->finished = true;
		return c;
	}

	int rv = ungetc(c, lexer->stream);
	if(rv != c) {
		lexer->finished = true;
		lexer->error = true;
	}

	return rv;
}

static void update_lineinfo(Lexer* lexer) {

	lexer->saved_line = lexer->line;
	lexer->saved_col = lexer->col;

	return;
}

static void reset_buffer_info(Lexer* lexer) {

	lexer->buffer = NULL;
	lexer->buffer_capacity = 0;
	lexer->buffer_length = 0;

	return;
}

static void free_buffer(Lexer* lexer) {

	free(lexer->buffer);
	reset_buffer_info(lexer);

	return;
}

static int append_char_to_buffer(Lexer* lexer, char c) {

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

static int append_current_symbol_to_buffer(Lexer* lexer) {

	if(lexer == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	return append_char_to_buffer(lexer, lexer->symbol);
}

static int finalize_buffer(Lexer* lexer) {
	return append_char_to_buffer(lexer, (char) '\0');
}

static bool verify_keyword(Lexer* lexer, char* keyword) {

	size_t index = 0;
	size_t length = strlen(keyword);

	if(length == 0) {
		return true;
	}

	while(true) {

		if(lexer->symbol != keyword[index]) {
			return false;
		}

		index++;

		if(index >= length) {
			return true;
		}

		if(advance(lexer) == EOF) {
			return false;
		}
	}
}

// --------

int Lexer_init_at(Lexer* lexer, FILE* stream) {

	if(lexer == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(stream == NULL) {
		lexer->error = true;
		lexer->finished = true;
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	lexer->initial_buffer_capacity = 256;
	lexer->symbol = '\0';
	lexer->line = 1;
	lexer->col = 1;
	lexer->finished = false;
	lexer->error = false;
	lexer->state = LEXER_STATE_EXPECTING_NEW_TOKEN;
	lexer->stream = stream;

	update_lineinfo(lexer);
	reset_buffer_info(lexer);

	return 0;
}

void Lexer_destroy_at(Lexer* lexer) {

	if(lexer == NULL) {
		return;
	}

	free_buffer(lexer);

	return;
}

int Lexer_get_next_token(Lexer* lexer, Token* token) {

	if(lexer == NULL || lexer->finished || lexer->error) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(token == NULL) {
		lexer->error = true;
		lexer->finished = true;
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	memset(token, 0, sizeof(Token));

	int c;

	while((c = advance(lexer)) != EOF) {

		int status = 0;

		switch(lexer->state) {

			case LEXER_STATE_EXPECTING_NEW_TOKEN:

				update_lineinfo(lexer);

				switch(lexer->symbol) {

					case ' ':
					case '\n':
					case '\r':
					case '\t':
						continue;

					case 't':

						if(verify_keyword(lexer, "track:") == false) {
							return ERROR_CODE_UNEXPECTED_FOLLOW_UP_CHARACTER;
						}

						Token_init_at(token, TOKEN_KEYWORD_TRACK, lexer->saved_line, lexer->saved_col);

						return 0;


					case '|':

						if(peek(lexer) != (int) '|') {
							return ERROR_CODE_UNEXPECTED_FOLLOW_UP_CHARACTER;
						}

						status = append_current_symbol_to_buffer(lexer);
						if(status != 0) {
							return status;
						}

						lexer->state = LEXER_STATE_EXPECTING_TRACK;

						continue;

					case '/':

						if(advance(lexer) != (int) '/') {
							return ERROR_CODE_UNEXPECTED_CHARACTER;
						}

						lexer->state = LEXER_STATE_EXPECTING_COMMENT;

						continue;

					default:
						lexer->error = true;
						lexer->finished = true;
						return ERROR_CODE_UNEXPECTED_CHARACTER;
				}

			case LEXER_STATE_EXPECTING_TRACK:

				status = append_current_symbol_to_buffer(lexer);
				if(status != 0) {
					return status;
				}

				switch(lexer->symbol) {

					case ' ':
					case '\n':
					case '\r':
					case '\t':
						continue;

					case '|':

						if(peek(lexer) != (int) '|') {
							continue;
						}

						if(advance(lexer) != '|') {
							lexer->error = true;
							lexer->finished = true;
							return ERROR_CODE_INVALID_STATE;
						}

						status = append_current_symbol_to_buffer(lexer);
						if(status != 0) {
							return status;
						}

						status = finalize_buffer(lexer);
						if(status != 0) {
							return status;
						}

						Token_init_at(token, TOKEN_TRACK, lexer->saved_line, lexer->saved_col);
						Token_set_content(token, lexer->buffer, lexer->buffer_length);
						reset_buffer_info(lexer);

						lexer->state = LEXER_STATE_EXPECTING_NEW_TOKEN;

						return 0;

					default:
						continue;
					}

			case LEXER_STATE_EXPECTING_COMMENT:

				switch(lexer->symbol) {

					case '\n':
					case '\r':

						status = finalize_buffer(lexer);
						if(status != 0) {
							return status;
						}

						Token_init_at(token, TOKEN_COMMENT, lexer->saved_line, lexer->saved_col);
						Token_set_content(token, lexer->buffer, lexer->buffer_length);
						reset_buffer_info(lexer);

						lexer->state = LEXER_STATE_EXPECTING_NEW_TOKEN;

						return 0;

					default:
						status = append_current_symbol_to_buffer(lexer);
						if(status != 0) {
							return status;
						}
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

