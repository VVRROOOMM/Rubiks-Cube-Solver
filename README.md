# Rubiks-Cube-Solver
A fast lightweight Rubik's Cube Solver that implements Kociemba's two phase algorithm in C++

This solver focuses on:
- Performance
- Correctness
- Simplicity (the solver engine has zero external dependencies)
- Optional SQLite logging module

---

## Features
- Implements Kociemba's two phase algorithm
- Solves any valid 3x3x3 Rubik's cube
- Simple Makefile to build ("make run")
- SQLite database that stores initial cubes, solutions, time to solve, and nodes explored

---

### Prerequisites
- C++ compiler that supports C++20 (I used g++)
- 'make'
- sqlite3 version 3.50.3 (version that I'm using)

---

### Build & run
Clone the repository and use "make run" which will run a speed test on 1000 cubes, or do "./bin/solver {option} {number of cubes}". 
For options:
- 1: speed test for solving the specified number of cubes
- 2: single threaded solving and logging of the specified number of cubes

---

### Algorithm Overview
This solver uses Kociemba's Two-Phase Algorithm, which splits solving into 2 phases.

Phase 1:
All corner and edge orientations are 0 and all E slice edges are in the E slice.

Phase 2:
Solves the cube completely

---

### Benchmarks
Hardware:
- cpu: Intel(R) Core(TM) Ultra 9 185H
- Threads: single threaded
- Build: uses -O3 compiler flag

Results (seed 123456, on 1,000 cubes averaged over 3 runs after running once) without logging:
- Average solve time: 6.479 ms
- Average solution length: 23.098 moves
- Average phase 1 nodes explored: 149,456
- Average phase 2 nodes explored: 544,844

Results (seed 123456, on 1,000 cubes averaged over 3 runs after running once) with logging:
- Total solve and log time: 6.6851 seconds
- Average solve and log time per cube: 6.685 ms

---

### Project Structure
- bin: all executables are compiled here
- include: all .h fles
- obj: all .o files
- solve_logs: sqlite3 database of all solves
- src: all .cpp files
- tables: tables used by the solver, heuristics and transition
- Makefile
- README.md

---

### TODO
- [x] add SQLite to log results
- [x] add command line arguments/input
- [ ] add multi threading for solving cubes
- [ ] create docker images for portability
- [ ] implement MPI so it can run on a cluster computer
- [ ] add gui/frontend for insights

