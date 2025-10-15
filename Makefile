# --- Compiler setup ---
CC = gcc
CFLAGS = -O2 -Wall -Iinclude
BIN_DIR = build

# --- Source directories ---
SRC_DIR = src
GEN_DIR = $(SRC_DIR)/generator
SOLVER_DIR = $(SRC_DIR)/solvers
BENCH_DIR = $(SRC_DIR)/benchmark
INT_DIR = $(SRC_DIR)/interface

# --- Source files ---
SOLVERS = $(SOLVER_DIR)/backtracking.c \
          $(SOLVER_DIR)/backtracking_mrv_bitmask.c \
          $(SOLVER_DIR)/constrain_propagation.c \
          $(SOLVER_DIR)/dlx.c

GENERATOR = $(GEN_DIR)/generator.c
INTERFACE = $(INT_DIR)/main.c
BENCHMARK = $(BENCH_DIR)/benchmark.c
DATASET = $(BENCH_DIR)/generate_datasets.c

# --- Binaries ---
SUDOKU_BIN = $(BIN_DIR)/sudoku
BENCH_BIN = $(BIN_DIR)/benchmark
DATASET_BIN = $(BIN_DIR)/generate_datasets

# --- Default rule ---
all: $(SUDOKU_BIN) $(BENCH_BIN) $(DATASET_BIN)

# --- Ensure build directory exists ---
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# --- Build rules ---
$(SUDOKU_BIN): $(INTERFACE) $(GENERATOR) $(SOLVERS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BENCH_BIN): $(BENCHMARK) $(GENERATOR) $(SOLVERS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(DATASET_BIN): $(DATASET) $(GENERATOR) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# --- Cleanup ---
clean:
	rm -rf $(BIN_DIR)
	find $(SRC_DIR) -name '*.o' -delete

.PHONY: all clean
