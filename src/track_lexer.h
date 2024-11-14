#ifndef TRACK_LEXER_H
#define TRACK_LEXER_H
// --------

#include <stdbool.h>

#include "bar_token.h"
#include "token.h"

// --------

struct TrackLexer;

// --------

typedef struct TrackLexer TrackLexer;

// --------

typedef enum TrackLexerState {
	TRACK_LEXER_STATE_START = 0,
	TRACK_LEXER_STATE_EXPECTING_BAR,
	TRACK_LEXER_STATE_EXPECTING_COMMENT,
	NUM_TRACK_LEXER_STATES,
} TrackLexerState;

struct TrackLexer {
	char* buffer;
	size_t buffer_capacity;
	size_t buffer_length;
	size_t initial_buffer_capacity;
	unsigned int line;
	unsigned int col;
	unsigned int saved_line;
	unsigned int saved_col;
	char symbol;
	bool finished;
	bool error;
	TrackLexerState state;
	Token* track;
	size_t track_position;
};

// --------

int TrackLexer_init_at(TrackLexer* lexer, Token* track);
void TrackLexer_destroy_at(TrackLexer* lexer);

int TrackLexer_get_next_bar(TrackLexer* lexer, BarToken* token);

// --------
#endif

