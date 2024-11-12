#ifndef BAR_H
#define BAR_H
// --------

#include <stddef.h>

// --------

struct Bar;

// --------

typedef struct Bar Bar;

// --------

typedef enum BarType {
	BAR_INVALID = 0,
	BAR_BAR,
	BAR_TRACK_START,
	BAR_TRACK_END,
	BAR_COMMENT,
	NUM_BAR_TYPES,
} BarType;

struct Bar {
	void* content;
	size_t content_length;
	BarType type;
	unsigned int line;
	unsigned int col;
};

// --------

void Bar_init_at(Bar* bar, BarType type, unsigned int line, unsigned int col);

void Bar_set_content(Bar* bar, void* content, size_t content_length);

void Bar_destroy_at(Bar* bar);

// --------
#endif

