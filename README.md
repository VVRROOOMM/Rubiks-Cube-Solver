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
- Allows the user to specify the number of threads and how many total cubes they want to solve

---

### Prerequisites
- C++ compiler that supports C++20 (I used g++)
- 'make'
- sqlite3 version 3.50.3 (version that I'm using)

---

### Build & run
Clone the repository and use "make run" which will run a speed test on 1000 cubes, or do "./bin/solver {option} {number of cubes}".
For option 3 you can specify the number of threads you want to run by doing "./bin/solver 3 {total number of cubes} {number of threads}".
For options:
- 1: speed test for solving the specified number of cubes
- 2: single threaded solving and logging of the specified number of cubes
- 3: multi threaded solving and logging of the specified number of cubes (total cubes not cubes per thread)

---

### Algorithm Overview
This solver uses Kociemba's Two-Phase Algorithm, which splits solving into 2 phases.

Phase 1:
All corner and edge orientations are 0 and all E slice edges are in the E slice.

Phase 2:
Solves the cube completely

---

### Hardware

Hardware:
- cpu: Intel(R) Core(TM) Ultra 9 185H
- 6 performance cores (hyper threading is on so 12 logical performance threads): 5.1 GHz
- 8 efficient cores: 3.8 GHz
- 2 low power efficient cores: 2.3 GHz
- RAM: 32 GB @ 6400 MT/s

---

### Benchmarks
- Build: uses -O3 compiler flag

Results (seed 123456, on 1,000 cubes averaged over 3 runs after running once) without logging:
- Average solve time: 3.8870 ms
- Average solution length: 23.098 moves
- Average phase 1 nodes explored: 149,456
- Average phase 2 nodes explored: 544,844

Results (seed 123456, on 1,000 cubes averaged over 3 runs after running once) with logging:
- Total solve and log time: 3.9357 seconds
- Average solve and log time per cube: 3.9357 ms

Multi Threading Results:
- the 2nd column labelled average over 4 x 25,000 cube runs means that for each number of threads I measured the total speed to scramble and solve 25,000 cubes 4 times and averaged it out for the calulation
- the 3rd column labelled measured 1 x 100,000 cube run means that I had that number of threads scrambling and solving 100,000 cubes and recorded the total time to scramble, solve and log the information

| number of threads | average over 4 x 25,000 cube runs (cubes per second) | measured over 1 x 100,000 cube runs (cubes per second)|
|:-----------------:|:---------------------------------:|:----------------------------------:|
| 1 | 205.14 | 200.65 |
| 2 | 389.94 | 373.19 |
| 3 | 541.13 | 496.75 |
| 4 | 647.28 | 605.01 |
| 5 | 709.61 | 704.30 |
| 6 | 804.47 | 826.71 |
| 7 | 896.82 | 907.90 |
| 8 | 973.75 | 998.90 |
| 9 | 1054.36 | 1080.20 |
| 10 | 1157.81 | 1180.06 |
| 11 | 1258.33 | 1276.97 |
| 12 | 1310.55 | 1362.32 |
| 13 | 1444.47 | 1420.11 |
| 14 | 1482.35 | 1456.77 |
| 15 | 1566.47 | 1500.95 |
| 16 | 1652.30 | 1556.73 |
| 17 | 1679.59 | 1610.67 |
| 18 | 1732.03 | 1648.48 |
| 19 | 1695.57 | 1690.06 |

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
- [x] add multi threading for solving cubes
- [ ] create docker images for portability and consistency
- [ ] implement MPI so it can run on a cluster computer
- [ ] add gui/frontend for insights

