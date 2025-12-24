//tests coordinate representation of a cube and transition tables

#include <gtest/gtest.h>

#include "Cube.h"
#include "Tables.h"

using namespace std;

//the following 9 tests are random cubes and ensure the indexer returns the manually calculated keys
TEST(eo_cube1, eoIndex) 
{
	Cube::initialize();
	
	Cube cube("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	
	uint16_t key = 0b00110100001;
		
	ASSERT_EQ(cube.p1eoIndex(), key);
}

TEST(co_cube1, coIndex) 
{
	Cube::initialize();
	
	Cube cube("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	
	uint16_t key = 2087;
	
	ASSERT_EQ(cube.p1coIndex(), key);
}

TEST(eSlice_cube1, eSliceIndex) 
{
	Cube::initialize();
	Cube cube("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	
	uint16_t key = 463;
	
	ASSERT_EQ(cube.p1eSliceIndex(), key);
}

TEST(eo_cube2, eoIndex) 
{
	Cube::initialize();
	
	Cube cube("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	
	uint16_t key = 0b11011100110;
	
	ASSERT_EQ(cube.p1eoIndex(), key);
}

TEST(co_cube2, coIndex) 
{
	Cube::initialize();
	
	Cube cube("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	
	uint16_t key = 650;
	
	ASSERT_EQ(cube.p1coIndex(), key);
}

TEST(eSlice_cube2, eSliceIndex) 
{
	Cube::initialize();
	Cube cube("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	
	uint16_t key = 367;
	
	ASSERT_EQ(cube.p1eSliceIndex(), key);
}

TEST(eo_cube3, eoIndex) 
{
	Cube::initialize();
	
	Cube cube("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	
	uint16_t key = 0b00110010000;
	
	ASSERT_EQ(cube.p1eoIndex(), key);
}

TEST(co_cube3, coIndex) 
{
	Cube::initialize();

	Cube cube("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	
	uint16_t key = 1971;
	
	ASSERT_EQ(cube.p1coIndex(), key);
}

TEST(eSlice_cube3, eSliceIndex) 
{
	Cube::initialize();
	Cube cube("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	
	uint16_t key = 225;
	
	ASSERT_EQ(cube.p1eSliceIndex(), key);
}

//the 5 tests check edge orientation indexers, but are the exact same for co, eSlicep1, el, cl and eSlicep2
//checks that a cube updated to a key gives the inputed key
TEST(eo_roundTrip, eoIndex)
{
	Cube::initialize();
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (uint16_t i = 0; i < 2048; i++) {
		cube.p1eoReverse(i);
		ASSERT_EQ(cube.p1eoIndex(), i);
	}
}

//ensures no transitions out of bounds for the possible key values
TEST(eoTransitionBounds, eoTransitions)
{
	Cube::initialize();

	Tables::loadTables();
	
	for (size_t i = 0; i < NUM_MOVES; i++) {
		for (int j = 0; j < EO_SIZE; j++) {
			ASSERT_TRUE(eoTransitionTableRef[j][i] >= 0 && eoTransitionTableRef[j][i] < EO_SIZE);
		}
	}
}

//ensures the transition table accurately represents the cube
TEST(eoTransitionRoundTrip, eoTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (int i = 0; i < EO_SIZE; i++) {		
		for (size_t j = 0; j < NUM_MOVES; j++) {
			cube.p1eoReverse(i);
			
			cube.rotate(j);
			ASSERT_EQ(cube.p1eoIndex(), eoTransitionTableRef[i][j]);
		}
	}
}

//ensures transition tables lead back to itself after doing inverse moves
TEST(eoTransitionInverses, eoTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<string> moves = {"U", "U'", "U2", "L", "L'", "L2", "F", "F'", "F2", "R", "R'", "R2", "B", "B'", "B2", "D", "D'", "D2"};
	vector<string> inverse_moves = {"U'", "U", "U2", "L'", "L", "L2", "F'", "F", "F2", "R'", "R", "R2", "B'", "B", "B2", "D'", "D", "D2"};
	
	for (int i = 0; i < EO_SIZE; i++) {		
		for (size_t j = 0; j < NUM_MOVES; j++) {
			uint16_t index = eoTransitionTableRef[i][Cube::stringToMoves(moves[j])];
			index = eoTransitionTableRef[index][Cube::stringToMoves(inverse_moves[j])];
			
			ASSERT_EQ(i, index);
			
			index = eoTransitionTableRef[index][Cube::stringToMoves(inverse_moves[j])];
			index = eoTransitionTableRef[index][Cube::stringToMoves(moves[j])];
			
			ASSERT_EQ(i, index);
		}
	}
}

//ensures that applying the move a set number of times yields the same cube/coordinate
TEST(eoTransitionCycles, eoTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<string> moves = {"U", "U'", "U2", "L", "L'", "L2", "F", "F'", "F2", "R", "R'", "R2", "B", "B'", "B2", "D", "D'", "D2"};
	vector<int> cycles = {4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 2};
	
	for (int i = 0; i < EO_SIZE; i++) {		
		for (size_t j = 0; j < NUM_MOVES; j++) {
			int index = i;
			for (int k = 0; k < cycles[j]; k++) {
				index = eoTransitionTableRef[index][Cube::stringToMoves(moves[j])];
			}
			
			ASSERT_EQ(i, index);
		}
	}
}


TEST(co_roundTrip, coIndex)
{
	Cube::initialize();

	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (uint16_t i = 0; i < 2187; i++) {
		cube.p1coReverse(i);
		ASSERT_EQ(cube.p1coIndex(), i);
	}
}

TEST(coTransitionsBounds, coTransitions)
{
	Tables::loadTables();
	
	for (uint16_t i = 0; i < CO_SIZE; i++) {
		for (size_t j = 0; j < NUM_MOVES; j++) {
			ASSERT_TRUE(coTransitionTableRef[i][j] >= 0 && coTransitionTableRef[i][j] < CO_SIZE);
		}
	}
}

TEST(coTransitionRoundTrip, coTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (int i = 0; i < CO_SIZE; i++) {		
		for (size_t j = 0; j < NUM_MOVES; j++) {
			cube.p1coReverse(i);
			
			cube.rotate(j);
			ASSERT_EQ(cube.p1coIndex(), coTransitionTableRef[i][j]);
		}
	}
}

TEST(coTransitionInverses, coTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<string> moves = {"U", "U'", "U2", "L", "L'", "L2", "F", "F'", "F2", "R", "R'", "R2", "B", "B'", "B2", "D", "D'", "D2"};
	vector<string> inverse_moves = {"U'", "U", "U2", "L'", "L", "L2", "F'", "F", "F2", "R'", "R", "R2", "B'", "B", "B2", "D'", "D", "D2"};
	
	for (int i = 0; i < CO_SIZE; i++) {		
		for (size_t j = 0; j < NUM_MOVES; j++) {
			uint16_t index = coTransitionTableRef[i][Cube::stringToMoves(moves[j])];
			index = coTransitionTableRef[index][Cube::stringToMoves(inverse_moves[j])];

			ASSERT_EQ(i, index);
			index = coTransitionTableRef[index][Cube::stringToMoves(inverse_moves[j])];
			index = coTransitionTableRef[index][Cube::stringToMoves(moves[j])];
			
			ASSERT_EQ(i, index);
		}
	}
}

TEST(coTransitionCycles, coTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<string> moves = {"U", "U'", "U2", "L", "L'", "L2", "F", "F'", "F2", "R", "R'", "R2", "B", "B'", "B2", "D", "D'", "D2"};
	vector<int> cycles = {4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 2};
	
	for (int i = 0; i < CO_SIZE; i++) {		
		for (size_t j = 0; j < NUM_MOVES; j++) {
			int index = i;
			for (int k = 0; k < cycles[j]; k++) {
				index = coTransitionTableRef[index][Cube::stringToMoves(moves[j])];
			}
			
			ASSERT_EQ(i, index);
		}
	}
}


TEST(eSlice_roundTrip, eIndex)
{
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	Cube::initialize();
	
	for (uint16_t i = 0; i < 495; i++) {
		Tables::emptyCube(cube);
		cube.p1eSliceReverse(i);
		ASSERT_EQ(cube.p1eSliceIndex(), i);
	}
}

TEST(eSliceTransitionsBounds, eSliceTransitions)
{
	Tables::loadTables();
	
	for (uint16_t i = 0; i < E_SIZE; i++) {
		for (size_t j = 0; j < NUM_MOVES; j++) {
			ASSERT_TRUE(eTransitionTableRef[i][j] >= 0 && eTransitionTableRef[i][j] < E_SIZE);
		}
	}
}

TEST(eSliceTransitionRoundTrip, eSliceTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (int i = 0; i < E_SIZE; i++) {
		for (size_t j = 0; j < NUM_MOVES; j++) {
			Tables::emptyCube(cube);
			cube.p1eSliceReverse(i);
			
			cube.rotate(j);
			ASSERT_EQ(cube.p1eSliceIndex(), eTransitionTableRef[i][j]);
		}
	}
}

TEST(eSliceTransitionInverses, eSliceTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<string> moves = {"U", "U'", "U2", "L", "L'", "L2", "F", "F'", "F2", "R", "R'", "R2", "B", "B'", "B2", "D", "D'", "D2"};
	vector<string> inverse_moves = {"U'", "U", "U2", "L'", "L", "L2", "F'", "F", "F2", "R'", "R", "R2", "B'", "B", "B2", "D'", "D", "D2"};
	
	for (int i = 0; i < E_SIZE; i++) {		
		for (size_t j = 0; j < NUM_MOVES; j++) {
			uint16_t index = eTransitionTableRef[i][Cube::stringToMoves(moves[j])];
			index = eTransitionTableRef[index][Cube::stringToMoves(inverse_moves[j])];

			ASSERT_EQ(i, index);
			index = eTransitionTableRef[index][Cube::stringToMoves(inverse_moves[j])];
			index = eTransitionTableRef[index][Cube::stringToMoves(moves[j])];
			
			ASSERT_EQ(i, index);
		}
	}
}

TEST(eSliceTransitionCycles, eSliceTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<string> moves = {"U", "U'", "U2", "L", "L'", "L2", "F", "F'", "F2", "R", "R'", "R2", "B", "B'", "B2", "D", "D'", "D2"};
	vector<int> cycles = {4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 2};
	
	for (int i = 0; i < E_SIZE; i++) {		
		for (size_t j = 0; j < NUM_MOVES; j++) {
			int index = i;
			for (int k = 0; k < cycles[j]; k++) {
				index = eTransitionTableRef[index][Cube::stringToMoves(moves[j])];
			}
			
			ASSERT_EQ(i, index);
		}
	}
}

//the following 9 tests take random cubes and ensures indexers are correct
TEST(el_cube1, elIndex)
{
	Cube::initialize();
	
	Cube cube("DUUUUDUDULFBLLRRRBLFLBFFLBFFLFRRLRLRRRFBBFBBBDDDDDUUUD");
	
	ASSERT_EQ(cube.p2elIndex(), 24620);
}

TEST(cl_cube1, clIndex)
{
	Cube::initialize();
	
	Cube cube("DUUUUDUDULFBLLRRRBLFLBFFLBFFLFRRLRLRRRFBBFBBBDDDDDUUUD");
	
	ASSERT_EQ(cube.p2clIndex(), 15165);
}

TEST(eSlice_p2_cube1, eSliceIndex)
{
	Cube::initialize();
	
	Cube cube("DUUUUDUDULFBLLRRRBLFLBFFLBFFLFRRLRLRRRFBBFBBBDDDDDUUUD");
	
	ASSERT_EQ(cube.p2eSliceIndex(), 15);
}

TEST(el_cube2, elIndex)
{
	Cube::initialize();
	
	Cube cube("DUUDUDDUURRFRLLRFFLBBBFFLLBLBFLRRLLFRRBFBBRFBUUDUDDUDD");
	
	ASSERT_EQ(cube.p2elIndex(), 13883);
}

TEST(cl_cube2, clIndex)
{
	Cube::initialize();
	
	Cube cube("DUUDUDDUURRFRLLRFFLBBBFFLLBLBFLRRLLFRRBFBBRFBUUDUDDUDD");
	
	ASSERT_EQ(cube.p2clIndex(), 22517);
}

TEST(eSlice_p2_cube2, eSliceIndex)
{
	Cube::initialize();
	
	Cube cube("DUUDUDDUURRFRLLRFFLBBBFFLLBLBFLRRLLFRRBFBBRFBUUDUDDUDD");
	
	ASSERT_EQ(cube.p2eSliceIndex(), 18);
}

TEST(el_cube3, elIndex)
{
	Cube::initialize();
	
	Cube cube("DUDUUDUUURLBLLRLBBLFRFFBLRFBLFRRLRRRLBBBBFFFFDUDDDDUDU");
	
	ASSERT_EQ(cube.p2elIndex(), 4466);
}

TEST(cl_cube3, clIndex)
{
	Cube::initialize();
	
	Cube cube("DUDUUDUUURLBLLRLBBLFRFFBLRFBLFRRLRRRLBBBBFFFFDUDDDDUDU");
	
	ASSERT_EQ(cube.p2clIndex(), 16895);
}

TEST(eSlice_p2_cube3, eSliceIndex)
{
	Cube::initialize();
	
	Cube cube("DUDUUDUUURLBLLRLBBLFRFFBLRFBLFRRLRRRLBBBBFFFFDUDDDDUDU");
	
	ASSERT_EQ(cube.p2eSliceIndex(), 9);
}

//as specified above do the exact same thing as eo transition tests
TEST(el_roundTrip, elIndex)
{
	Cube::initialize();
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (uint16_t i = 0; i < 40320; i++) {
		cube.p2elReverse(i);
		ASSERT_EQ(cube.p2elIndex(), i);
	}
}

TEST(elTransitionBounds, elTransitions)
{
	Tables::loadTables();
	
	for (size_t i = 0; i < NUM_MOVES_P2; i++) {
		for (int j = 0; j < EL_SIZE; j++) {
			ASSERT_TRUE(elTransitionTableRef[j][i] >= 0 && elTransitionTableRef[j][i] < EL_SIZE);
		}
	}
}

TEST(elTransitionRoundTrip, elTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	vector<string> moves = {"U", "U'", "U2", "L2", "F2", "R2", "B2", "D", "D'", "D2"};
	
	for (int i = 0; i < EL_SIZE; i++) {		
		for (size_t j = 0; j < moves.size(); j++) {
			cube.p2elReverse(i);
			
			cube.rotate(moves[j]);
			ASSERT_EQ(cube.p2elIndex(), elTransitionTableRef[i][j]);
		}
	}
}

TEST(elTransitionInverses, elTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<int> moves = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	vector<int> inverse_moves = {1, 0, 2, 3, 4, 5, 6, 8, 7, 9};
	
	for (int i = 0; i < EL_SIZE; i++) {		
		for (size_t j = 0; j < NUM_MOVES_P2; j++) {
			uint16_t index = elTransitionTableRef[i][moves[j]];
			index = elTransitionTableRef[index][inverse_moves[j]];
			ASSERT_EQ(i, index);
			
			index = elTransitionTableRef[index][inverse_moves[j]];
			index = elTransitionTableRef[index][moves[j]];
			ASSERT_EQ(i, index);
		}
	}
}

TEST(elTransitionCycles, elTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<string> moves = {"U", "U'", "U2", "L2", "F2", "R2", "B2", "D", "D'", "D2"};
	vector<int> cycles = {4, 4, 2, 2, 2, 2, 2, 4, 4, 2};
	
	for (int i = 0; i < EL_SIZE; i++) {		
		for (size_t j = 0; j < moves.size(); j++) {
			int index = i;
			for (int k = 0; k < cycles[j]; k++) {
				index = elTransitionTableRef[index][j];
			}
			
			ASSERT_EQ(i, index);
		}
	}
}

TEST(cl_roundTrip, clIndex)
{
	Cube::initialize();
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (uint16_t i = 0; i < 40320; i++) {
		cube.p2clReverse(i);
		ASSERT_EQ(cube.p2clIndex(), i);
	}
}

TEST(clTransitionBounds, clTransitions)
{
	Tables::loadTables();
	
	for (size_t i = 0; i < NUM_MOVES_P2; i++) {
		for (int j = 0; j < CL_SIZE; j++) {
			ASSERT_TRUE(clTransitionTableRef[j][i] >= 0 && clTransitionTableRef[j][i] < CL_SIZE);
		}
	}
}

TEST(clTransitionRoundTrip, clTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	vector<string> moves = {"U", "U'", "U2", "L2", "F2", "R2", "B2", "D", "D'", "D2"};
	
	for (int i = 0; i < CL_SIZE; i++) {		
		for (size_t j = 0; j < moves.size(); j++) {
			cube.p2clReverse(i);
			
			cube.rotate(moves[j]);
			ASSERT_EQ(cube.p2clIndex(), clTransitionTableRef[i][j]);
		}
	}
}

TEST(clTransitionInverses, clTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<int> moves = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	vector<int> inverse_moves = {1, 0, 2, 3, 4, 5, 6, 8, 7, 9};
	
	for (int i = 0; i < CL_SIZE; i++) {		
		for (size_t j = 0; j < NUM_MOVES_P2; j++) {
			uint16_t index = clTransitionTableRef[i][moves[j]];
			index = clTransitionTableRef[index][inverse_moves[j]];
			ASSERT_EQ(i, index);
			
			index = clTransitionTableRef[index][inverse_moves[j]];
			index = clTransitionTableRef[index][moves[j]];
			ASSERT_EQ(i, index);
		}
	}
}

TEST(clTransitionCycles, clTransitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<string> moves = {"U", "U'", "U2", "L2", "F2", "R2", "B2", "D", "D'", "D2"};
	vector<int> cycles = {4, 4, 2, 2, 2, 2, 2, 4, 4, 2};
	
	for (int i = 0; i < CL_SIZE; i++) {		
		for (size_t j = 0; j < moves.size(); j++) {
			int index = i;
			for (int k = 0; k < cycles[j]; k++) {
				index = clTransitionTableRef[index][j];
			}
			
			ASSERT_EQ(i, index);
		}
	}
}

TEST(eSlice_p2_roundTrip, eSlice_p2Index)
{
	Cube::initialize();
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (uint16_t i = 0; i < E_SIZE_P2; i++) {
		cube.p2eSliceReverse(i);
		ASSERT_EQ(cube.p2eSliceIndex(), i);
	}
}

TEST(eSlice_p2TransitionBounds, eSlice_p2Transitions)
{
	Tables::loadTables();
	
	for (size_t i = 0; i < NUM_MOVES_P2; i++) {
		for (int j = 0; j < E_SIZE_P2; j++) {
			ASSERT_TRUE(ep2TransitionTableRef[j][i] >= 0 && ep2TransitionTableRef[j][i] < E_SIZE_P2);
		}
	}
}

TEST(eSlice_p2TransitionRoundTrip, eSlice_p2Transitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	vector<string> moves = {"U", "U'", "U2", "L2", "F2", "R2", "B2", "D", "D'", "D2"};
	
	for (int i = 0; i < E_SIZE_P2; i++) {		
		for (size_t j = 0; j < moves.size(); j++) {
			cube.p2eSliceReverse(i);
			
			cube.rotate(moves[j]);
			ASSERT_EQ(cube.p2eSliceIndex(), ep2TransitionTableRef[i][j]);
		}
	}
}

TEST(eSlice_p2TransitionInverses, eSlice_p2Transitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<int> moves = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	vector<int> inverse_moves = {1, 0, 2, 3, 4, 5, 6, 8, 7, 9};
	
	for (int i = 0; i < E_SIZE_P2; i++) {		
		for (size_t j = 0; j < NUM_MOVES_P2; j++) {
			uint16_t index = ep2TransitionTableRef[i][moves[j]];
			index = ep2TransitionTableRef[index][inverse_moves[j]];
			ASSERT_EQ(i, index);
			
			index = ep2TransitionTableRef[index][inverse_moves[j]];
			index = ep2TransitionTableRef[index][moves[j]];
			ASSERT_EQ(i, index);
		}
	}
}

TEST(eSlice_p2TransitionCycles, eSlice_p2Transitions)
{
	Tables::loadTables();
	Cube::initialize();
	
	vector<string> moves = {"U", "U'", "U2", "L2", "F2", "R2", "B2", "D", "D'", "D2"};
	vector<int> cycles = {0, 0, 0, 2, 2, 2, 2, 0, 0, 0};
	
	for (int i = 0; i < E_SIZE_P2; i++) {		
		for (size_t j = 0; j < moves.size(); j++) {
			int index = i;
			for (int k = 0; k < cycles[j]; k++) {
				index = ep2TransitionTableRef[index][j];
			}
			
			ASSERT_EQ(i, index);
		}
	}
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}