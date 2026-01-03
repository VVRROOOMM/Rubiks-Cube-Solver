#include "MultiThreadSolver.h"

using namespace std;

using s_clock = chrono::high_resolution_clock;

const size_t p1MoveGroupMulti[NUM_P1_MOVES_MULTI] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5};
const size_t p2MoveGroupMulti[NUM_P2_MOVES_MULTI] = {0, 0, 0, 1, 2, 3, 4, 5, 5, 5};

void MultiThreadSolver::solveWrapperMulti(mutex& m, queue<DBCube>& sharedQueue, int cubes_to_do)
{
	uint8_t solution_len = 0;
	uint8_t solution2_len = 0;
	vector<int> solutions;

	uint64_t curr_p1counter = 0;
	uint64_t curr_p2counter = 0;
	
	DBCube cube_data;
	
	auto start = s_clock::now();
	auto end = s_clock::now();
	chrono::duration<double, milli> curr_time{0};
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	random_device rd;
	
	//assign solutions with dummy values
	solutions.assign(50, -1);
	
	for (int n = 0; n < cubes_to_do; n++) {
		solution_len = 0;
		solution2_len = 0;
		curr_p1counter = 0;
		curr_p2counter = 0;
		
		cube.scramble(rd());
		
		start = s_clock::now();
		//calls solve to solve while timing it
		bool result = MultiThreadSolver::solveMulti(solutions, cube, cube_data, solution_len, solution2_len, curr_p1counter, curr_p2counter);
		end = s_clock::now();
		
		//total_time tracks total time spent solving the vector of cubes
		curr_time = end - start;
		
		//if the solver runs into an issue, the thread leaves and the final check to ensure the specified number of cubes were logged will fail
		if (!result) {
			cerr << "exiting solve wrapper" << endl;
			return;
		}
		
		//debug statements
		//cube.printInfo();
		//cout << endl << endl;
		
		//safety check to ensure the cube is solved
		if (!cube.isSolved()) {
			cerr << "cube: " << n << " wasn't solved" << endl;
			cube.printInfo();
			return;
		}
		
		//sqlite logging
		cube_data.set_time(curr_time.count() / 1.0);
		cube_data.set_phase1_nodes(curr_p1counter);
		cube_data.set_phase2_nodes(curr_p2counter);
		
		{
			lock_guard<mutex> guard(m);
			sharedQueue.push(cube_data);
		}
	}
}

//solves a given cube, vector<int>& must be filled with -1 and valid size (assumption made by code)
//cube_data stores relevant information such as the length of the solution, and the initial state
bool MultiThreadSolver::solveMulti(vector<int>& solutions, Cube& cube, DBCube& cube_data, uint8_t& solution_len, uint8_t& solution2_len, uint64_t& curr_p1counter, uint64_t& curr_p2counter)
{	
	//DBCube info
	string initial = cube.getStringKey();
	string solution = "";
	solutions.resize(128);

	//moves in string representation for debug statements
	vector<string> p1MoveStrings = {"U", "U'", "U2", "L", "L'", "L2", "F", "F'", "F2", "R", "R'", "R2", "B", "B'", "B2", "D", "D'", "D2"};
	
	vector<string> p2MoveStrings = {"U", "U'", "U2", "L2", "F2", "R2", "B2", "D", "D'", "D2"};
	
	//debug statement
	//cout << "phase 1" << endl;
	
	//calls phase 1 solver
	phase1SolverWrapper(solutions, cube, solution_len, curr_p1counter);
	
	//apply the moves to get to phase1 completion
	for (int i = 0; i < solution_len; i++) {
		cube.rotate(p1MoveStrings[solutions[i]]);
		//cout << p1MoveStrings[solutions[i]] << endl;
		solution += p1MoveStrings[solutions[i]] + " ";
	}
	
	//debug statement
	//cube.printInfo();
	
	//ensures cube has completed phase 1 correctly or else phase 2 will run until stopped (impossible to solve)
	//tested on a bunch of cubes, uncomment if you want
	/*if (!cube.phase1Solved()) {
		cerr << "CUBE HAS NOT SOLVED PHASE 1 CORRECTLY" << endl;
		return false;
	}*/
	
	//debug statement
	//cout << "phase 2" << endl;
	
	//calls phase 2 solver
	phase2SolverWrapper(solutions, cube, solution_len, solution2_len, curr_p2counter);
	
	//apply moves to solve cube
	for (size_t i = solution_len; i < solution_len + solution2_len; i++) {
		cube.rotate(p2MoveStrings[solutions[i]]);
		//cout << p2MoveStrings[solutions[i]] << endl;
		solution += p2MoveStrings[solutions[i]] + " ";
	}
	
	//ensure the cube is solved
	//tested on a bunch of cubes, uncomment if you want
	/*if (!cube.isSolved()) {
		cube.printInfo();
		cerr << "CUBE WAS NOT SOLVED CORRECTLY" << endl;
		return false;
	}*/
	
	//sqlite logging
	cube_data.set_initial_cube(initial);
	if (!solution.empty()) {
		solution.pop_back();
	}
	cube_data.set_solution(solution);
	
	return true;
}

