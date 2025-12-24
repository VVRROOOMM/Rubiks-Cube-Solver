# Rubiks-Cube-Solver
An efficient, dependency free Rubik's Cube Solver that implements Kociemba's two phase algorithm in C++

This solver focuses on:
- Performance
- Correctness
- Simplicity (zero external dependencies)

---

## Features
-Implements Kociemba's two phase algorithm
-Solves any valid 3x3x3 Rubik's cube
-Simple Makefile to build ("make run")

---

### Prerequisites
- C++ compiler that supports C++20 (I used g++)
- 'make'

---

### Build & run
Clone the repository and use "make run"

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

Results:
- Average solve time: 8.162 ms
- Average solution length: 23.098 moves
- Average phase 1 nodes explored: 149,456
- Average phase 2 nodes explored: 544,844

---

### Project Structure
- src: all .cpp files
- include: all .h fles
- Makefile
- README.md

---

### TODO
- [ ] add SqlLite to log results
- [ ] create docker images for portability
- [ ] implement MPI so it can run on a cluster computer
- [ ] add command line arguments/input
- [ ] add gui/frontend for insights

