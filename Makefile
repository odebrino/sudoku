# ============================================================
#  Sudoku Project - Optimized Makefile
# ============================================================

CC       := gcc
CFLAGS   := -std=c11 -Wall -Wextra -Iinclude -MMD -MP
OPT      ?= -O2
DEBUG    ?= 0
BIN_DIR  := build
OBJ_DIR  := $(BIN_DIR)/obj

ifeq ($(DEBUG),1)
    CFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += $(OPT)
endif

SRC_DIR  := src
GEN_DIR  := $(SRC_DIR)/generator
SOLVER_DIR := $(SRC_DIR)/solvers
BENCH_DIR  := $(SRC_DIR)/benchmark
INT_DIR    := $(SRC_DIR)/interface

SOLVERS   := $(wildcard $(SOLVER_DIR)/*.c)
GENERATOR := $(GEN_DIR)/generator.c
INTERFACE := $(INT_DIR)/main.c
BENCHMARK := $(BENCH_DIR)/benchmark.c
DATASET   := $(BENCH_DIR)/generate_datasets.c

SUDOKU_BIN   := $(BIN_DIR)/sudoku
BENCH_BIN    := $(BIN_DIR)/benchmark
DATASET_BIN  := $(BIN_DIR)/generate_datasets

OBJS_SOLVERS := $(SOLVERS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJS_GEN     := $(GENERATOR:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJS_INT     := $(INTERFACE:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJS_BENCH   := $(BENCHMARK:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJS_DATASET := $(DATASET:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS := $(OBJS_SOLVERS:.o=.d) $(OBJS_GEN:.o=.d) $(OBJS_INT:.o=.d) $(OBJS_BENCH:.o=.d) $(OBJS_DATASET:.o=.d)

all: dirs $(SUDOKU_BIN) $(BENCH_BIN) $(DATASET_BIN)

dirs:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR)

$(SUDOKU_BIN): $(OBJS_INT) $(OBJS_GEN) $(OBJS_SOLVERS)
	@echo "\033[1;32m[LD]\033[0m $@"
	@$(CC) $(CFLAGS) $^ -o $@

$(BENCH_BIN): $(OBJS_BENCH) $(OBJS_GEN) $(OBJS_SOLVERS)
	@echo "\033[1;32m[LD]\033[0m $@"
	@$(CC) $(CFLAGS) $^ -o $@

$(DATASET_BIN): $(OBJS_DATASET) $(OBJS_GEN)
	@echo "\033[1;32m[LD]\033[0m $@"
	@$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "\033[1;34m[CC]\033[0m $<"
	@$(CC) $(CFLAGS) -c $< -o $@

run: $(SUDOKU_BIN)
	@./$(SUDOKU_BIN)

bench: $(BENCH_BIN)
	@./$(BENCH_BIN)

datasets: $(DATASET_BIN)
	@./$(DATASET_BIN)

clean:
	@echo "\033[1;31m[RM]\033[0m Cleaning..."
	@rm -rf $(BIN_DIR)

-include $(DEPS)

.PHONY: all clean dirs run bench datasets
