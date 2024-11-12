#ifndef TRACK_LEXER_H
#define TRACK_LEXER_H
// --------

#include <stdio.h>
#include <stdbool.h>

#include "bar.h"

// --------

struct TrackLexer;

// --------

typedef struct TrackLexer TrackLexer;

// --------

typedef enum TrackLexerState {
	TRACK_LEXER_STATE_EXPECTING_NEW_TOKEN = 0,
	TRACK_LEXER_STATE_EXPECTING_TRACK_START,
	TRACK_LEXER_STATE_EXPECTING_BAR,
	TRACK_LEXER_STATE_EXPECTING_COMMENT,
	TRACK_LEXER_STATE_EXPECTING_BAR_COMMENT,
	NUM_TRACK_LEXER_STATES,
} TrackLexerState;

struct TrackLexer {
	FILE* stream;
	TrackLexerState state;
	unsigned int line;
	unsigned int col;
	char symbol;
	bool finished;
	bool error;
};

// --------

int TrackLexer_init_at(TrackLexer* lexer, FILE* stream);

int TrackLexer_get_next_token(TrackLexer* lexer, Bar* bar);

// --------
#endif

