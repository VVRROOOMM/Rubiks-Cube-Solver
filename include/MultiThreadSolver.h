#pragma once

#include "Cube.h"
#include "Tables.h"
#include "DBCube.h"

#include <algorithm>
#include <chrono>
#include <mutex>
#include <queue> 

using namespace std;

//number of moves used to generate tables in each phase
constexpr size_t NUM_P1_MOVES_MULTI = 18;
constexpr size_t NUM_P2_MOVES_MULTI = 10;

//these will be used for pruning during IDA*
const extern size_t p1MoveGroupMulti[NUM_P1_MOVES_MULTI];
const extern size_t p2MoveGroupMulti[NUM_P2_MOVES_MULTI];

class MultiThreadSolver{
	public:
		static void solveWrapperMulti(mutex& m, queue<DBCube>& sharedQueue, int cubes_to_do);
		static bool solveMulti(vector<int>& solutions, Cube& cube, DBCube& cube_data, uint8_t& solution_len, uint8_t& solution2_len, uint64_t& curr_p1counter, uint64_t& curr_p2counter);

		static void phase1SolverWrapper(vector<int>& solutions, Cube& cube, uint8_t& solution_len, uint64_t& curr_p1counter);
		static int phase1Solver(vector<int>& solutions, int max_cost, int curr_cost, uint16_t eoKey, uint16_t coKey, uint16_t eKey, size_t prevMove, uint8_t& solution_len, uint64_t& curr_p1counter);
		
		static void phase2SolverWrapper(vector<int>& solutions, Cube& cube, uint8_t& solution_len, uint8_t& solution2_len, uint64_t& curr_p2counter);
		static int phase2Solver(vector<int>& solutions, int max_cost, int curr_cost, uint16_t elKey, uint16_t clKey, uint8_t eKey, size_t prevMove, uint8_t& solution_len, uint8_t& solution2_len, uint64_t& curr_p2counter);
		
		static void initializeSolver();
};