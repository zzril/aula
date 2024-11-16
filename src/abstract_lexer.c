#include <stdlib.h>

#include "abstract_lexer.h"
#include "error_codes.h"

// --------

void update_lineinfo(AbstractLexer* lexer) {

	lexer->saved_line = lexer->line;
	lexer->saved_col = lexer->col;

	return;
}

void reset_buffer_info(AbstractLexer* lexer) {

	lexer->buffer = NULL;
	lexer->buffer_capacity = 0;
	lexer->buffer_length = 0;

	return;
}

void free_buffer(AbstractLexer* lexer) {

	free(lexer->buffer);
	reset_buffer_info(lexer);

	return;
}

int append_char_to_buffer(AbstractLexer* lexer, char c) {

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

int append_current_symbol_to_buffer(AbstractLexer* lexer) {

	if(lexer == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	return append_char_to_buffer(lexer, lexer->symbol);
}

int finalize_buffer(AbstractLexer* lexer) {
	return append_char_to_buffer(lexer, (char) '\0');
}

int AbstractLexer_init_at(AbstractLexer* lexer, size_t initial_buffer_capacity) {

	if(lexer == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	lexer->initial_buffer_capacity = initial_buffer_capacity;
	lexer->line = 1;
	lexer->col = 1;
	lexer->symbol = '\0';
	lexer->finished = false;
	lexer->error = false;

	update_lineinfo(lexer);
	reset_buffer_info(lexer);

	return 0;
}

