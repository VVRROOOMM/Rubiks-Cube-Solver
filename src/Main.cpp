#include <iostream>

#include "Cube.h"
#include "Tables.h"
#include "Solver.h"
#include "DatabaseLogger.h"
#include "DBCube.h"

#include <chrono>

using namespace std;

using s_clock = chrono::high_resolution_clock;

#ifndef UNIT_TEST
int main(int argc, char* argv[]) {
	/*
	these are what modes to run in
	options:
		1: speed mode no logging
		2: single threaded logging
	
	num_cubes: number of cubes you want, this defaults to 1000 cubes unless specified
	*/
	int option = 1;
	int num_cubes = 1000;
	
	//the second argument is option
	if (argc >= 2) {
		//validate the option was an integer
		try
		{
			option = stoi(string(argv[1]));
			if (option <= 0) {
				cout << "option was not a positive integer, program will not run" << endl;
			}
		}
		catch (const invalid_argument& e) {
			cerr << "invalid value of option given" << endl;
			return 0;
		}
		catch (const out_of_range& e) {
			cerr << "out of range options value given" << endl;
			return 0;
		}
	}
	
	//third argument is optional and is the num_cubes to solve (and possibly log)
	if (argc >= 3) {
		//ensure it's an intger
		try
		{
			num_cubes = stoi(string(argv[2]));
			if (num_cubes <= 0) {
				cout << "num_cubes was not a positive integer, program will not run" << endl;
				return 0;
			}
		}
		catch (const invalid_argument& e) {
			cerr << "invalid num_cubes value given" << endl;
			return 0;
		}
		catch (const out_of_range& e) {
			cerr << "out of range num_cubes value given" << endl;
			return 0;
		}
	}
	else {
		cout << "no num_cubes specified, defaulting to: " << num_cubes << endl;
	}
	
	//initialize solver so cube.cpp and solver.cpp work
	Solver::initializeSolver();

	vector<Cube> cubes;
	vector<DBCube> cubes_to_log;
	
	cubes.reserve(num_cubes);
	
	if (option == 1) {
		//generate num_cubes cubes using this specific seed, change if you want but 123456 was used to benchmark
		//note the benchmark was made using only 1000 cubes
		Cube::generateCubes(cubes, 123456, num_cubes);

		//solve them all
		Solver::solveWrapperSpeed(cubes, true);
	}
	else if (option == 2) {
		//note that the benchmarks also used 123456 as the specific seed as .the speed test used the same speed and this gives a good baseline
		//to switch to benchmarking comment out the generateCubes line with the random seed and uncomment out the set seed, start, end and duration related lines
		cubes_to_log.reserve(num_cubes);
		
		random_device rd;
		
		//generate num_cubes cubes using a random seed
		//Cube::generateCubes(cubes, 123456, num_cubes);
		Cube::generateCubes(cubes, rd(), num_cubes);
		//auto start = s_clock::now();
		//solve them all
		Solver::solveWrapper(cubes, cubes_to_log, false);
		
		//log the results
		DatabaseLogger logger("solve_logs/solves.db", 1);
		logger.sqlite3_log_db(cubes_to_log);
		//auto end = s_clock::now();
		//chrono::duration<double, milli> elapsed_time{0};
		
		//elapsed_time = end - start;
		
		//cout << "time taken to log " << num_cubes << " cubes is: " << elapsed_time.count() << endl;
	}
	else {
		cout << "invalid option given" << endl;
		return 0;
	}
	
	cout << "program done" << endl;
	
	return 0;
}
#endif