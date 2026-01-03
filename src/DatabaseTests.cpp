//these tests will handle all functions that will be used for future database integration

#include <gtest/gtest.h>

#include "Cube.h"
#include "DBCube.h"
#include "Solver.h"
#include "DatabaseLogger.h"
#include "MultiThreadSolver.h"
#include "Main.h"

#include <unordered_set>
#include <queue>

namespace
{
	//checks cubes are equals and prints out all mismatches of array representations
	bool areEquals(const Cube& cube1, const Cube& cube2)
	{
		const array<uint8_t, NUM_EDGES> eo1 = cube1.getEO();
		const array<uint8_t, NUM_EDGES> eo2 = cube1.getEO();
		
		const array<uint8_t, NUM_EDGES> el1 = cube1.getEL();
		const array<uint8_t, NUM_EDGES> el2 = cube2.getEL();
		
		const array<uint8_t, NUM_CORNERS> co1 = cube1.getCO();
		const array<uint8_t, NUM_CORNERS> co2 = cube2.getCO();
		
		const array<uint8_t, NUM_CORNERS> cl1 = cube1.getCL();
		const array<uint8_t, NUM_CORNERS> cl2 = cube2.getCL();
		
		bool result = true;
		
		for (size_t i = 0; i < NUM_EDGES; i++) {
			if (el1[i] != el2[i]) {
				cerr << "edge " << i << " do not match locations. Cube1: " << static_cast<int>(el1[i]) << " , Cube2: " << static_cast<int>(el2[i]) << endl;
				result = false;
			}
			if (eo1[i] != eo2[i]) {
				cerr << "edge " << i << " do not match orientations. Cube1: " << static_cast<int>(eo1[i]) << " , Cube2: " << static_cast<int>(eo2[i]) << endl;
				result = false;
			}
		}
		
		if (!result) {
			cerr << endl;
		}
		
		for (size_t i = 0; i < NUM_CORNERS; i++) {
			if (cl1[i] != cl2[i]) {
				cerr << "corner " << i << " do not match locations. Cube1: " << static_cast<int>(cl1[i]) << " , Cube2: " << static_cast<int>(cl2[i]) << endl;
				result = false;
			}
			if (co1[i] != co2[i]) {
				cerr << "corner " << i << " do not match orientations. Cube1: " << static_cast<int>(co1[i]) << " , Cube2: " << static_cast<int>(co2[i]) << endl;
				result = false;
			}
		}
		
		return result;
	}
	
	//checks database entries are the same (excluding solution time)
	bool areEquals(const DBCube& cube1, const DBCube& cube2)
	{
		bool result = true;
		
		if (cube1.get_initial_cube() != cube2.get_initial_cube()) {
			result = false;
		}
		else if (cube1.get_solution() != cube2.get_solution()) {
			result = false;
		}
		else if (cube1.get_phase1_nodes() != cube2.get_phase1_nodes()) {
			result = false;
		}
		else if (cube1.get_phase2_nodes() != cube2.get_phase2_nodes()) {
			result =  false;
		}
		
		if (!result) {
			cout << "cube1: " << cube1.get_initial_cube() << " cube2: " << cube2.get_initial_cube() << endl;
			cout << "cube1: " << cube1.get_solution() << " cube2: " << cube2.get_solution() << endl;
			cout << "cube1: " << cube1.get_phase1_nodes() << " cube2: " << cube2.get_phase1_nodes() << endl;
			cout << "cube1: " << cube1.get_phase2_nodes() << " cube2: " << cube2.get_phase2_nodes() << endl;
		}
		
		return result;
	}
}

//cubeToString tests takes a cube, converts it to a string, then reload a cube using the outputted string and ensures the cubes are equal
TEST(cube1ToString, cubeToString)
{
	Cube::initialize();
	
	string initial_cube = "LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR";

	Cube cube(initial_cube);
	
	string result = cube.getStringKey();
	
	Cube cube2(result);
	
	ASSERT_TRUE(areEquals(cube, cube2));
	ASSERT_TRUE(initial_cube == result);
}

TEST(cube2ToString, cubeToString)
{
	Cube::initialize();
	
	string initial_cube = "DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURDRUUF";

	Cube cube(initial_cube);
	
	string result = cube.getStringKey();
	
	Cube cube2(result);
	
	ASSERT_TRUE(areEquals(cube, cube2));
	ASSERT_TRUE(initial_cube == result);
}

TEST(cube3ToString, cubeToString)
{
	Cube::initialize();
	
	string initial_cube = "FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU";

	Cube cube(initial_cube);
	
	string result = cube.getStringKey();
	
	Cube cube2(result);
	
	ASSERT_TRUE(areEquals(cube, cube2));
	ASSERT_TRUE(initial_cube == result);
}

TEST(cube4ToString, cubeToString)
{
	Cube::initialize();
	
	string initial_cube = "RBFLUBRRRBBUDLUUFBFFFFFDUUDDRURRFLLBLUDDBBLDRLRFLDUBLD";

	Cube cube(initial_cube);
	
	string result = cube.getStringKey();
	
	Cube cube2(result);
	
	ASSERT_TRUE(areEquals(cube, cube2));
	ASSERT_TRUE(initial_cube == result);
}

