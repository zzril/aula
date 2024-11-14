#include <stdlib.h>

#include <string.h>

#include "bar_token.h"

// --------

void BarToken_init_at(BarToken* token, unsigned int line, unsigned int col) {

	token->line = line;
	token->col = col;

	return;
}

void BarToken_set_content(BarToken* token, char* content, size_t content_length) {

	token->content = content;
	token->content_length = content_length;

	return;
}

void BarToken_destroy_at(BarToken* token) {

	if(token == NULL) {
		return;
	}

	free(token->content);
	memset(token, 0, sizeof(BarToken));

	return;
}

