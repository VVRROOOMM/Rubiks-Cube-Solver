#include <iostream>

#include "Main.h"
#include "DBCube.h"
#include "Solver.h"
#include "MultiThreadSolver.h"
#include "DatabaseLogger.h"

using namespace std;

using s_clock = chrono::high_resolution_clock;

//this method creates num_threads + 1 worker threads and 1 solver thread, so take number of threads you want and subtract 1 to use this
void Main::multiThreadLogger(int num_cubes, bool benchmark, double version, int num_threads)
{	
	auto start = s_clock::now();
	auto end = s_clock::now();
	chrono::duration<double, milli> elapsed_time{0};
	
	//these will be shared queue and mutex for multithreading
	mutex m;
	queue<DBCube> q;
	
	//open the logger
	DatabaseLogger logger("solve_logs/solves.db", version);
	
	//create a vector of threads to join later
	vector<thread> workers;
	workers.reserve(num_threads);

	//so the thread calling/running this method doesn't sit idle +1 to num_threads so this one will run the solver as well
	int cubes_per_thread = num_cubes / (num_threads + 1);
	int remainder = num_cubes % (num_threads + 1);
	int extra_cube = 0;

	//creates the threads
	for (int i = 0; i < num_threads; i++) {
		if (remainder > 0) {
			extra_cube = 1;
			remainder--;
		}
		else {
			extra_cube = 0;
		}
		//this print statement is a debugging statement comment out if you want however note that on a cluster computer this will be messy
		cout << "thread: " << i << " created to solve " << cubes_per_thread + extra_cube << endl;
		//the threads call solveWrapperMulti in MultiThreadSolver.cpp
		workers.emplace_back(MultiThreadSolver::solveWrapperMulti, ref(m), ref(q), cubes_per_thread + extra_cube);
	}

	//to signal we are leaving
	atomic<bool> end_program{false};

	//call the logger as a thread
	thread db_log(&DatabaseLogger::sqlite3_log_db_multi, &logger ,ref(q), ref(m), ref(end_program));
	
	//this thread is just sitting idle so just send it to solve cubes
	MultiThreadSolver::solveWrapperMulti(ref(m), ref(q), cubes_per_thread);
	
	for (auto& t : workers) {
		t.join();
	}
	
	//all threads join so we are done end logger
	end_program.store(true);
	db_log.join();
	
	//print info if needed
	if (benchmark) {
		end = s_clock::now();
		elapsed_time = end - start;
		cout << num_cubes << " done in " << elapsed_time.count() << " ms" << endl;
		cout << elapsed_time.count() / num_cubes << " ms/cube" << endl;
		cout << (num_cubes * 1000) / elapsed_time.count() << " cubes / s" << endl;
	}
	
	cout << "done running " << (num_threads + 1) << " threads" << endl;
}

void Main::singleThreadLog(int num_cubes, bool benchmark, double version, bool print)
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

		cout << "time taken to log " << num_cubes << " cubes is: " << elapsed_time.count() << " ms" << endl;
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

//used if mpi isn't being used
int Main::singleComputerMain(int argc, char* argv[]) {
	double single_version = 1.1;
	double multi_version = 1.2;
	
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
			return 1;
		}
		catch (const out_of_range& e) {
			cerr << "out of range options value given" << endl;
			return 1;
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
				return 1;
			}
		}
		catch (const invalid_argument& e) {
			cerr << "invalid num_cubes value given" << endl;
			return 1;
		}
		catch (const out_of_range& e) {
			cerr << "out of range num_cubes value given" << endl;
			return 1;
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
				return 1;
			}
		}
		catch (const invalid_argument& e) {
			cerr << "invalid num_threads value given" << endl;
			return 1;
		}
		catch (const out_of_range& e) {
			cerr << "out of range num_threads value given" << endl;
			return 1;
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
		Main::singleThreadLog(num_cubes, false, single_version, false);
	}
	else if (option == 3) {
		//this code was to generate the data for benchmarking multi threaded code
		//r is the number of runs to complete this does 4 runs of 25,000 cubes
		//the thread_max may fail if you have 1 thread or c++ can't determine the number of cores you have
		/*int thread_max = (int) (thread::hardware_concurrency() - 2);
		for (int r = 0; r < 4; r++) {
			for (int i = 0; i <= thread_max - 1; i++) {
				Main::multiThreadLogger(25000, true, multi_version, i);
				cout << endl;
			}
			cout << endl;
		}
		
		for (int i = 1; i <= 19; i++) {
			Main::multiThreadLogger(100000, true, multi_version, i);
			cout << endl;
		}*/
		
		Main::multiThreadLogger(num_cubes, true, multi_version, num_threads);
	}
	else {
		cout << "invalid option given" << endl;
		return 1;
	}
	
	return 0;
}

//this is what's being called on a node in a cluster computer
int Main::clusterComputerMain(int argc, char* argv[], int rank, int size)
{
	//default to solving a total of 100,000 cubes
	int num_cubes = 100000;
	int num_threads;
	
	//set to 2 threads as default, change if you want otherwise we subtract 2 here so that we add 1, meaning only 1 thread handles logging
	if (thread::hardware_concurrency() <= 2) {
		num_threads = 2;
	}
	else {
		num_threads = (int) (thread::hardware_concurrency() - 2);
	}
	
	//current version of the solver
	double version = 1.3;
	
	//trying to get the number of cubes
	if (argc >= 2) {
		try
		{
			num_cubes = stoi(string(argv[1]));
			if (num_cubes <= 0) {
				cout << "num_cubes was not a positive integer, program will not run" << endl;
				return 1;
			}
		}
		catch (const invalid_argument& e) {
			cerr << "invalid num_cubes value given" << endl;
			return 1;
		}
		catch (const out_of_range& e) {
			cerr << "out of range num_cubes value given" << endl;
			return 1;
		}
	}
	
	//for my design node 0 doesn't do anything special it just solves cubes and logs
	int cubes_per_node = num_cubes / size;
	int remaining_cubes = num_cubes % size;
	
	if (rank < remaining_cubes) {
		cubes_per_node++;
	}
	
	//load/create the solver info like tables and stuff
	MultiThreadSolver::initializeSolver();
	
	multiThreadLogger(cubes_per_node, true, version, num_threads);
	
	return 0;
}

#ifndef UNIT_TEST
int main(int argc, char* argv[]) 
{
	//get the rank input and size input from openmpi
	const char* rank_input = getenv("RANK");
	const char* size_input = getenv("SIZE");
	
	//debugging statement, comment out if you want
	if (rank_input) {
		cout << rank_input << endl;
	}
	if (size_input) {
		cout << size_input << endl;
	}

	int result = 0;
	
	if (rank_input && size_input) {
		//if they aren't null and exist mpi should pass in valid integers
		int rank = stoi(rank_input);
		int size = stoi(size_input);
		
		//debugging statement, comment out if you want
		cout << "MPI detected, node: " << rank << " running and calling cluster main" << endl;
		
		//call the cluster main
		result = Main::clusterComputerMain(argc, argv, rank, size);
	}
	else {
		//if we are not using mpi, the solver runs on the single computer
		result = Main::singleComputerMain(argc, argv);
	}
	
	return result;
}
#endif
