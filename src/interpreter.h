#ifndef INTERPRETER_H
#define INTERPRETER_H
// --------

#include <stdbool.h>
#include <stdio.h>

// --------

struct Interpreter;

// --------

typedef struct Interpreter Interpreter;

// --------

typedef enum InterpreterState {
	INTERPRETER_STATE_EXPECTING_KEYWORD = 0,
	INTERPRETER_STATE_EXPECTING_TEMPO,
	INTERPRETER_STATE_EXPECTING_TRACK,
	NUM_INTERPRETER_STATES,
} InterpreterState;

typedef enum InterpreterErrorState {
	INTERPRETER_ERROR_STATE_UNKNOWN_ERROR = 0,
	INTERPRETER_ERROR_STATE_UNEXPECTED_TOKEN,
	INTERPRETER_ERROR_STATE_INTERNAL_ERROR,
	NUM_INTERPRETER_ERROR_STATES,
} InterpreterErrorState;

struct Interpreter {
	char* filename;
	InterpreterState state;
	InterpreterErrorState error_state;
	bool finished;
	bool error;
	bool printed_err_msg;
};

// --------

void Interpreter_init_at(Interpreter* interpreter);

int Interpreter_interpret(Interpreter* interpreter, FILE* stream);
int Interpreter_interpret_file(Interpreter* interpreter, char* filename);

// --------
#endif

