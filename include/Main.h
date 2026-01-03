//this .h file is just so DatabaseTests.cpp can access Main.cpp methods to test

#pragma once

#include "Cube.h"
#include "Tables.h"
#include "Solver.h"
#include "DatabaseLogger.h"
#include "DBCube.h"
#include "MultiThreadSolver.h"

#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

class Main {
	public:
		static void multiThreadLogger(int num_cubes, bool benchmark, int version, int num_threads);
		static void singleThreadLog(int num_cubes, bool benchmark, int version, bool print);
		static void speedRun(int seed, int num_cubes);
};