#include <stdio.h>
#include <stdlib.h>

#include "interpreter.h"

// --------

int main(int argc, char** argv) {

	char* filename;
	Interpreter interpreter;

	if(argc != 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	filename = argv[1];

	Interpreter_init_at(&interpreter);

	if(Interpreter_interpret_file(&interpreter, filename) != 0) {
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

