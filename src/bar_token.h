#ifndef BAR_TOKEN_H
#define BAR_TOKEN_H
// --------

#include <stddef.h>

// --------

struct BarToken;

// --------

typedef struct BarToken BarToken;

// --------

struct BarToken {
	char* content;
	size_t content_length;
	unsigned int line;
	unsigned int col;
};

// --------

void BarToken_init_at(BarToken* token, unsigned int line, unsigned int col);

void BarToken_set_content(BarToken* token, char* content, size_t content_length);

void BarToken_destroy_at(BarToken* token);

// --------
#endif