//phase 1 solver wrapper to run the IDA* search until we solve phase 1
void MultiThreadSolver::phase1SolverWrapper(vector<int>& solutions, Cube& cube, uint8_t& solution_len, uint64_t& curr_p1counter)
{
	//initial keys
	uint16_t eoKey = cube.p1eoIndex();
	uint16_t coKey = cube.p1coIndex();
	uint16_t eKey = cube.p1eSliceIndex();
	
	//old heuristic table usage
	//int max_cost = max(max(elHeuristicsTableRef[elKey], clHeuristicsTableRef[clKey]), ep2HeuristicsTableRef[eKey]);
	
	//getting estimate
	int max_cost = max(eocoHeuristicsTableRef[eoKey * CO_SIZE + coKey], eHeuristicsTableRef[eKey]);
	
	//call the solver once
	max_cost = phase1Solver(solutions, max_cost, 0, eoKey, coKey, eKey, 100, solution_len, curr_p1counter);

	//keep running the solver every time it leaves with the new max encountered unless we get -1 (phase 1 solved)
	while (max_cost != -1) {
		max_cost = phase1Solver(solutions, max_cost, 0, eoKey, coKey, eKey, 100, solution_len, curr_p1counter);
	}
}

//phase 1 IDA* solver
int MultiThreadSolver::phase1Solver(vector<int>& solutions, int max_cost, int curr_cost, uint16_t eoKey, uint16_t coKey, uint16_t eKey, size_t prevMove, uint8_t& solution_len, uint64_t& curr_p1counter)
{
	//update node expansion
	curr_p1counter++;

	//if we're solved phase 1 leave
	if (Cube::phase1Solved(eoKey, coKey, eKey)) {
		return -1;
	}
	
	//old heuristic table usage
	//int estimate = max(max(eoHeuristicsTableRef[eoKey], coHeuristicsTableRef[coKey]), eHeuristicsTableRef[eKey]);
	
	/*int estimate = eoHeuristicsTableRef[eoKey];
	int co = coHeuristicsTableRef[coKey];
	int ep1 = eHeuristicsTableRef[eKey];
	
	if (co > estimate) {
		estimate = co;
	}
	
	if (ep1 > estimate) {
		estimate = ep1;
	}*/
	
	//getting estimate for the current node
	int estimate = eocoHeuristicsTableRef[eoKey * CO_SIZE + coKey];
	int ep1 = eHeuristicsTableRef[eKey];
	
	if (ep1 > estimate) {
		estimate = ep1;
	}
	
	if ((curr_cost + estimate) > max_cost) {
		return curr_cost + estimate;
	}
	
	int next_cost = curr_cost + 1;
	int min_cost = INT_MAX;
	
	//read onl pointers to transition tables with __restrict for compiler optimization
	const uint16_t* __restrict EORef = eoTransitionTableRef[eoKey];
	const uint16_t* __restrict CORef = coTransitionTableRef[coKey];
	const uint16_t* __restrict ERef = eTransitionTableRef[eKey];
	
	for (size_t i = 0; i < NUM_P1_MOVES_MULTI; i++) {
		if (p1MoveGroupMulti[i] != prevMove) {
			//"rotating" the cube via coordinate update (precomputed table in tables.cpp)
			uint16_t tempEOKey = EORef[i];
			uint16_t tempCOKey = CORef[i];
			uint16_t tempEKey = ERef[i];
			
			//recursive call for IDA* with new key values
			int result = phase1Solver(solutions, max_cost, next_cost, tempEOKey, tempCOKey, tempEKey, p1MoveGroupMulti[i], solution_len, curr_p1counter);
			
			//if the result is -1 (solved), update the solution vector and return -1
			if (result == -1) {
				solutions[curr_cost] = i;
				solution_len++;
				return -1;
			}
			
			//ifthe cube isn't solved try finding the lowest value that's above the current min_cost
			if (result < min_cost) {
				min_cost = result;
			}
		}
	}
	
	return min_cost;
}

