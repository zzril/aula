#ifndef ABSTRACT_LEXER_H
#define ABSTRACT_LEXER_H
// --------

#include <stdbool.h>
#include <stddef.h>

// --------

struct AbstractLexer;

// --------

typedef struct AbstractLexer AbstractLexer;

// --------

struct AbstractLexer {
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
};

// --------

void update_lineinfo(AbstractLexer* lexer);

void reset_buffer_info(AbstractLexer* lexer);
void free_buffer(AbstractLexer* lexer);

int append_char_to_buffer(AbstractLexer* lexer, char c);
int append_current_symbol_to_buffer(AbstractLexer* lexer);
int finalize_buffer(AbstractLexer* lexer);

int AbstractLexer_init_at(AbstractLexer* lexer, size_t initial_buffer_capacity);

// --------
#endif

