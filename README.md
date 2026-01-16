# Rubiks-Cube-Solver
A fast lightweight Rubik's Cube Solver that implements Kociemba's two phase algorithm in C++

This solver focuses on:
- Performance
- Correctness (extensive google tests)
- Simplicity (the solver itself has zero external dependencies)
- Optional SQLite logging module
- Scalability with OpenMPI

---

## Features
- Implements Kociemba's two phase algorithm
- Solves any valid 3x3x3 Rubik's cube
- Simple Makefile to build ("make run")
- SQLite database that stores initial cubes, solutions, time to solve, and nodes explored
- Allows the user to specify the number of threads and how many total cubes they want to solve
- Docker file so you can build images for linux and windows
- Supports OpenMPI to run on a cluster computer

---

### Prerequisites
- C++ compiler that supports C++20 (I used g++)
- 'make'
- sqlite3 version 3.50.3 (version that I'm using)
- GoogleTest for testing
- Docker to build or run docker images
- OpenMPI for cluster computing (optional)

---

### Build & run
Clone the repository and use "make run" which will run a speed test on 1000 cubes, or do "./bin/solver {option} {number of cubes}".
For option 3 you can specify the number of threads you want to run by doing "./bin/solver 3 {total number of cubes} {number of threads}".
For options:
- 1: speed test for solving the specified number of cubes
- 2: single threaded solving and logging of the specified number of cubes
- 3: multi threaded solving and logging of the specified number of cubes (total cubes not cubes per thread)

you can also pull the docker image I pushed:
vvrrooomm/rubiks-solver-single:arm64 [this was built on a Raspberry pi 5 it runs on windows but is much slower]
vvrrooomm/rubiks-solver-single:amd64 [this was built on a Windows computer with an Intel CPU]
vvrrooomm/rubiks-solver-multi:arm64 [this was built on a Raspberry pi 5 and intended to be used on a cluster of Raspberry Pi 5's as the name suggests it's on a 64 bit operating system]

to use the rubiks-solver-single just do:
docker pull [the image you want]
docker run -v "[path to where results will save]:/app/solve_logs" [image you pulled]
the "[path to where results will save]" will be where the solves.db is saved on your computer

to use the rubiks-solver-multi all nodes must have docker and OpenMPI to work.
On every node download docker and OpenMPI and do:
docker pull vvrrooomm/rubiks-solver-multi:arm64

then on the manager node you can run anywhere with the hosts.txt for your own cluster.

Then to run do the command:
mpirun -n [number of nodes] --hostfile [host file name, ie. hosts.txt]   bash -lc '
    docker run --rm \
      -e RANK=$OMPI_COMM_WORLD_RANK \
      -e SIZE=$OMPI_COMM_WORLD_SIZE \
      -v "[where you want to save the results]:/app/solve_logs" \
      vvrrooomm/rubiks-solver-multi:arm64 \
      [number of cubes]
  '

my exact command is:

mpirun -n 10 --hostfile hosts.txt bash -lc '
    docker run --rm \
      -e RANK=$OMPI_COMM_WORLD_RANK \
      -e SIZE=$OMPI_COMM_WORLD_SIZE \
      -v "/home/pi/solve_logs:/app/solve_logs" \
      vvrrooomm/rubiks-solver-multi:arm64 \
      [number of cubes]
  '

here I would run on 10 nodes/slots, and the results will be under /home/pi/solve_logs in solves.db
over the 10 nodes/slots each would solve (2000/10 = 200) 200 cubes.

Then copy them back and merge the results to a main database.
---

### Algorithm Overview
This solver uses Kociemba's Two-Phase Algorithm, which splits solving into 2 phases.

Phase 1:
All corner and edge orientations are 0 and all E slice edges are in the E slice.

Phase 2:
Solves the cube completely

---

### Hardware

Laptop Hardware:
- cpu: Intel(R) Core(TM) Ultra 9 185H
- 6 performance cores (hyper threading is on so 12 logical performance threads): 5.1 GHz
- 8 efficient cores: 3.8 GHz
- 2 low power efficient cores: 2.3 GHz
- RAM: 32 GB @ 6400 MT/s

Raspberry pi 5 Hardware (32 bit and 64 bit variations):
- cpu: Broadcom BCM2712 2.4 GHz quad-core
- 4 cores: 2.4 GHz
- RAM: 16 GB

---

### Benchmarks
- Build: uses -O3 compiler flag

Laptop Results (seed 123456, on 1,000 cubes averaged over 3 runs after running once) without logging:
- Average solve time: 3.8870 ms
- Average solution length: 23.098 moves
- Average phase 1 nodes explored: 149,456
- Average phase 2 nodes explored: 544,844

Raspberry pi 5 (32 bits) Results (seed 123456, on 1,000 cubes averaged over 3 runs after running once) without logging:
- Average solve time: 12.2766 ms
- Average solution length: 23.098 moves
- Average phase 1 nodes explored: 149,456
- Average phase 2 nodes explored: 544,844

Raspberry pi 5 (64 bits) Results (seed 123456, on 1,000 cubes averaged over 3 runs after running once) without logging:
- Average solve time: 10.931 ms
- Average solution length: 23.098 moves
- Average phase 1 nodes explored: 149,456
- Average phase 2 nodes explored: 544,844

Laptop Results (seed 123456, on 1,000 cubes averaged over 3 runs after running once) with logging:
- Total solve and log time: 3.9357 seconds
- Average solve and log time per cube: 3.9357 ms

Raspberry pi 5 (32 bits) Results (seed 123456, on 1,000 cubes averaged over 3 runs after running once) with logging:
- Total solve and log time: 12.2677 seconds
- Average solve and log time per cube: 12.2677 ms

Raspberry pi 5 (64 bits) Results (seed 123456, on 1,000 cubes averaged over 3 runs after running once) with logging:
- Total solve and log time: 10.992 seconds
- Average solve and log time per cube: 10.992 ms

Laptop Multi Threading Results:
- the 2nd column labelled average over 4 x 25,000 cube runs means that for each number of threads I measured the total speed to scramble and solve 25,000 cubes 4 times and averaged it out for the calulation
- the 3rd column labelled measured 1 x 100,000 cube run means that I had that number of threads scrambling and solving 100,000 cubes and recorded the total time to scramble, solve and log the information

| number of solving threads | average over 4 x 25,000 cube runs (cubes per second) | measured over 1 x 100,000 cube runs (cubes per second)|
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

Raspberry pi 5 (32 bits) Multi Threading Results:
- the 2nd column labelled average over 4 x 25,000 cube runs means that for each number of threads I measured the total speed to scramble and solve 25,000 cubes 4 times and averaged it out for the calulation
- the 3rd column labelled measured 1 x 100,000 cube run means that I had that number of threads scrambling and solving 100,000 cubes and recorded the total time to scramble, solve and log the information

| number of solving threads | average over 4 x 25,000 cube runs (cubes per second) | measured over 1 x 100,000 cube runs (cubes per second)|
|:-----------------:|:---------------------------------:|:----------------------------------:|
| 1 | 74.613 | 74.9735 |
| 2 | 142.072 | 142.742 |
| 3 | 211.188 | 210.550 |

Raspberry pi 5 (64 bits) Multi Threading Results:
- the 2nd column labelled average over 4 x 25,000 cube runs means that for each number of threads I measured the total speed to scramble and solve 25,000 cubes 4 times and averaged it out for the calulation
- the 3rd column labelled measured 1 x 100,000 cube run means that I had that number of threads scrambling and solving 100,000 cubes and recorded the total time to scramble, solve and log the information

| number of solving threads | average over 4 x 25,000 cube runs (cubes per second) | measured over 1 x 100,000 cube runs (cubes per second)|
|:-----------------:|:---------------------------------:|:----------------------------------:|
| 1 | 89.151 | 87.810 |
| 2 | 164.059 | 163.09 |
| 3 | 233.125 | 229.996 |

Raspberry pi 5 (64 bits) Multi Threading Cluster Results:
- I took the number of nodes multiplied by 100,000 cubes to get try measuring how linearly the cluster grows
- measured solving speed is the total time that it takes all nodes to solve their specified number of cubes, it's measured from the first time a node starts to the last node finishing solving.
- each node records their results loclly
- I then used a shell script to copy the database back to the manager and delete the database on the workers, then I merged the temporary worker databases with the master database.
- The entire process of solving, copying and merging is measured and tht's what the throughput speed measured

| number of nodes (pi 5 64 bits) | measured solving speed | measured throughput speed |
|:------------------------------:|:----------------------:|:-------------------------:|
| 1 | 218.019 | 218.019 |
| 2 | 446.170 | 445.208 |
| 3 | 658.954 | 649.384 |
| 4 | 872.474 | 860.028 |
| 5 | 1100.602 | 1082.515 |
| 6 | 1266.937 | 1235.661 |
| 7 | 1470.721 | 1421.556 |
| 8 | 1691.142 | 1620.353 |
| 9 | 1901.750 | 1828.255 |
| 10 | 2124.531 | 2047.053 |

* note at 1 node, the manager is where results are compiled so no copying of data or merging of databases occurred

---

### Project Structure
- bin: all executables are compiled here
- docker: docker instructions to build an image
- include: all .h fles
- obj: all .o files
- solve_logs: sqlite3 database of all solves
- src: all .cpp files including the 3 google tests
- tables: tables used by the solver, heuristics and transition
- Makefile
- README.md
- .dockerignore

---

### TODO
- [x] add SQLite to log results
- [x] add command line arguments/input
- [x] add multi threading for solving cubes
- [x] create docker images for portability and consistency
- [x] implement MPI so it can run on a cluster computer

