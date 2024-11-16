#ifndef LEXER_H
#define LEXER_H
// --------

#include <stdio.h>

#include "abstract_lexer.h"
#include "token.h"

// --------

struct Lexer;

// --------

typedef struct Lexer Lexer;

// --------

typedef enum LexerState {
	LEXER_STATE_EXPECTING_NEW_TOKEN = 0,
	LEXER_STATE_EXPECTING_BPM,
	LEXER_STATE_EXPECTING_TRACK,
	LEXER_STATE_EXPECTING_COMMENT,
	NUM_LEXER_STATES,
} LexerState;

struct Lexer {
	AbstractLexer super;
	LexerState state;
	FILE* stream;
};

// --------

int Lexer_init_at(Lexer* lexer, FILE* stream);
void Lexer_destroy_at(Lexer* lexer);

int Lexer_get_next_token(Lexer* lexer, Token* token);

// --------
#endif

