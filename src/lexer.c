#include <stdlib.h>

#include <errno.h>
#include <string.h>

#include "error_codes.h"
#include "lexer.h"

// --------

static int advance(AbstractLexer* lexer);
static int peek(AbstractLexer* lexer);

static bool verify_keyword(AbstractLexer* lexer, const char* keyword);

static int Lexer_get_next_token_internal(AbstractLexer* lexer, Token* token);

// --------

static int advance(AbstractLexer* lexer) {

	if(lexer->finished | lexer->error) {
		return EOF;
	}

	int c = getc(((Lexer*) lexer)->stream);
	if(c == EOF) {
		lexer->finished = true;
		return c;
	}

	lexer->symbol = (char) c;
	(lexer->col)++;

	if(lexer->symbol == '\n') {
		lexer->col = 1;
		(lexer->line)++;
	}

	return c;
}

static int peek(AbstractLexer* lexer) {

	if(lexer->finished | lexer->error) {
		return EOF;
	}

	int c = getc(((Lexer*) lexer)->stream);
	if(c == EOF) {
		lexer->finished = true;
		return c;
	}

	int rv = ungetc(c, ((Lexer*) lexer)->stream);
	if(rv != c) {
		lexer->finished = true;
		lexer->error = true;
	}

	return rv;
}

static bool verify_keyword(AbstractLexer* lexer, const char* keyword) {

	size_t index = 0;
	size_t length = strlen(keyword);

	if(length == 0) {
		return true;
	}

	while(true) {

		if(lexer->symbol != keyword[index]) {
			return false;
		}

		index++;

		if(index >= length) {
			return true;
		}

		if(advance(lexer) == EOF) {
			return false;
		}
	}
}