TEST(cube5ToString, cubeToString)
{
	Cube::initialize();
	
	string initial_cube = "BUFFUFLLDRDDULUDDRFDRFFBBLUFRUURBRDLLLURBRBLLDBFRDBBFU";

	Cube cube(initial_cube);
	
	string result = cube.getStringKey();
	
	Cube cube2(result);
	
	ASSERT_TRUE(areEquals(cube, cube2));
	ASSERT_TRUE(initial_cube == result);
}

//more extensive testing for cubeToString by checking 500k cubes
//this took my computer like 40 seconds to do, but passed, uncomment if you want
/*TEST(bfsToString, cubeToString)
{
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	int depth = 0;
	int max_depth = 6;
	
	queue<Cube> toDo;
	toDo.push(cube);
	
	uint64_t key = cube.getZobrist();
	
	unordered_set<uint64_t> seen;
	seen.insert(key);
	
	while (depth < max_depth) {
		depth++;
		int n = toDo.size();
		cout << depth << " todo.size() = " << n << endl;
		for (int i = 0; i < n; i++) {
			cube = toDo.front();
			toDo.pop();
			for (size_t move = 0; move < NUM_MOVES; move++) {
				Cube temp = cube;
				temp.rotate(move);
				key = temp.getZobrist();
				if (!seen.contains(key) && depth != max_depth) {
					seen.insert(key);
					toDo.push(temp);
				}
				Cube temp2(temp.getStringKey());
				ASSERT_TRUE(areEquals(temp, temp2));
			}
		}
	}
}*/

TEST(sqlite3_single_thread_solving_loading, database_tests)
{
	Solver::initializeSolver();

	vector<DBCube> cubes_to_solve;
	int num_solve = 1000;
	int initial_count = 0;
	int end_count = 0;
	
	{
		DatabaseLogger logger("solve_logs/solves.db", -1);
		//get the number of cubes with this version -1 for testing
		initial_count = logger.sqlite3_count_by_version(-1);
	}
	
	Main::singleThreadLog(num_solve, false, -1, false);
	
	DatabaseLogger logger("solve_logs/solves.db", -1);
	//this end_count should be exactly num_solve greater then initial_count
	end_count = logger.sqlite3_count_by_version(-1);
	
	ASSERT_EQ(initial_count + num_solve, end_count);
	
	//reload the DBcubes to a new vector, these cubes we will load in the initial state, solve them with the solution and check they are solved
	logger.sqlite3_load_version(cubes_to_solve);
	
	for (DBCube& temp : cubes_to_solve) {
		//rebuild cube
		Cube cube(temp.get_initial_cube());
		
		vector<string> solution = temp.solutionSplitter();
				
		for (const string& s : solution) {
			cube.rotate(s);
		}
		
		ASSERT_TRUE(cube.isSolved());
	}
	
	//this deletes all entries with version -1 (this test)
	logger.sqlite3_delete_by_version(-1);
	
	//if you uncomment the delete and want to keep the data remove this test
	ASSERT_EQ(logger.sqlite3_count_by_version(-1), 0);
}

//this test ensures that multi threaded code results in the same data generation as single threaded code
//then ensures the number of logged cubes increased by the right amount
TEST(sqlite3_multi_thread_solving_loading, database_tests)
{
	MultiThreadSolver::initializeSolver();
	

	vector<DBCube> cubes_to_solve;
	//if you do 10,000, we solve 190,000 cubes which is relatively fast, but the actual resolving single threaded took 40 minutes
	//1000 cubes at each stage solved: 19,000 cubes took 3 minutes
	//int num_solve = 10000;
	int num_solve = 100;
	int initial_count = 0;
	int end_count = 0;
	int lim = 19;
	int counter = 0;
	
	DBCube single_thread_data;
	Cube cube;
	
	{
		DatabaseLogger logger("solve_logs/solves.db", -1);
		//get the number of cubes with this version -1 for testing
		initial_count = logger.sqlite3_count_by_version(-1);
	}
		
	for (int i = 1; i <= lim; i++) {
		Main::multiThreadLogger(num_solve, false, -1, i);
	}
	
	DatabaseLogger logger("solve_logs/solves.db", -1);
	//this end_count should be exactly num_solve greater then initial_count
	end_count = logger.sqlite3_count_by_version(-1);
	
	ASSERT_EQ(initial_count + num_solve * lim, end_count);
	
	//reload the DBcubes to a new vector, these cubes we will load in the initial state, solve them with the solution and check they are solved
	logger.sqlite3_load_version(cubes_to_solve);
	
	for (DBCube& temp : cubes_to_solve) {
		counter++;
		if ((counter % 1000) == 0) {
			cout << counter << endl;
		}
		//rebuild cube
		cube.stringToCube(temp.get_initial_cube());
		
		vector<string> solution = temp.solutionSplitter();
				
		for (const string& s : solution) {
			cube.rotate(s);
		}
		
		ASSERT_TRUE(cube.isSolved());
		
		cube.stringToCube(temp.get_initial_cube());
		
		Solver::solveDBCubeTest(cube, single_thread_data);
		
		ASSERT_TRUE(areEquals(single_thread_data, temp));
	}
	
	//this deletes all entries with version -1 (this test)
	logger.sqlite3_delete_by_version(-1);
	
	//if you uncomment the delete and want to keep the data remove this test
	ASSERT_EQ(logger.sqlite3_count_by_version(-1), 0);
	
	cout << "done test" << endl;
}

int main(int argc, char **argv) {	
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
