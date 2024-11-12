#include <stdlib.h>

#include <string.h>

#include "bar.h"

// --------

void Bar_init_at(Bar* bar, BarType type, unsigned int line, unsigned int col) {

	bar->type = type;
	bar->line = line;
	bar->col = col;

	return;
}

void Bar_set_content(Bar* bar, void* content, size_t content_length) {

	bar->content = content;
	bar->content_length = content_length;

	return;
}

void Bar_destroy_at(Bar* bar) {

	if(bar == NULL) {
		return;
	}

	free(bar->content);
	memset(bar, 0, sizeof(Bar));

	return;
}

