#include <stdlib.h>

#include <string.h>

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

