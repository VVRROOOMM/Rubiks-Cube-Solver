#include "Solver.h"

using namespace std;

using s_clock = chrono::high_resolution_clock;

size_t p1MoveGroup[NUM_P1_MOVES] = {0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5};
size_t p2MoveGroup[NUM_P2_MOVES] = {0, 0, 0, 1, 2, 3, 4, 5, 5, 5};

uint8_t solution_len = 0;
uint8_t solution2_len = 0;

//remove these if you want, they just track expansion per phase
uint64_t p1counter = 0;
uint64_t p2counter = 0;

//given a vector of cubes we solve all of them and currently records information such as solution length, time to solve etc.
bool Solver::solveWrapper(vector<Cube>& cubes)
{
	solution_len = 0;
	solution2_len = 0;
	vector<int> solutions;
	int counter = 1;
	uint64_t total_len = 0;
	
	auto start = s_clock::now();
	auto end = s_clock::now();
	chrono::duration<double, milli> total_time{0};
	
	for (Cube& cube : cubes) {
		cout << "Solving Cube: " << counter << endl;
		counter++;
		//assign solutions with dummy values
		solutions.assign(50, -1);
		solution_len = 0;
		solution2_len = 0;
		
		start = s_clock::now();
		//calls solve(vector<int>&, Cube&) to solve while timing it
		bool result = solve(solutions, cube);
		end = s_clock::now();
		
		//total_time tracks total time spent solving the vector of cubes
		total_time += end - start;
		
		//total_len tracks total solution length
		total_len += solution2_len + solution_len;
		
		//print out node expansions seen so far (doesn't reset)
		cout << p1counter << endl;
		cout << p2counter << endl;
		
		//if the solver runs into an issue it returns false so return false
		if (!result) {
			cerr << "exiting solve wrapper" << endl;
			return false;
		}
		//cube.printInfo();
		//cout << endl << endl;
		
		//safety check to ensure the cube is solved
		if (!cube.isSolved()) {
			cerr << "cube: " << counter - 1 << " wasn't solved" << endl;
			cube.printInfo();
			return false;
		}
	}
	
	//these print statements output information: time take to solve cubes, average time per cube, average solution length, nodes explored, and how many nodes per phase explored
	double size = cubes.size() * 1.0;
	
	double milliseconds = total_time.count() / 1.0;
	
	cout << milliseconds << " miliseconds taken to solve " << cubes.size() << " cubes" << endl;
	cout << milliseconds / cubes.size() << " milliseconds per solve" << endl;
	
	cout << "average solution length: " << (total_len / size) << endl;
	cout << "average nodes explored: " << ((p1counter + p2counter) / size) << endl;
	cout << "average p1 nodes explored: " << (p1counter / size) << endl;
	cout << "average p2 nodes explored: " << (p2counter / size) << endl;
	
	return true;
}

//solves a given cube, vector<int>& must be filled with -1 and valid size (assumption made by code)
bool Solver::solve(vector<int>& solutions, Cube& cube)
{	
	//moves in string representation for debug statements
	vector<string> p1MoveStrings = {"U", "U'", "U2", "L", "L'", "L2", "F", "F'", "F2", "R", "R'", "R2", "B", "B'", "B2", "D", "D'", "D2"};
	
	vector<string> p2MoveStrings = {"U", "U'", "U2", "L2", "F2", "R2", "B2", "D", "D'", "D2"};
	
	//debug statement
	//cout << "phase 1" << endl;
	
	//calls phase 1 solver
	phase1SolverWrapper(solutions, cube);
	
	//apply the moves to get to phase1 completion
	for (int i = 0; i < solution_len; i++) {
		cube.rotate(p1MoveStrings[solutions[i]]);
		//cout << p1MoveStrings[solutions[i]] << endl;
	}
	
	//debug statement
	//cube.printInfo();
	
	//ensures cube has completed phase 1 correctly or else phase 2 will run until stopped (impossible to solve)
	if (!cube.phase1Solved()) {
		cerr << "CUBE HAS NOT SOLVED PHASE 1 CORRECTLY" << endl;
		return false;
	}
	
	//debug statement
	//cout << "phase 2" << endl;
	
	//calls phase 2 solver
	phase2SolverWrapper(solutions, cube);
	
	//apply moves to solve cube
	for (size_t i = solution_len; i < solution_len + solution2_len; i++) {
		cube.rotate(p2MoveStrings[solutions[i]]);
		//cout << p2MoveStrings[solutions[i]] << endl;
		//cout << cube.p2elIndex() << " " << cube.p2clIndex() << " " << cube.p2eSliceIndex() << endl;
	}
	
	//ensure the cube is solved
	if (!cube.isSolved()) {
		cube.printInfo();
		cerr << "CUBE WAS NOT SOLVED CORRECTLY" << endl;
		return false;
	}
	
	return true;
}

