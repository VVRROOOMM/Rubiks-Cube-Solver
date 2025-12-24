//these tests will handle all functions that will be used for future database integration

#include <gtest/gtest.h>

#include "Cube.h"

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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
