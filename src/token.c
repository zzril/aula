#include <stdlib.h>

#include <string.h>

#include "error_codes.h"
#include "token.h"

// --------

const char* TOKEN_KEYWORDS[NUM_TOKEN_KEYWORD_TYPES] = {
	[TOKEN_INVALID] = "<invalid token>",
	[TOKEN_KEYWORD_BPM] = "bpm:",
	[TOKEN_KEYWORD_TRACK] = "track:",
};

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

int Token_print(Token* token, FILE* stream) {

	if(token == NULL || stream == NULL) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	switch(token->type) {

		case TOKEN_KEYWORD_BPM:
		case TOKEN_KEYWORD_TRACK:
			fputs(TOKEN_KEYWORDS[token->type], stream);
			return 0;

		case TOKEN_TRACK:
		case TOKEN_COMMENT:

			if(token->content.buffer == NULL) {
				return ERROR_CODE_INVALID_STATE;
			}

			if(token->content_length == 0) {
				return 0;
			}

			fputs(token->content.buffer, stream);

			return 0;

		case TOKEN_LITERAL_INTEGER:
			fprintf(stream, "%d", token->content.integer);
			return 0;

		default:
			fputs(TOKEN_KEYWORDS[TOKEN_INVALID], stream);
			return 0;
	}
}

