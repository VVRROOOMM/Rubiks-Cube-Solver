/*
quick check to ensure cubes don't show up multiple times that many times
comment stuff
update benchmarks
*/

#include <iostream>

#include "Main.h"
#include "DBCube.h"
#include "Solver.h"
#include "MultiThreadSolver.h"
#include "DatabaseLogger.h"

using namespace std;

using s_clock = chrono::high_resolution_clock;

void Main::multiThreadLogger(int num_cubes, bool benchmark, int version, int num_threads)
{	
	auto start = s_clock::now();
	auto end = s_clock::now();
	chrono::duration<double, milli> elapsed_time{0};
	
	mutex m;
	queue<DBCube> q;
	
	DatabaseLogger logger("solve_logs/solves.db", version);
	
	vector<thread> workers;
	workers.reserve(num_threads);

	int cubes_per_thread = num_cubes / num_threads;
	int remainder = num_cubes % num_threads;
	int extra_cube = 0;

	for (int i = 0; i < num_threads; i++) {
		if (remainder > 0) {
			extra_cube = 1;
			remainder--;
		}
		else {
			extra_cube = 0;
		}
		workers.emplace_back(MultiThreadSolver::solveWrapperMulti, ref(m), ref(q), cubes_per_thread + extra_cube);
	}

	atomic<bool> end_program{false};

	thread db_log(&DatabaseLogger::sqlite3_log_db_multi, &logger ,ref(q), ref(m), ref(end_program));
	
	for (auto& t : workers) {
		t.join();
	}
	
	end_program.store(true);
	db_log.join();
	
	if (benchmark) {
		end = s_clock::now();
		elapsed_time = end - start;
		cout << num_cubes << " done in " << elapsed_time.count() << " ms" << endl;
		cout << elapsed_time.count() / num_cubes << " ms/cube" << endl;
		cout << (num_cubes * 1000) / elapsed_time.count() << " cubes / s" << endl;
	}
	
	cout << "done running " << num_threads << " threads" << endl;
}

void Main::singleThreadLog(int num_cubes, bool benchmark, int version, bool print)
{
	//note that the benchmarks also used 123456 as the specific seed as .the speed test used the same speed and this gives a good baseline
	//to switch to benchmarking comment out the generateCubes line with the random seed and uncomment out the set seed, start, end and duration related lines
	vector<Cube> cubes;
	vector<DBCube> cubes_to_log;
	
	cubes.reserve(num_cubes);
	
	cubes_to_log.reserve(num_cubes);
	
	random_device rd;
	
	if (benchmark) {
		auto start = s_clock::now();
		//generate num_cubes cubes using the benchmarking seed
		Cube::generateCubes(cubes, 123456, num_cubes);
		
		//solve them all
		Solver::solveWrapper(cubes, cubes_to_log, print);	
		auto end = s_clock::now();
		chrono::duration<double, milli> elapsed_time{0};

		elapsed_time = end - start;

		cout << "time taken to log " << num_cubes << " cubes is: " << elapsed_time.count() << endl;
	}
	else {
		//generate num_cubes cubes using a random seed
		Cube::generateCubes(cubes, rd(), num_cubes);

		//solve them all
		Solver::solveWrapper(cubes, cubes_to_log, print);	
	}
	
	//log the results
	DatabaseLogger logger("solve_logs/solves.db", version);
	logger.sqlite3_log_db(cubes_to_log);
}

void Main::speedRun(int seed, int num_cubes)
{
	vector<Cube> cubes;
	cubes.reserve(num_cubes);
	
	//generate num_cubes cubes using this specific seed, change if you want but 123456 was used to benchmark
	//note the benchmark was made using only 1000 cubes
	Cube::generateCubes(cubes, seed, num_cubes);

	//solve them all
	Solver::solveWrapperSpeed(cubes, true);
}

#ifndef UNIT_TEST
int main(int argc, char* argv[]) {
	int version = 1;
	
	/*
	these are what modes to run in
	options:
		1: speed mode no logging
		2: single threaded logging
		3: multi threaded logging
	
	num_cubes: number of cubes you want, this defaults to 1000 cubes unless specified
	option: defaults to 1 if not specified
	num_threads: defaults to 4 if not specified
	*/
	int option = 1;
	int num_cubes = 1000;
	int num_threads = 4;
	
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
	
	//fourth argument is optional and is the num_threads to use
	if (argc >= 4) {
		//ensure it's an intger
		try
		{
			num_threads = stoi(string(argv[3]));
			if (num_threads <= 0) {
				cout << "num_threads was not a positive integer, program will not run" << endl;
				return 0;
			}
		}
		catch (const invalid_argument& e) {
			cerr << "invalid num_threads value given" << endl;
			return 0;
		}
		catch (const out_of_range& e) {
			cerr << "out of range num_threads value given" << endl;
			return 0;
		}
	}
	else {
		cout << "no num_threads specified, defaulting to: " << num_threads << endl;
	}
	
	//initialize solver so cube.cpp and solver.cpp work
	Solver::initializeSolver();

	vector<Cube> cubes;
	vector<DBCube> cubes_to_log;
	
	cubes.reserve(num_cubes);
	
	if (option == 1) {
		//change this seed if you want, but this is the one benchmarked against originally (123456)
		Main::speedRun(123456, num_cubes);
	}
	else if (option == 2) {
		Main::singleThreadLog(num_cubes, true, version, false);
	}
	else if (option == 3) {
		for (int r = 0; r < 4; r++) {
			for (int i = 1; i <= 19; i++) {
				Main::multiThreadLogger(25000, true, version, i);
				cout << endl;
			}
			cout << endl;
		}
		
		for (int i = 1; i <= 19; i++) {
			Main::multiThreadLogger(100000, true, version, i);
			cout << endl;
		}
		
		//Main::multiThreadLogger(num_cubes, true, version, num_threads);
	}
	else {
		cout << "invalid option given" << endl;
		return 0;
	}
	
	cout << "program done" << endl;
	
	return 0;
}
#endif
