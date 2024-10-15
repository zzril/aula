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
	LEXER_STATE_EXPECTING_COMMENT,
	LEXER_STATE_EXPECTING_BAR_COMMENT,
	NUM_LEXER_STATES,
} LexerState;

typedef enum LexerErrorState {
	LEXER_ERROR_STATE_UNKNOWN_ERROR = 0,
	LEXER_ERROR_STATE_UNEXPECTED_EOF,
	LEXER_ERROR_STATE_UNEXPECTED_CHARACTER,
	NUM_LEXER_ERROR_STATES,
} LexerErrorState;

struct Lexer {
	FILE* stream;
	LexerState state;
	LexerErrorState error_state;
	unsigned int line;
	unsigned int col;
	char symbol;
	bool finished;
	bool error;
};

// --------

extern const char* LexerErrors[NUM_LEXER_ERROR_STATES];

// --------

int Lexer_init_at(Lexer* lexer, FILE* stream);

int Lexer_get_next_token(Lexer* lexer, Token* token);

// --------
#endif

