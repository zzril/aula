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
	TOKEN_BAR,
	TOKEN_TRACK_START,
	TOKEN_TRACK_END,
	TOKEN_COMMENT,
	NUM_TOKEN_TYPES,
} TokenType;

struct Token {
	void* content;
	size_t content_length;
	TokenType type;
	unsigned int line;
	unsigned int col;
};

// --------

void Token_init_at(Token* token, TokenType type, unsigned int line, unsigned int col);

void Token_set_content(Token* token, void* content, size_t content_length);

void Token_destroy_at(Token* token);

// --------
#endif

