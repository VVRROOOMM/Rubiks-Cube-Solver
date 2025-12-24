/*
TO DO
go through tests and stuff and look for unused stuff
*/

#include <iostream>

#include "Cube.h"
#include "Tables.h"
#include "Solver.h"

#include <chrono>

using namespace std;

using s_clock = chrono::high_resolution_clock;

#ifndef UNIT_TEST
int main() {
	//initialize solver so cube.cpp and solver.cpp work
	Solver::initializeSolver();

	vector<Cube> cubes;
	
	//generate 10,000 cubes using this seed
	Cube::generateCubes(cubes, 123456, 10000);
	
	//solve them all
	Solver::solveWrapper(cubes);
	
	return 0;
}
#endif