#include <stdlib.h>

#include <string.h>

#include "error_codes.h"
#include "token.h"

// --------

void Token_init_at(Token* token, TokenType type, unsigned int line, unsigned int col) {

	token->type = type;
	token->line = line;
	token->col = col;

	return;
}

int Token_set_content_buffer(Token* token, char* buffer, size_t length) {

	switch(token->type) {

		case TOKEN_TRACK:
		case TOKEN_COMMENT:

			token->content.buffer = buffer;
			token->content_length = length;
			return 0;

		default:
			return ERROR_CODE_INVALID_ARGUMENT;
	}
}

void Token_destroy_at(Token* token) {

	if(token == NULL) {
		return;
	}

	switch(token->type) {

		case TOKEN_TRACK:
		case TOKEN_COMMENT:

			free(token->content.buffer);

		default:
			break;
	}

	memset(token, 0, sizeof(Token));

	return;
}

