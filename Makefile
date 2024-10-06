NAME=aula

SRC_DIR=./src
BLD_DIR=./build
BIN_DIR=./bin

HEADERS=$(SRC_DIR)/instrument.h $(SRC_DIR)/interpreter.h $(SRC_DIR)/lexer.h $(SRC_DIR)/mixer.h $(SRC_DIR)/note.h $(SRC_DIR)/note_compiler.h $(SRC_DIR)/note_conversion.h $(SRC_DIR)/player.h $(SRC_DIR)/token.h $(SRC_DIR)/wave.h $(SRC_DIR)/config.h
OBJECTS=$(BLD_DIR)/instrument.o $(BLD_DIR)/interpreter.o $(BLD_DIR)/lexer.o $(BLD_DIR)/mixer.o $(BLD_DIR)/note.o $(BLD_DIR)/note_compiler.o $(BLD_DIR)/note_conversion.o $(BLD_DIR)/player.o $(BLD_DIR)/token.o $(BLD_DIR)/wave.o $(BLD_DIR)/cli.o

BIN=$(BIN_DIR)/$(NAME)

# --------

FLAGS=-Wall -Wextra -pedantic -g

CC=clang
CFLAGS=$(FLAGS) `sdl2-config --cflags`

LD=$(CC)
LDFLAGS=$(FLAGS) -lm `sdl2-config --libs`

# --------

all: $(BIN)

clean:
	rm -r $(BLD_DIR) || true

# --------

$(BIN): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

$(BLD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	mkdir -p $(BLD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

