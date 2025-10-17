# ---- generate_datasets target (usa DLX p/ unicidade) ----
build/generate_datasets: src/benchmark/generate_datasets.c src/generator/generator.c src/solvers/dlx.c $(wildcard src/utils/*.c)
	$(CC) $(CFLAGS) -Isrc -Iinclude $^ -o $@ $(LIBS) -lm

# ---- bf_suite: brute force validation & timing ----
build/bf_suite: src/tests/bf_suite.c \
                src/generator/generator.c \
                src/solvers/dlx.c src/solvers/backtracking.c src/solvers/backtracking_mrv_bitmask.c src/solvers/constrain_propagation.c src/solvers/rules.c src/solvers/solver.c \
                $(wildcard src/utils/*.c)
	$(CC) $(CFLAGS) -Isrc -Iinclude $^ -o $@ $(LIBS) -lm

# ---- datasets: conveniência ----
datasets: build/generate_datasets
	./build/generate_datasets