//phase 2 solver wrapper to run the IDA* search until the cube is solved
void MultiThreadSolver::phase2SolverWrapper(vector<int>& solutions, Cube& cube, uint8_t& solution_len, uint8_t& solution2_len, uint64_t& curr_p2counter)
{
	//initial coordinates
	uint16_t elKey = cube.p2elIndex();
	uint16_t clKey = cube.p2clIndex();
	uint8_t eKey = cube.p2eSliceIndex();

	//old heuristic table usage
	//int max_cost = max(max(elHeuristicsTableRef[elKey], clHeuristicsTableRef[clKey]), ep2HeuristicsTableRef[eKey]);
	
	//initial estimate
	int max_cost = max(eleSliceHeuristicsTableRef[elKey * E_SIZE_P2 + eKey], cleSliceHeuristicsTableRef[clKey * E_SIZE_P2 + eKey]);

	//initial IDA* search
	max_cost = phase2Solver(solutions, max_cost, 0, elKey, clKey, eKey, 100, solution_len, solution2_len, curr_p2counter);

	//keep searching until the cube is solved
	while (max_cost != -1) {
		max_cost = phase2Solver(solutions, max_cost, 0, elKey, clKey, eKey, 100, solution_len, solution2_len, curr_p2counter);
	}	
}

//phase 2 IDA* solver
int MultiThreadSolver::phase2Solver(vector<int>& solutions, int max_cost, int curr_cost, uint16_t elKey, uint16_t clKey, uint8_t eKey, size_t prevMove, uint8_t& solution_len, uint8_t& solution2_len, uint64_t& curr_p2counter)
{
	//update node expansion
	curr_p2counter++;
	
	//if the coordinates are the solved coordinates leave
	if (elKey == 0 && clKey == 0 && eKey == 0) {
		return -1;
	}
	
	//old heuristic table usage
	//int estimate = max(max(elHeuristicsTableRef[elKey], clHeuristicsTableRef[clKey]), ep2HeuristicsTableRef[eKey]);
	
	/*int estimate = elHeuristicsTableRef[elKey];
	int cl = clHeuristicsTableRef[clKey];
	int ep2 = ep2HeuristicsTableRef[eKey];
	
	if (cl > estimate) {
		estimate = cl;
	}
	
	if (ep2 > estimate) {
		estimate = ep2;
	}*/
	
	//getting the current estimate
	int estimate = eleSliceHeuristicsTableRef[elKey * E_SIZE_P2 + eKey];
	int cleSlice = cleSliceHeuristicsTableRef[clKey * E_SIZE_P2 + eKey];
	
	if (cleSlice > estimate) {
		estimate = cleSlice;
	}
	
	//leave if we exceed the max_cost
	if ((curr_cost + estimate) > max_cost) {
		return curr_cost + estimate;
	}
	
	int next_cost = curr_cost + 1;
	int min_cost = INT_MAX;
	
	//pointers to transition tables with __restrict for compiler optimizations
	const uint16_t* __restrict elRef = elTransitionTableRef[elKey];
	const uint16_t* __restrict clRef = clTransitionTableRef[clKey];
	const uint8_t* __restrict ep2Ref = ep2TransitionTableRef[eKey];
	
	for (size_t i = 0; i < NUM_P2_MOVES_MULTI; i++) {
		if (prevMove != p2MoveGroupMulti[i]) {
			//updating coordinates to "rotate" the cube
			uint16_t tempELKey = elRef[i];
			uint16_t tempCLKey = clRef[i];
			uint8_t tempEKey = ep2Ref[i];
			
			//recursive IDA* search call
			int result = phase2Solver(solutions, max_cost, next_cost, tempELKey, tempCLKey, tempEKey, p2MoveGroupMulti[i], solution_len, solution2_len, curr_p2counter);
			
			//if solved record move and return -1
			if (result == -1) {
				solutions[curr_cost + solution_len] = i;
				solution2_len++;
				return -1;
			}
			
			//get the lowest cost we've seen that isn't solved (exceeds max cost estimate)
			if (result < min_cost) {
				min_cost = result;
			}
		}
	}
	
	return min_cost;
}

//initialize Cube tables and Tables to make Solver.cpp, and Cube.cpp functional
void MultiThreadSolver::initializeSolver()
{
	Cube::initialize();
	Tables::loadTables();
}
