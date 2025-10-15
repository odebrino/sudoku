# Sudoku Solver & Generator  

This project was developed as part of the **Imperative Programming course (ENSTA Paris, 2025–2026)**.  
It implements:  
- A **Sudoku solver** (with traceability of solving steps).  
- A **Sudoku puzzle generator** with configurable difficulty.  
- A second phase with a **graphical interface**.  

---

## 📌 Features  

- ✅ **Solver**: solves 9×9 Sudoku grids using backtracking and heuristics.  
- ✅ **Traceability**: logs each hypothesis and elimination during solving.  
- ✅ **Hint mode**: suggests one cell (“coup de pouce”) to help the player.  
- ✅ **Save & resume**: allows interrupting and resuming a game.  
- ✅ **Generator**: produces valid Sudoku grids with adjustable difficulty.  
- ✅ **Unit tests**: cover solver, generator, and utility functions.  
- ✅ **Benchmarks**: measure algorithmic performance.  

---

## 📂 Project structure  

.
├── build/ # Compiled binaries and objects
├── data/ # Sample inputs and outputs
│ ├── inputs/
│ └── outputs/
├── include/ # Header files (.h)
│ ├── benchmark.h
│ ├── generator.h
│ ├── interface.h
│ ├── solver.h
│ └── utils.h
├── plots/ # Benchmark results and plots
├── scripts/ # Automation and execution scripts
├── src/ # Main source code
│ ├── benchmark/
│ ├── generator/
│ ├── solvers/
│ └── utils/
├── tests/ # Unit tests
│ ├── test_benchmark.c
│ ├── test_generator.c
│ └── test_solver.c
└── Makefile # Project build rules

yaml
Copiar código

---

## 🚀 Build & run  

### Build  
```bash
make
Run solver
bash
Copiar código
./build/bin/sudoku_solver data/inputs/example.txt
Run generator
bash
Copiar código
./build/bin/sudoku_generator --difficulty medium
Run tests
bash
Copiar código
make test
Run benchmarks
bash
Copiar código
./build/bin/benchmark
🧩 Example
Input (example.txt):

Copiar código
530070000
600195000
098000060
800060003
400803001
700020006
060000280
000419005
000080079
Output (solution):

Copiar código
534678912
672195348
198342567
859761423
426853791
713924856
961537284
287419635
345286179
📖 Academic context
This project follows the official assignment specifications:

Solver with traceability of steps.

Hint mode (“coup de pouce”).

Game persistence (save & resume).

Generator with configurable complexity.

👨‍💻 Authors
ODE BRINO Rafael
RIZO Maria Eduarda

(ENSTA — 2025/2026)