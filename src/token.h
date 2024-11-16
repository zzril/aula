#ifndef TOKEN_H
#define TOKEN_H
// --------

#include <stddef.h>

// --------

struct Token;

// --------

typedef struct Token Token;

// --------

typedef enum TokenType {
	TOKEN_INVALID = 0,
	TOKEN_KEYWORD_BPM,
	TOKEN_KEYWORD_TRACK,
	TOKEN_LITERAL_INTEGER,
	TOKEN_TRACK,
	TOKEN_COMMENT,
	NUM_TOKEN_TYPES,
} TokenType;

typedef union TokenContent {
	char* buffer;
	int literal_integer;
} TokenContent;

struct Token {
	TokenContent content;
	size_t content_length;
	TokenType type;
	unsigned int line;
	unsigned int col;
};

// --------

void Token_init_at(Token* token, TokenType type, unsigned int line, unsigned int col);

int Token_set_content_buffer(Token* token, char* buffer, size_t length);

void Token_destroy_at(Token* token);

// --------
#endif

