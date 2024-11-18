#include <stdlib.h>

#include <string.h>

#include "bar_token.h"
#include "error_codes.h"

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

int BarToken_print(BarToken* token, FILE* stream) {

	if(token == NULL || token->content == NULL || stream == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(token->content_length == 0) {
		return 0;
	}

	fputs(token->content, stream);

	return 0;
}

