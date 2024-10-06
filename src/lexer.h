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
	LEXER_STATE_EXPECTING_TRACK_START,
	LEXER_STATE_EXPECTING_BAR,
	NUM_LEXER_STATES,
} LexerState;

struct Lexer {
	FILE* stream;
	LexerState state;
	unsigned int line;
	unsigned int col;
	char symbol;
	bool finished;
	bool error;
};

// --------

int Lexer_init_at(Lexer* lexer, FILE* stream);

int Lexer_get_next_token(Lexer* lexer, Token* token);

// --------
#endif

