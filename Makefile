NAME=aula

SRC_DIR=./src
BLD_DIR=./build
BIN_DIR=./bin
EX_DIR=./examples

SOURCES=$(wildcard $(SRC_DIR)/*.c)
HEADERS=$(wildcard $(SRC_DIR)/*.h)

OBJECTS=$(patsubst $(SRC_DIR)/%.c,$(BLD_DIR)/%.o,$(SOURCES))

BIN=$(BIN_DIR)/$(NAME)

EXAMPLES=$(wildcard $(EX_DIR)/*.aula)
TESTS=$(patsubst $(EX_DIR)/%.aula,test-%,$(EXAMPLES))

# --------

FLAGS=-Wall -Wextra -pedantic -g

CC=clang
CFLAGS=$(FLAGS) `sdl2-config --cflags`

LD=$(CC)
LDFLAGS=$(FLAGS) -lm `sdl2-config --libs`

# --------

.PHONY: all
all: $(BIN)

.PHONY: clean
clean:
	rm -r $(BLD_DIR) $(BIN_DIR) || true

.PHONY: check
check: $(TESTS)

# --------

$(BIN): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

$(BLD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	mkdir -p $(BLD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: $(TESTS)
$(TESTS): test-%: $(EX_DIR)/%.aula $(BIN)
	./$(BIN) $<