static int Lexer_get_next_token_internal(AbstractLexer* lexer, Token* token) {

	if(lexer == NULL || lexer->finished || lexer->error) {
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	if(token == NULL) {
		lexer->error = true;
		lexer->finished = true;
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	memset(token, 0, sizeof(Token));

	int c;

	while((c = advance(lexer)) != EOF) {

		int status = 0;
		long integer_literal = 0;
		char* endptr = NULL;

		switch(((Lexer*) lexer)->state) {

			case LEXER_STATE_EXPECTING_NEW_TOKEN:

				update_lineinfo(lexer);

				switch(lexer->symbol) {

					case ' ':
					case '\n':
					case '\r':
					case '\t':
						continue;

					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':

						status = append_current_symbol_to_buffer(lexer);
						if(status != 0) {
							return status;
						}

						((Lexer*) lexer)->state = LEXER_STATE_EXPECTING_LITERAL_INTEGER;

						continue;

					case 'b':

						if(verify_keyword(lexer, TOKEN_KEYWORDS[TOKEN_KEYWORD_BPM]) == false) {
							return ERROR_CODE_UNEXPECTED_CHARACTER;
						}

						Token_init_at(token, TOKEN_KEYWORD_BPM, lexer->saved_line, lexer->saved_col);

						return 0;

					case 't':

						if(verify_keyword(lexer, TOKEN_KEYWORDS[TOKEN_KEYWORD_TRACK]) == false) {
							return ERROR_CODE_UNEXPECTED_CHARACTER;
						}

						Token_init_at(token, TOKEN_KEYWORD_TRACK, lexer->saved_line, lexer->saved_col);

						return 0;

					case '|':

						if(peek(lexer) != (int) '|') {
							return ERROR_CODE_UNEXPECTED_FOLLOW_UP_CHARACTER;
						}

						status = append_current_symbol_to_buffer(lexer);
						if(status != 0) {
							return status;
						}

						((Lexer*) lexer)->state = LEXER_STATE_EXPECTING_TRACK;

						continue;

					case '/':

						if(peek(lexer) != (int) '/') {
							return ERROR_CODE_UNEXPECTED_FOLLOW_UP_CHARACTER;
						}

						status = append_current_symbol_to_buffer(lexer);
						if(status != 0) {
							return status;
						}

						((Lexer*) lexer)->state = LEXER_STATE_EXPECTING_COMMENT;

						continue;

					default:
						lexer->error = true;
						lexer->finished = true;
						return ERROR_CODE_UNEXPECTED_CHARACTER;
				}

			case LEXER_STATE_EXPECTING_LITERAL_INTEGER:

				switch(lexer->symbol) {

					case ' ':
					case '\n':
					case '\r':
					case '\t':

						status = finalize_buffer(lexer);
						if(status != 0) {
							return status;
						}

						Token_init_at(token, TOKEN_LITERAL_INTEGER, lexer->saved_line, lexer->saved_col);

						errno = 0;
						integer_literal = strtol(lexer->buffer, &endptr, 10);

						switch(errno) {

							case 0:
								break;

							case ERANGE:
								return ERROR_CODE_INTEGER_OVERFLOW;

							default:
								return ERROR_CODE_INVALID_STATE;
						}

						if(*endptr != '\0') {
							return ERROR_CODE_INVALID_STATE;
						}

						token->content.integer = (int) integer_literal;

						if((long) (token->content.integer) != integer_literal) {
							return ERROR_CODE_INTEGER_OVERFLOW;
						}

						reset_buffer_info(lexer);

						((Lexer*) lexer)->state = LEXER_STATE_EXPECTING_NEW_TOKEN;

						return 0;

					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':

						status = append_current_symbol_to_buffer(lexer);
						if(status != 0) {
							return status;
						}

						continue;

					default:
						lexer->error = true;
						lexer->finished = true;
						return ERROR_CODE_UNEXPECTED_CHARACTER;
				}

			case LEXER_STATE_EXPECTING_TRACK:

				status = append_current_symbol_to_buffer(lexer);
				if(status != 0) {
					return status;
				}

				switch(lexer->symbol) {

					case ' ':
					case '\n':
					case '\r':
					case '\t':
						continue;

					case '|':

						if(peek(lexer) != (int) '|') {
							continue;
						}

						if(advance(lexer) != '|') {
							lexer->error = true;
							lexer->finished = true;
							return ERROR_CODE_INVALID_STATE;
						}

						status = append_current_symbol_to_buffer(lexer);
						if(status != 0) {
							return status;
						}

						status = finalize_buffer(lexer);
						if(status != 0) {
							return status;
						}

						Token_init_at(token, TOKEN_TRACK, lexer->saved_line, lexer->saved_col);
						status = Token_set_content_buffer(token, lexer->buffer, lexer->buffer_length);
						if(status != 0) {
							return status;
						}
						reset_buffer_info(lexer);

						((Lexer*) lexer)->state = LEXER_STATE_EXPECTING_NEW_TOKEN;

						return 0;

					default:
						continue;
					}

			case LEXER_STATE_EXPECTING_COMMENT:

				switch(lexer->symbol) {

					case '\n':
					case '\r':

						status = finalize_buffer(lexer);
						if(status != 0) {
							return status;
						}

						Token_init_at(token, TOKEN_COMMENT, lexer->saved_line, lexer->saved_col);
						status = Token_set_content_buffer(token, lexer->buffer, lexer->buffer_length);
						if(status != 0) {
							return status;
						}
						reset_buffer_info(lexer);

						((Lexer*) lexer)->state = LEXER_STATE_EXPECTING_NEW_TOKEN;

						return 0;

					default:
						status = append_current_symbol_to_buffer(lexer);
						if(status != 0) {
							return status;
						}
						continue;
				}

			default:
				lexer->error = true;
				lexer->finished = true;
				return ERROR_CODE_INVALID_STATE;
		}
	}

	lexer->finished = true;
	return 0;
}

// --------

int Lexer_init_at(Lexer* lexer, FILE* stream) {

	int status = AbstractLexer_init_at((AbstractLexer*) lexer, 256);
	if(status != 0) {
		return status;
	}

	if(stream == NULL) {
		((AbstractLexer*) lexer)->error = true;
		((AbstractLexer*) lexer)->finished = true;
		return ERROR_CODE_INVALID_ARGUMENT;
	}

	lexer->state = LEXER_STATE_EXPECTING_NEW_TOKEN;
	lexer->stream = stream;

	return 0;
}

void Lexer_destroy_at(Lexer* lexer) {

	if(lexer == NULL) {
		return;
	}

	free_buffer((AbstractLexer*) lexer);

	return;
}

int Lexer_get_next_token(Lexer* lexer, Token* token) {
	return Lexer_get_next_token_internal((AbstractLexer*) lexer, token);
}

