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

void Token_set_content(Token* token, void* content, size_t content_length) {

	token->content = content;
	token->content_length = content_length;

	return;
}

void Token_destroy_at(Token* token) {

	if(token == NULL) {
		return;
	}

	free(token->content);
	memset(token, 0, sizeof(Token));

	return;
}

int Token_print(Token* token, FILE* stream) {

	if(token == NULL || stream == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	switch(token->type) {

		case TOKEN_BAR:
			if(token->content != NULL) {
				fputs("\"", stream);
				fputs((char*) (token->content), stream);
				fputs("\"", stream);
			}
			return 0;

		default:
			return 0;
	}
}

