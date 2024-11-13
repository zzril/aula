#ifndef LEXER_H
#define LEXER_H
// --------

#include <stdio.h>
#include <stdbool.h>

#include "token.h"

// --------

struct Lexer;

// --------

typedef struct Lexer Lexer;

// --------

typedef enum LexerState {
	LEXER_STATE_EXPECTING_NEW_TOKEN = 0,
	LEXER_STATE_EXPECTING_TRACK,
	LEXER_STATE_EXPECTING_COMMENT,
	NUM_LEXER_STATES,
} LexerState;

struct Lexer {
	FILE* stream;
	char* buffer;
	size_t buffer_capacity;
	size_t buffer_length;
	size_t initial_buffer_capacity;
	LexerState state;
	unsigned int line;
	unsigned int col;
	unsigned int saved_line;
	unsigned int saved_col;
	char symbol;
	bool finished;
	bool error;
};

// --------

int Lexer_init_at(Lexer* lexer, FILE* stream);
void Lexer_destroy_at(Lexer* lexer);

int Lexer_get_next_token(Lexer* lexer, Token* token);

// --------
#endif