//phase 1 solver wrapper to run the IDA* search until we solve phase 1
void Solver::phase1SolverWrapper(vector<int>& solutions, Cube& cube)
{
	//initial keys
	uint16_t eoKey = cube.p1eoIndex();
	uint16_t coKey = cube.p1coIndex();
	uint16_t eKey = cube.p1eSliceIndex();
	
	//old heuristic tables
	//int max_cost = max(max(eoHeuristicsTableRef[eoKey], coHeuristicsTableRef[coKey]), eHeuristicsTableRef[eKey]);
	
	//getting estimate
	int max_cost = max(eocoHeuristicsTableRef[eoKey * CO_SIZE + coKey], eHeuristicsTableRef[eKey]);
	
	//call the solver once
	max_cost = phase1Solver(solutions, max_cost, 0, eoKey, coKey, eKey, 100);

	//keep running the solver every time it leaves with the new max encountered unless we get -1 (phase 1 solved)
	while (max_cost != -1) {
		max_cost = phase1Solver(solutions, max_cost, 0, eoKey, coKey, eKey, 100);
	}
}

//phase 1 IDA* solver
int Solver::phase1Solver(vector<int>& solutions, int max_cost, int curr_cost, uint16_t eoKey, uint16_t coKey, uint16_t eKey, size_t prevMove)
{
	//update node expansion
	p1counter++;

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
	
	for (size_t i = 0; i < NUM_P1_MOVES; i++) {
		if (p1MoveGroup[i] != prevMove) {
			//"rotating" the cube via coordinate update (precomputed table in tables.cpp)
			uint16_t tempEOKey = EORef[i];
			uint16_t tempCOKey = CORef[i];
			uint16_t tempEKey = ERef[i];
			
			//recursive call for IDA* with new key values
			int result = phase1Solver(solutions, max_cost, next_cost, tempEOKey, tempCOKey, tempEKey, p1MoveGroup[i]);
			
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
void Solver::phase2SolverWrapper(vector<int>& solutions, Cube& cube)
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
	max_cost = phase2Solver(solutions, max_cost, 0, elKey, clKey, eKey, 100);

	//keep searching until the cube is solved
	while (max_cost != -1) {
		max_cost = phase2Solver(solutions, max_cost, 0, elKey, clKey, eKey, 100);
	}	
}

//phase 2 IDA* solver
int Solver::phase2Solver(vector<int>& solutions, int max_cost, int curr_cost, uint16_t elKey, uint16_t clKey, uint8_t eKey, size_t prevMove)
{
	//update node expansion
	p2counter++;
	
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
	
	for (size_t i = 0; i < NUM_P2_MOVES; i++) {
		if (prevMove != p2MoveGroup[i]) {
			//updating coordinates to "rotate" the cube
			uint16_t tempELKey = elRef[i];
			uint16_t tempCLKey = clRef[i];
			uint8_t tempEKey = ep2Ref[i];
			
			//recursive IDA* search call
			int result = phase2Solver(solutions, max_cost, next_cost, tempELKey, tempCLKey, tempEKey, p2MoveGroup[i]);
			
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
void Solver::initializeSolver()
{
	Cube::initialize();
	Tables::loadTables();
}

