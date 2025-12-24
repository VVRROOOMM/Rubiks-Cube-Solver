//ensures rotations are correct for cubes

#include <gtest/gtest.h>

#include "Cube.h"

#include <unordered_set>

using namespace std;

namespace
{
	//checks if 2 cubes have the exact same array representation, returns false and prints all mismatches
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

//the following 4 enum_testing tests ensures they return correct values
TEST(face_enum, enum_testing)
{
	ASSERT_EQ(Cube::charToFace('U'), 0);
	ASSERT_EQ(Cube::charToFace('L'), 1);
	ASSERT_EQ(Cube::charToFace('F'), 2);
	ASSERT_EQ(Cube::charToFace('R'), 3);
	ASSERT_EQ(Cube::charToFace('B'), 4);
	ASSERT_EQ(Cube::charToFace('D'), 6);
	
	ASSERT_THROW(Cube::charToFace('u'), invalid_argument);
	ASSERT_THROW(Cube::charToFace('l'), invalid_argument);
	ASSERT_THROW(Cube::charToFace('f'), invalid_argument);
	ASSERT_THROW(Cube::charToFace('r'), invalid_argument);
	ASSERT_THROW(Cube::charToFace('b'), invalid_argument);
	ASSERT_THROW(Cube::charToFace('d'), invalid_argument);
	
	ASSERT_THROW(Cube::charToFace('!'), invalid_argument);
	ASSERT_THROW(Cube::charToFace(' '), invalid_argument);
}

TEST(edge_enum, enum_testing)
{
	ASSERT_EQ(Cube::stringToEdge("UF"), 0);
	ASSERT_EQ(Cube::stringToEdge("UR"), 1);
	ASSERT_EQ(Cube::stringToEdge("UB"), 2);
	ASSERT_EQ(Cube::stringToEdge("UL"), 3);
	
	ASSERT_EQ(Cube::stringToEdge("FL"), 4);
	ASSERT_EQ(Cube::stringToEdge("FR"), 5);
	ASSERT_EQ(Cube::stringToEdge("BR"), 6);
	ASSERT_EQ(Cube::stringToEdge("BL"), 7);
	
	ASSERT_EQ(Cube::stringToEdge("DF"), 8);
	ASSERT_EQ(Cube::stringToEdge("DR"), 9);
	ASSERT_EQ(Cube::stringToEdge("DB"), 10);
	ASSERT_EQ(Cube::stringToEdge("DL"), 11);

	ASSERT_THROW(Cube::stringToEdge("Fl"), invalid_argument);
	ASSERT_THROW(Cube::stringToEdge(" Fl"), invalid_argument);
	ASSERT_THROW(Cube::stringToEdge("Fl "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToEdge("Fl"), invalid_argument);
	ASSERT_THROW(Cube::stringToEdge(" Fl"), invalid_argument);
	ASSERT_THROW(Cube::stringToEdge("fl "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToEdge("fL"), invalid_argument);
	ASSERT_THROW(Cube::stringToEdge(" fL"), invalid_argument);
	ASSERT_THROW(Cube::stringToEdge("fL "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToEdge(""), invalid_argument);
	ASSERT_THROW(Cube::stringToEdge("ASTTWTAWET"), invalid_argument);
	ASSERT_THROW(Cube::stringToEdge("!@%*@)%@%&!@{}[]"), invalid_argument);
}

TEST(corner_enum, enum_testing)
{
	ASSERT_EQ(Cube::stringToCorner("UFL"), 0);
	ASSERT_EQ(Cube::stringToCorner("UFR"), 1);
	ASSERT_EQ(Cube::stringToCorner("UBR"), 2);
	ASSERT_EQ(Cube::stringToCorner("UBL"), 3);
	
	ASSERT_EQ(Cube::stringToCorner("DFL"), 4);
	ASSERT_EQ(Cube::stringToCorner("DFR"), 5);
	ASSERT_EQ(Cube::stringToCorner("DBR"), 6);
	ASSERT_EQ(Cube::stringToCorner("DBL"), 7);		

	ASSERT_THROW(Cube::stringToCorner("dFL"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner(" dFL"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner("dFL "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToCorner("DfL"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner(" DfL"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner("DfL "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToCorner("dfL"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner(" dfL"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner("dfL "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToCorner("DFl"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner(" DFl"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner("DFl "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToCorner("dFl"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner(" dFl"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner("dFl "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToCorner("Dfl"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner(" Dfl"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner("Dfl "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToCorner("dfl"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner(" dfl"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner("dfl "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToCorner(""), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner("asdtwaWET"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner("hello world"), invalid_argument);
	ASSERT_THROW(Cube::stringToCorner("!@!@\\!@%(!@[]@!%*"), invalid_argument);
}

TEST(move_enum, enum_testing)
{
	ASSERT_EQ(Cube::stringToMoves("U"), 0);
	ASSERT_EQ(Cube::stringToMoves("U'"), 1);
	ASSERT_EQ(Cube::stringToMoves("U2"), 2);
	
	ASSERT_EQ(Cube::stringToMoves("L"), 3);
	ASSERT_EQ(Cube::stringToMoves("L'"), 4);
	ASSERT_EQ(Cube::stringToMoves("L2"), 5);
	
	ASSERT_EQ(Cube::stringToMoves("F"), 6);
	ASSERT_EQ(Cube::stringToMoves("F'"), 7);
	ASSERT_EQ(Cube::stringToMoves("F2"), 8);
	
	ASSERT_EQ(Cube::stringToMoves("R"), 9);
	ASSERT_EQ(Cube::stringToMoves("R'"), 10);
	ASSERT_EQ(Cube::stringToMoves("R2"), 11);
	
	ASSERT_EQ(Cube::stringToMoves("B"), 12);
	ASSERT_EQ(Cube::stringToMoves("B'"), 13);
	ASSERT_EQ(Cube::stringToMoves("B2"), 14);
	
	ASSERT_EQ(Cube::stringToMoves("D"), 15);
	ASSERT_EQ(Cube::stringToMoves("D'"), 16);
	ASSERT_EQ(Cube::stringToMoves("D2"), 17);
	
	ASSERT_THROW(Cube::stringToMoves("r"), invalid_argument);
	ASSERT_THROW(Cube::stringToMoves(" r"), invalid_argument);
	ASSERT_THROW(Cube::stringToMoves("r "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToMoves(" R"), invalid_argument);
	ASSERT_THROW(Cube::stringToMoves("R "), invalid_argument);
	
	ASSERT_THROW(Cube::stringToMoves(""), invalid_argument);
	ASSERT_THROW(Cube::stringToMoves("!@%)!#&)!%@[]"), invalid_argument);
	ASSERT_THROW(Cube::stringToMoves("awoehtaSTWETH"), invalid_argument);
}

/*
the following tests are ordered:
cube_loading: ensures cubes are loaded in correctly
cube_[move]_rotations: ensures that rotations accurately 
for cube1, all these have been commented but the structure is the same
*/
TEST(cube_loading1, cube_loading)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube2("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	//setting the values of the eo, el, co and cl arrays
	eo[0] = 0;
	eo[1] = 0;
	eo[2] = 1;
	eo[3] = 1;
	eo[4] = 0;
	eo[5] = 1;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 1;
	eo[11] = 0;
	
	el[0] = 1;
	el[1] = 9;
	el[2] = 8;
	el[3] = 6;
	el[4] = 11;
	el[5] = 5;
	el[6] = 0;
	el[7] = 3;
	el[8] = 10;
	el[9] = 2;
	el[10] = 4;
	el[11] = 7;
	
	co[0] = 2;
	co[1] = 2;
	co[2] = 1;
	co[3] = 2;
	co[4] = 0;
	co[5] = 2;
	co[6] = 2;
	co[7] = 1;
	
	cl[0] = 7;
	cl[1] = 3;
	cl[2] = 1;
	cl[3] = 0;
	cl[4] = 4;
	cl[5] = 6;
	cl[6] = 2;
	cl[7] = 5;
	
	//setting array values to cube2
	cube2.setEO(eo);
	cube2.setEL(el);
	cube2.setCO(co);
	cube2.setCL(cl);
	
	//ensuring the cubes are equal
	ASSERT_TRUE(areEquals(cube1, cube2));
}

TEST(cube_u_rotations1, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube2("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube3("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube4("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube5("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	
	//setting values for the cube after U rotation
	eo[0] = 0;
	eo[1] = 1;
	eo[2] = 1;
	eo[3] = 0;
	eo[4] = 0;
	eo[5] = 1;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 1;
	eo[11] = 0;
	
	el[0] = 9;
	el[1] = 8;
	el[2] = 6;
	el[3] = 1;
	el[4] = 11;
	el[5] = 5;
	el[6] = 0;
	el[7] = 3;
	el[8] = 10;
	el[9] = 2;
	el[10] = 4;
	el[11] = 7;
	
	co[0] = 2;
	co[1] = 1;
	co[2] = 2;
	co[3] = 2;
	co[4] = 0;
	co[5] = 2;
	co[6] = 2;
	co[7] = 1;
	
	cl[0] = 3;
	cl[1] = 1;
	cl[2] = 0;
	cl[3] = 7;
	cl[4] = 4;
	cl[5] = 6;
	cl[6] = 2;
	cl[7] = 5;
	
	//set the values
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	//rotate cube2
	cube2.rotate("U");
	
	//ensure they are equal
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	//set values after U'
	eo[0] = 1;
	eo[1] = 0;
	eo[2] = 0;
	eo[3] = 1;
	
	el[0] = 6;
	el[1] = 1;
	el[2] = 9;
	el[3] = 8;
	
	co[0] = 2;
	co[1] = 2;
	co[2] = 2;
	co[3] = 1;
	
	cl[0] = 0;
	cl[1] = 7;
	cl[2] = 3;
	cl[3] = 1;
	
	//set values
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	//rotate U' on a cube
	cube3.rotate("U'");
	
	//ensure they are equal
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	//set values after U2
	eo[0] = 1;
	eo[1] = 1;
	eo[2] = 0;
	eo[3] = 0;
	
	el[0] = 8;
	el[1] = 6;
	el[2] = 1;
	el[3] = 9;
	
	co[0] = 1;
	co[1] = 2;
	co[2] = 2;
	co[3] = 2;
	
	cl[0] = 1;
	cl[1] = 0;
	cl[2] = 7;
	cl[3] = 3;
	
	//updating the cube
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	//rotate U2 on cube 4
	cube4.rotate("U2");
	
	//ensure they are equl
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	//checks after applying U that U' gives the original cube
	cube2.rotate("U'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	//checks after applying U' that U gives the original cube
	cube3.rotate("U");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	//checks that after applying U2, U2 gives the original cube
	cube4.rotate("U2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	//ensures that U U and U' U' give the same cube as U2
	cube2.rotate("U");
	cube2.rotate("U");
	
	cube3.rotate("U'");
	cube3.rotate("U'");
	
	cube4.rotate("U2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_l_rotations1, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube2("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube3("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube4("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube5("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	
	eo[0] = 0;
	eo[1] = 0;
	eo[2] = 1;
	eo[3] = 0;
	eo[4] = 1;
	eo[5] = 1;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 1;
	eo[11] = 0;
	
	el[0] = 1;
	el[1] = 9;
	el[2] = 8;
	el[3] = 3;
	el[4] = 6;
	el[5] = 5;
	el[6] = 0;
	el[7] = 7;
	el[8] = 10;
	el[9] = 2;
	el[10] = 4;
	el[11] = 11;
	
	co[0] = 1;
	co[1] = 2;
	co[2] = 1;
	co[3] = 2;
	co[4] = 0;
	co[5] = 2;
	co[6] = 2;
	co[7] = 2;
	
	cl[0] = 0;
	cl[1] = 3;
	cl[2] = 1;
	cl[3] = 5;
	cl[4] = 7;
	cl[5] = 6;
	cl[6] = 2;
	cl[7] = 4;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("L");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[3] = 0;
	eo[4] = 0;
	eo[7] = 1;
	eo[11] = 0;
	
	el[3] = 11;
	el[4] = 7;
	el[7] = 6;
	el[11] = 3;
	
	co[0] = 2;
	co[3] = 0;
	co[4] = 2;
	co[7] = 1;
	
	cl[0] = 4;
	cl[3] = 7;
	cl[4] = 5;
	cl[7] = 0;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("L'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[3] = 0;
	eo[4] = 0;
	eo[7] = 0;
	eo[11] = 1;
	
	el[3] = 7;
	el[4] = 3;
	el[7] = 11;
	el[11] = 6;
	
	co[0] = 1;
	co[3] = 0;
	co[4] = 2;
	co[7] = 2;
	
	cl[0] = 5;
	cl[3] = 4;
	cl[4] = 0;
	cl[7] = 7;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("L2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("L'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("L");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("L2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("L");
	cube2.rotate("L");
	
	cube3.rotate("L'");
	cube3.rotate("L'");
	
	cube4.rotate("L2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_f_rotations1, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube2("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube3("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube4("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube5("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	
	eo[0] = 1;
	eo[1] = 0;
	eo[2] = 1;
	eo[3] = 1;
	eo[4] = 1;
	eo[5] = 1;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 1;
	eo[11] = 0;
	
	el[0] = 11;
	el[1] = 9;
	el[2] = 8;
	el[3] = 6;
	el[4] = 10;
	el[5] = 1;
	el[6] = 0;
	el[7] = 3;
	el[8] = 5;
	el[9] = 2;
	el[10] = 4;
	el[11] = 7;
	
	co[0] = 1;
	co[1] = 1;
	co[2] = 1;
	co[3] = 2;
	co[4] = 1;
	co[5] = 0;
	co[6] = 2;
	co[7] = 1;
	
	cl[0] = 4;
	cl[1] = 7;
	cl[2] = 1;
	cl[3] = 0;
	cl[4] = 6;
	cl[5] = 3;
	cl[6] = 2;
	cl[7] = 5;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("F");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[0] = 0;
	eo[4] = 1;
	eo[5] = 1;
	eo[8] = 1;
	
	el[0] = 5;
	el[4] = 1;
	el[5] = 10;
	el[8] = 11;
	
	co[0] = 0;
	co[1] = 1;
	co[4] = 1;
	co[5] = 1;
	
	cl[0] = 3;
	cl[1] = 6;
	cl[4] = 7;
	cl[5] = 4;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("F'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[0] = 0;
	eo[4] = 1;
	eo[5] = 0;
	eo[8] = 0;
	
	el[0] = 10;
	el[4] = 5;
	el[5] = 11;
	el[8] = 1;
	
	co[0] = 2;
	co[1] = 0;
	co[4] = 2;
	co[5] = 2;
	
	cl[0] = 6;
	cl[1] = 4;
	cl[4] = 3;
	cl[5] = 7;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("F2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("F'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("F");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("F2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("F");
	cube2.rotate("F");
	
	cube3.rotate("F'");
	cube3.rotate("F'");
	
	cube4.rotate("F2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_r_rotations1, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube2("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube3("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube4("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube5("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	
	eo[0] = 0;
	eo[1] = 1;
	eo[2] = 1;
	eo[3] = 1;
	eo[4] = 0;
	eo[5] = 0;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 1;
	eo[11] = 0;
	
	el[0] = 1;
	el[1] = 5;
	el[2] = 8;
	el[3] = 6;
	el[4] = 11;
	el[5] = 2;
	el[6] = 9;
	el[7] = 3;
	el[8] = 10;
	el[9] = 0;
	el[10] = 4;
	el[11] = 7;
	
	co[0] = 2;
	co[1] = 0;
	co[2] = 1;
	co[3] = 2;
	co[4] = 0;
	co[5] = 1;
	co[6] = 2;
	co[7] = 1;
	
	cl[0] = 7;
	cl[1] = 6;
	cl[2] = 3;
	cl[3] = 0;
	cl[4] = 4;
	cl[5] = 2;
	cl[6] = 1;
	cl[7] = 5;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("R");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[1] = 0;
	eo[5] = 0;
	eo[6] = 0;
	eo[9] = 1;
	
	el[1] = 0;
	el[5] = 9;
	el[6] = 2;
	el[9] = 5;
	
	co[1] = 2;
	co[2] = 1;
	co[5] = 1;
	co[6] = 0;
	
	cl[1] = 1;
	cl[2] = 2;
	cl[5] = 3;
	cl[6] = 6;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("R'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[1] = 0;
	eo[5] = 0;
	eo[6] = 1;
	eo[9] = 0;
	
	el[1] = 2;
	el[5] = 0;
	el[6] = 5;
	el[9] = 9;
	
	co[1] = 2;
	co[2] = 2;
	co[5] = 1;
	co[6] = 2;
	
	cl[1] = 2;
	cl[2] = 6;
	cl[5] = 1;
	cl[6] = 3;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("R2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("R'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("R");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("R2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("R");
	cube2.rotate("R");
	
	cube3.rotate("R'");
	cube3.rotate("R'");
	
	cube4.rotate("R2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_b_rotations1, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube2("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube3("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube4("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube5("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	
	eo[0] = 0;
	eo[1] = 0;
	eo[2] = 1;
	eo[3] = 1;
	eo[4] = 0;
	eo[5] = 1;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 1;
	eo[11] = 0;
	
	el[0] = 1;
	el[1] = 9;
	el[2] = 0;
	el[3] = 6;
	el[4] = 11;
	el[5] = 5;
	el[6] = 4;
	el[7] = 8;
	el[8] = 10;
	el[9] = 2;
	el[10] = 3;
	el[11] = 7;
	
	co[0] = 2;
	co[1] = 2;
	co[2] = 0;
	co[3] = 0;
	co[4] = 0;
	co[5] = 2;
	co[6] = 0;
	co[7] = 0;
	
	cl[0] = 7;
	cl[1] = 3;
	cl[2] = 2;
	cl[3] = 1;
	cl[4] = 4;
	cl[5] = 6;
	cl[6] = 5;
	cl[7] = 0;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("B");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[2] = 1;
	eo[6] = 0;
	eo[7] = 0;
	eo[10] = 1;
	
	el[2] = 3;
	el[6] = 8;
	el[7] = 4;
	el[10] = 0;
	
	co[2] = 0;
	co[3] = 0;
	co[6] = 0;
	co[7] = 0;
	
	cl[2] = 0;
	cl[3] = 5;
	cl[6] = 1;
	cl[7] = 2;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("B'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[2] = 1;
	eo[6] = 0;
	eo[7] = 0;
	eo[10] = 1;
	
	el[2] = 4;
	el[6] = 3;
	el[7] = 0;
	el[10] = 8;
	
	co[2] = 1;
	co[3] = 2;
	co[6] = 2;
	co[7] = 1;
	
	cl[2] = 5;
	cl[3] = 2;
	cl[6] = 0;
	cl[7] = 1;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("B2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("B'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("B");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("B2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("B");
	cube2.rotate("B");
	
	cube3.rotate("B'");
	cube3.rotate("B'");
	
	cube4.rotate("B2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_d_rotations1, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube2("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube3("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube4("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	Cube cube5("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	
	eo[0] = 0;
	eo[1] = 0;
	eo[2] = 1;
	eo[3] = 1;
	eo[4] = 0;
	eo[5] = 1;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 0;
	eo[11] = 1;
	
	el[0] = 1;
	el[1] = 9;
	el[2] = 8;
	el[3] = 6;
	el[4] = 11;
	el[5] = 5;
	el[6] = 0;
	el[7] = 3;
	el[8] = 7;
	el[9] = 10;
	el[10] = 2;
	el[11] = 4;
	
	co[0] = 2;
	co[1] = 2;
	co[2] = 1;
	co[3] = 2;
	co[4] = 1;
	co[5] = 0;
	co[6] = 2;
	co[7] = 2;
	
	cl[0] = 7;
	cl[1] = 3;
	cl[2] = 1;
	cl[3] = 0;
	cl[4] = 5;
	cl[5] = 4;
	cl[6] = 6;
	cl[7] = 2;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("D");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[8] = 0;
	eo[9] = 1;
	eo[10] = 0;
	eo[11] = 0;
	
	el[8] = 2;
	el[9] = 4;
	el[10] = 7;
	el[11] = 10;
	
	co[4] = 2;
	co[5] = 2;
	co[6] = 1;
	co[7] = 0;
	
	cl[4] = 6;
	cl[5] = 2;
	cl[6] = 5;
	cl[7] = 4;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("D'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[8] = 1;
	eo[9] = 0;
	eo[10] = 0;
	eo[11] = 0;
	
	el[8] = 4;
	el[9] = 7;
	el[10] = 10;
	el[11] = 2;
	
	co[4] = 2;
	co[5] = 1;
	co[6] = 0;
	co[7] = 2;
	
	cl[4] = 2;
	cl[5] = 5;
	cl[6] = 4;
	cl[7] = 6;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("D2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("D'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("D");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("D2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("D");
	cube2.rotate("D");
	
	cube3.rotate("D'");
	cube3.rotate("D'");
	
	cube4.rotate("D2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_loading2, cube_loading)
{
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURDRUUF");
	Cube cube2("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	eo[0] = 1;
	eo[1] = 1;
	eo[2] = 0;
	eo[3] = 1;
	eo[4] = 1;
	eo[5] = 1;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 1;
	eo[9] = 1;
	eo[10] = 0;
	eo[11] = 1;
	
	el[0] = 11;
	el[1] = 6;
	el[2] = 4;
	el[3] = 2;
	el[4] = 9;
	el[5] = 10;
	el[6] = 8;
	el[7] = 7;
	el[8] = 0;
	el[9] = 1;
	el[10] = 3;
	el[11] = 5;
	
	co[0] = 0;
	co[1] = 2;
	co[2] = 2;
	co[3] = 0;
	co[4] = 0;
	co[5] = 0;
	co[6] = 2;
	co[7] = 0;
	
	cl[0] = 5;
	cl[1] = 2;
	cl[2] = 0;
	cl[3] = 6;
	cl[4] = 7;
	cl[5] = 1;
	cl[6] = 4;
	cl[7] = 3;
	
	cube2.setEO(eo);
	cube2.setEL(el);
	cube2.setCO(co);
	cube2.setCL(cl);
	
	ASSERT_TRUE(areEquals(cube1, cube2));
}

TEST(cube_u_rotations2, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube2("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube3("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube4("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube5("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	
	eo[0] = 1;
	eo[1] = 0;
	eo[2] = 1;
	eo[3] = 1;
	eo[4] = 1;
	eo[5] = 1;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 1;
	eo[9] = 1;
	eo[10] = 0;
	eo[11] = 1;
	
	el[0] = 6;
	el[1] = 4;
	el[2] = 2;
	el[3] = 11;
	el[4] = 9;
	el[5] = 10;
	el[6] = 8;
	el[7] = 7;
	el[8] = 0;
	el[9] = 1;
	el[10] = 3;
	el[11] = 5;
	
	co[0] = 2;
	co[1] = 2;
	co[2] = 0;
	co[3] = 0;
	co[4] = 0;
	co[5] = 0;
	co[6] = 2;
	co[7] = 0;
	
	cl[0] = 2;
	cl[1] = 0;
	cl[2] = 6;
	cl[3] = 5;
	cl[4] = 7;
	cl[5] = 1;
	cl[6] = 4;
	cl[7] = 3;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("U");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[0] = 1;
	eo[1] = 1;
	eo[2] = 1;
	eo[3] = 0;
	
	el[0] = 2;
	el[1] = 11;
	el[2] = 6;
	el[3] = 4;
	
	co[0] = 0;
	co[1] = 0;
	co[2] = 2;
	co[3] = 2;
	
	cl[0] = 6;
	cl[1] = 5;
	cl[2] = 2;
	cl[3] = 0;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("U'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[0] = 0;
	eo[1] = 1;
	eo[2] = 1;
	eo[3] = 1;
	
	el[0] = 4;
	el[1] = 2;
	el[2] = 11;
	el[3] = 6;
	
	co[0] = 2;
	co[1] = 0;
	co[2] = 0;
	co[3] = 2;
	
	cl[0] = 0;
	cl[1] = 6;
	cl[2] = 5;
	cl[3] = 2;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("U2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("U'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("U");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("U2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("U");
	cube2.rotate("U");
	
	cube3.rotate("U'");
	cube3.rotate("U'");
	
	cube4.rotate("U2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_l_rotations2, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube2("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube3("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube4("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube5("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	
	eo[0] = 1;
	eo[1] = 1;
	eo[2] = 0;
	eo[3] = 0;
	eo[4] = 1;
	eo[5] = 1;
	eo[6] = 0;
	eo[7] = 1;
	eo[8] = 1;
	eo[9] = 1;
	eo[10] = 0;
	eo[11] = 1;
	
	el[0] = 11;
	el[1] = 6;
	el[2] = 4;
	el[3] = 7;
	el[4] = 2;
	el[5] = 10;
	el[6] = 8;
	el[7] = 5;
	el[8] = 0;
	el[9] = 1;
	el[10] = 3;
	el[11] = 9;
	
	co[0] = 2;
	co[1] = 2;
	co[2] = 2;
	co[3] = 1;
	co[4] = 1;
	co[5] = 0;
	co[6] = 2;
	co[7] = 2;
	
	cl[0] = 6;
	cl[1] = 2;
	cl[2] = 0;
	cl[3] = 3;
	cl[4] = 5;
	cl[5] = 1;
	cl[6] = 4;
	cl[7] = 7;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("L");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[3] = 1;
	eo[4] = 1;
	eo[7] = 1;
	eo[11] = 0;
	
	el[3] = 9;
	el[4] = 5;
	el[7] = 2;
	el[11] = 7;
	
	co[0] = 2;
	co[3] = 1;
	co[4] = 1;
	co[7] = 2;
	
	cl[0] = 7;
	cl[3] = 5;
	cl[4] = 3;
	cl[7] = 6;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("L'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[3] = 1;
	eo[4] = 0;
	eo[7] = 1;
	eo[11] = 1;
	
	el[3] = 5;
	el[4] = 7;
	el[7] = 9;
	el[11] = 2;
	
	co[0] = 0;
	co[3] = 0;
	co[4] = 0;
	co[7] = 0;
	
	cl[0] = 3;
	cl[3] = 7;
	cl[4] = 6;
	cl[7] = 5;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("L2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("L'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("L");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("L2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("L");
	cube2.rotate("L");
	
	cube3.rotate("L'");
	cube3.rotate("L'");
	
	cube4.rotate("L2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_f_rotations2, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube2("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube3("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube4("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube5("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	
	eo[0] = 0;
	eo[1] = 1;
	eo[2] = 0;
	eo[3] = 1;
	eo[4] = 0;
	eo[5] = 0;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 1;
	eo[10] = 0;
	eo[11] = 1;
	
	el[0] = 9;
	el[1] = 6;
	el[2] = 4;
	el[3] = 2;
	el[4] = 0;
	el[5] = 11;
	el[6] = 8;
	el[7] = 7;
	el[8] = 10;
	el[9] = 1;
	el[10] = 3;
	el[11] = 5;
	
	co[0] = 1;
	co[1] = 2;
	co[2] = 2;
	co[3] = 0;
	co[4] = 2;
	co[5] = 0;
	co[6] = 2;
	co[7] = 0;
	
	cl[0] = 7;
	cl[1] = 5;
	cl[2] = 0;
	cl[3] = 6;
	cl[4] = 1;
	cl[5] = 2;
	cl[6] = 4;
	cl[7] = 3;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("F");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[0] = 0;
	eo[4] = 0;
	eo[5] = 0;
	eo[8] = 0;
	
	el[0] = 10;
	el[4] = 11;
	el[5] = 0;
	el[8] = 9;
	
	co[0] = 0;
	co[1] = 2;
	co[4] = 2;
	co[5] = 1;
	
	cl[0] = 2;
	cl[1] = 1;
	cl[4] = 5;
	cl[5] = 7;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("F'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[0] = 1;
	eo[4] = 1;
	eo[5] = 1;
	eo[8] = 1;
	
	el[0] = 0;
	el[4] = 10;
	el[5] = 9;
	el[8] = 11;
	
	co[0] = 0;
	co[1] = 0;
	co[4] = 2;
	co[5] = 0;
	
	cl[0] = 1;
	cl[1] = 7;
	cl[4] = 2;
	cl[5] = 5;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("F2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("F'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("F");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("F2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("F");
	cube2.rotate("F");
	
	cube3.rotate("F'");
	cube3.rotate("F'");
	
	cube4.rotate("F2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_r_rotations2, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube2("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube3("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube4("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube5("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	
	eo[0] = 1;
	eo[1] = 1;
	eo[2] = 0;
	eo[3] = 1;
	eo[4] = 1;
	eo[5] = 1;
	eo[6] = 1;
	eo[7] = 0;
	eo[8] = 1;
	eo[9] = 0;
	eo[10] = 0;
	eo[11] = 1;
	
	el[0] = 11;
	el[1] = 10;
	el[2] = 4;
	el[3] = 2;
	el[4] = 9;
	el[5] = 1;
	el[6] = 6;
	el[7] = 7;
	el[8] = 0;
	el[9] = 8;
	el[10] = 3;
	el[11] = 5;
	
	co[0] = 0;
	co[1] = 1;
	co[2] = 1;
	co[3] = 0;
	co[4] = 0;
	co[5] = 1;
	co[6] = 0;
	co[7] = 0;
	
	cl[0] = 5;
	cl[1] = 1;
	cl[2] = 2;
	cl[3] = 6;
	cl[4] = 7;
	cl[5] = 4;
	cl[6] = 0;
	cl[7] = 3;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("R");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[1] = 0;
	eo[5] = 1;
	eo[6] = 1;
	eo[9] = 1;
	
	el[1] = 8;
	el[5] = 6;
	el[6] = 1;
	el[9] = 10;
	
	co[1] = 0;
	co[2] = 1;
	co[5] = 1;
	co[6] = 1;
	
	cl[1] = 0;
	cl[2] = 4;
	cl[5] = 2;
	cl[6] = 1;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("R'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[1] = 1;
	eo[5] = 0;
	eo[6] = 1;
	eo[9] = 1;
	
	el[1] = 1;
	el[5] = 8;
	el[6] = 10;
	el[9] = 6;
	
	co[1] = 2;
	co[2] = 0;
	co[5] = 2;
	co[6] = 2;
	
	cl[1] = 4;
	cl[2] = 1;
	cl[5] = 0;
	cl[6] = 2;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("R2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("R'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("R");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("R2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("R");
	cube2.rotate("R");
	
	cube3.rotate("R'");
	cube3.rotate("R'");
	
	cube4.rotate("R2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_b_rotations2, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube2("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube3("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube4("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	Cube cube5("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURURUUF");
	
	eo[0] = 1;
	eo[1] = 1;
	eo[2] = 1;
	eo[3] = 1;
	eo[4] = 1;
	eo[5] = 1;
	eo[6] = 1;
	eo[7] = 1;
	eo[8] = 1;
	eo[9] = 1;
	eo[10] = 1;
	eo[11] = 1;
	
	el[0] = 11;
	el[1] = 6;
	el[2] = 8;
	el[3] = 2;
	el[4] = 9;
	el[5] = 10;
	el[6] = 3;
	el[7] = 4;
	el[8] = 0;
	el[9] = 1;
	el[10] = 7;
	el[11] = 5;
	
	co[0] = 0;
	co[1] = 2;
	co[2] = 0;
	co[3] = 1;
	co[4] = 0;
	co[5] = 0;
	co[6] = 2;
	co[7] = 1;
	
	cl[0] = 5;
	cl[1] = 2;
	cl[2] = 4;
	cl[3] = 0;
	cl[4] = 7;
	cl[5] = 1;
	cl[6] = 3;
	cl[7] = 6;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("B");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[2] = 1;
	eo[6] = 1;
	eo[7] = 1;
	eo[10] = 1;
	
	el[2] = 7;
	el[6] = 4;
	el[7] = 3;
	el[10] = 8;
	
	co[2] = 1;
	co[3] = 2;
	co[6] = 1;
	co[7] = 0;
	
	cl[2] = 6;
	cl[3] = 3;
	cl[6] = 0;
	cl[7] = 4;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("B'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[2] = 0;
	eo[6] = 0;
	eo[7] = 0;
	eo[10] = 0;
	
	el[2] = 3;
	el[6] = 7;
	el[7] = 8;
	el[10] = 4;
	
	co[2] = 0;
	co[3] = 2;
	co[6] = 0;
	co[7] = 2;
	
	cl[2] = 3;
	cl[3] = 4;
	cl[6] = 6;
	cl[7] = 0;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("B2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("B'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("B");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("B2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("B");
	cube2.rotate("B");
	
	cube3.rotate("B'");
	cube3.rotate("B'");
	
	cube4.rotate("B2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_d_rotations2, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURDRUUF");
	Cube cube2("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURDRUUF");
	Cube cube3("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURDRUUF");
	Cube cube4("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURDRUUF");
	Cube cube5("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURDRUUF");
	
	eo[0] = 1;
	eo[1] = 1;
	eo[2] = 0;
	eo[3] = 1;
	eo[4] = 1;
	eo[5] = 1;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 1;
	eo[9] = 1;
	eo[10] = 1;
	eo[11] = 0;
	
	el[0] = 11;
	el[1] = 6;
	el[2] = 4;
	el[3] = 2;
	el[4] = 9;
	el[5] = 10;
	el[6] = 8;
	el[7] = 7;
	el[8] = 5;
	el[9] = 0;
	el[10] = 1;
	el[11] = 3;
	
	co[0] = 0;
	co[1] = 2;
	co[2] = 2;
	co[3] = 0;
	co[4] = 0;
	co[5] = 0;
	co[6] = 0;
	co[7] = 2;
	
	cl[0] = 5;
	cl[1] = 2;
	cl[2] = 0;
	cl[3] = 6;
	cl[4] = 3;
	cl[5] = 7;
	cl[6] = 1;
	cl[7] = 4;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("D");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[8] = 1;
	eo[9] = 0;
	eo[10] = 1;
	eo[11] = 1;
	
	el[8] = 1;
	el[9] = 3;
	el[10] = 5;
	el[11] = 0;
	
	co[4] = 0;
	co[5] = 2;
	co[6] = 0;
	co[7] = 0;
	
	cl[4] = 1;
	cl[5] = 4;
	cl[6] = 3;
	cl[7] = 7;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("D'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[8] = 0;
	eo[9] = 1;
	eo[10] = 1;
	eo[11] = 1;
	
	el[8] = 3;
	el[9] = 5;
	el[10] = 0;
	el[11] = 1;
	
	co[4] = 2;
	co[5] = 0;
	co[6] = 0;
	co[7] = 0;
	
	cl[4] = 4;
	cl[5] = 3;
	cl[6] = 7;
	cl[7] = 1;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("D2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("D'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("D");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("D2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("D");
	cube2.rotate("D");
	
	cube3.rotate("D'");
	cube3.rotate("D'");
	
	cube4.rotate("D2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_loading3, cube_loading)
{
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube2("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	eo[0] = 0;
	eo[1] = 0;
	eo[2] = 1;
	eo[3] = 1;
	eo[4] = 0;
	eo[5] = 0;
	eo[6] = 1;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 0;
	eo[11] = 1;
	
	el[0] = 11;
	el[1] = 1;
	el[2] = 6;
	el[3] = 4;
	el[4] = 10;
	el[5] = 7;
	el[6] = 9;
	el[7] = 8;
	el[8] = 3;
	el[9] = 2;
	el[10] = 5;
	el[11] = 0;
	
	co[0] = 2;
	co[1] = 2;
	co[2] = 0;
	co[3] = 1;
	co[4] = 0;
	co[5] = 0;
	co[6] = 0;
	co[7] = 1;
	
	cl[0] = 0;
	cl[1] = 1;
	cl[2] = 4;
	cl[3] = 5;
	cl[4] = 6;
	cl[5] = 7;
	cl[6] = 2;
	cl[7] = 3;
	
	cube2.setEO(eo);
	cube2.setEL(el);
	cube2.setCO(co);
	cube2.setCL(cl);
	
	ASSERT_TRUE(areEquals(cube1, cube2));
}

TEST(cube_u_rotations3, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube2("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube3("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube4("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube5("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	
	eo[0] = 0;
	eo[1] = 1;
	eo[2] = 1;
	eo[3] = 0;
	eo[4] = 0;
	eo[5] = 0;
	eo[6] = 1;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 0;
	eo[11] = 1;
	
	el[0] = 1;
	el[1] = 6;
	el[2] = 4;
	el[3] = 11;
	el[4] = 10;
	el[5] = 7;
	el[6] = 9;
	el[7] = 8;
	el[8] = 3;
	el[9] = 2;
	el[10] = 5;
	el[11] = 0;
	
	co[0] = 2;
	co[1] = 0;
	co[2] = 1;
	co[3] = 2;
	co[4] = 0;
	co[5] = 0;
	co[6] = 0;
	co[7] = 1;
	
	cl[0] = 1;
	cl[1] = 4;
	cl[2] = 5;
	cl[3] = 0;
	cl[4] = 6;
	cl[5] = 7;
	cl[6] = 2;
	cl[7] = 3;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("U");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[0] = 1;
	eo[1] = 0;
	eo[2] = 0;
	eo[3] = 1;
	
	el[0] = 4;
	el[1] = 11;
	el[2] = 1;
	el[3] = 6;
	
	co[0] = 1;
	co[1] = 2;
	co[2] = 2;
	co[3] = 0;
	
	cl[0] = 5;
	cl[1] = 0;
	cl[2] = 1;
	cl[3] = 4;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("U'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[0] = 1;
	eo[1] = 1;
	eo[2] = 0;
	eo[3] = 0;
	
	el[0] = 6;
	el[1] = 4;
	el[2] = 11;
	el[3] = 1;
	
	co[0] = 0;
	co[1] = 1;
	co[2] = 2;
	co[3] = 2;
	
	cl[0] = 4;
	cl[1] = 5;
	cl[2] = 0;
	cl[3] = 1;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("U2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("U'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("U");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("U2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("U");
	cube2.rotate("U");
	
	cube3.rotate("U'");
	cube3.rotate("U'");
	
	cube4.rotate("U2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_l_rotations3, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube2("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube3("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube4("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube5("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	
	eo[0] = 0;
	eo[1] = 0;
	eo[2] = 1;
	eo[3] = 0;
	eo[4] = 1;
	eo[5] = 0;
	eo[6] = 1;
	eo[7] = 1;
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 0;
	eo[11] = 0;
	
	el[0] = 11;
	el[1] = 1;
	el[2] = 6;
	el[3] = 8;
	el[4] = 4;
	el[5] = 7;
	el[6] = 9;
	el[7] = 0;
	el[8] = 3;
	el[9] = 2;
	el[10] = 5;
	el[11] = 10;
	
	co[0] = 0;
	co[1] = 2;
	co[2] = 0;
	co[3] = 2;
	co[4] = 0;
	co[5] = 0;
	co[6] = 0;
	co[7] = 2;
	
	cl[0] = 5;
	cl[1] = 1;
	cl[2] = 4;
	cl[3] = 3;
	cl[4] = 0;
	cl[5] = 7;
	cl[6] = 2;
	cl[7] = 6;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("L");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[3] = 0;
	eo[4] = 1;
	eo[7] = 1;
	eo[11] = 0;
	
	el[3] = 10;
	el[4] = 0;
	el[7] = 4;
	el[11] = 8;
	
	co[0] = 2;
	co[3] = 0;
	co[4] = 2;
	co[7] = 0;
	
	cl[0] = 6;
	cl[3] = 0;
	cl[4] = 3;
	cl[7] = 5;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("L'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[3] = 1;
	eo[4] = 0;
	eo[7] = 0;
	eo[11] = 1;
	
	el[3] = 0;
	el[4] = 8;
	el[7] = 10;
	el[11] = 4;
	
	co[0] = 1;
	co[3] = 0;
	co[4] = 1;
	co[7] = 2;
	
	cl[0] = 3;
	cl[3] = 6;
	cl[4] = 5;
	cl[7] = 0;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("L2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("L'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("L");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("L2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("L");
	cube2.rotate("L");
	
	cube3.rotate("L'");
	cube3.rotate("L'");
	
	cube4.rotate("L2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_f_rotations3, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube2("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube3("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube4("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube5("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	
	eo[0] = 1;
	eo[1] = 0;
	eo[2] = 1;
	eo[3] = 1;
	eo[4] = 1;
	eo[5] = 1;
	eo[6] = 1;
	eo[7] = 0;
	eo[8] = 1;
	eo[9] = 0;
	eo[10] = 0;
	eo[11] = 1;
	
	el[0] = 10;
	el[1] = 1;
	el[2] = 6;
	el[3] = 4;
	el[4] = 3;
	el[5] = 11;
	el[6] = 9;
	el[7] = 8;
	el[8] = 7;
	el[9] = 2;
	el[10] = 5;
	el[11] = 0;
	
	co[0] = 1;
	co[1] = 1;
	co[2] = 0;
	co[3] = 1;
	co[4] = 2;
	co[5] = 0;
	co[6] = 0;
	co[7] = 1;
	
	cl[0] = 6;
	cl[1] = 0;
	cl[2] = 4;
	cl[3] = 5;
	cl[4] = 7;
	cl[5] = 1;
	cl[6] = 2;
	cl[7] = 3;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("F");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[0] = 1;
	eo[4] = 1;
	eo[5] = 1;
	eo[8] = 1;
	
	el[0] = 7;
	el[4] = 11;
	el[5] = 3;
	el[8] = 10;
	
	co[0] = 0;
	co[1] = 2;
	co[4] = 1;
	co[5] = 1;
	
	cl[0] = 1;
	cl[1] = 7;
	cl[4] = 0;
	cl[5] = 6;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("F'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[0] = 0;
	eo[4] = 0;
	eo[5] = 0;
	eo[8] = 0;
	
	el[0] = 3;
	el[4] = 7;
	el[5] = 10;
	el[8] = 11;
	
	co[0] = 0;
	co[1] = 0;
	co[4] = 2;
	co[5] = 2;
	
	cl[0] = 7;
	cl[1] = 6;
	cl[4] = 1;
	cl[5] = 0;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("F2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("F'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("F");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("F2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("F");
	cube2.rotate("F");
	
	cube3.rotate("F'");
	cube3.rotate("F'");
	
	cube4.rotate("F2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_r_rotations3, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube2("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube3("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube4("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube5("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	
	eo[0] = 0;
	eo[1] = 0;
	eo[2] = 1;
	eo[3] = 1;
	eo[4] = 0;
	eo[5] = 0;
	eo[6] = 0;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 1;
	eo[10] = 0;
	eo[11] = 1;
	
	el[0] = 11;
	el[1] = 7;
	el[2] = 6;
	el[3] = 4;
	el[4] = 10;
	el[5] = 2;
	el[6] = 1;
	el[7] = 8;
	el[8] = 3;
	el[9] = 9;
	el[10] = 5;
	el[11] = 0;
	
	co[0] = 2;
	co[1] = 1;
	co[2] = 1;
	co[3] = 1;
	co[4] = 0;
	co[5] = 2;
	co[6] = 1;
	co[7] = 1;
	
	cl[0] = 0;
	cl[1] = 7;
	cl[2] = 1;
	cl[3] = 5;
	cl[4] = 6;
	cl[5] = 2;
	cl[6] = 4;
	cl[7] = 3;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("R");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[1] = 1;
	eo[5] = 0;
	eo[6] = 0;
	eo[9] = 0;
	
	el[1] = 9;
	el[5] = 1;
	el[6] = 2;
	el[9] = 7;
	
	co[1] = 1;
	co[2] = 2;
	co[5] = 1;
	co[6] = 1;
	
	cl[1] = 4;
	cl[2] = 2;
	cl[5] = 1;
	cl[6] = 7;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("R'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[1] = 0;
	eo[5] = 1;
	eo[6] = 0;
	eo[9] = 0;
	
	el[1] = 2;
	el[5] = 9;
	el[6] = 7;
	el[9] = 1;
	
	co[1] = 0;
	co[2] = 0;
	co[5] = 0;
	co[6] = 2;
	
	cl[1] = 2;
	cl[2] = 7;
	cl[5] = 4;
	cl[6] = 1;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("R2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("R'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("R");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("R2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("R");
	cube2.rotate("R");
	
	cube3.rotate("R'");
	cube3.rotate("R'");
	
	cube4.rotate("R2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_b_rotations3, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube2("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube3("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube4("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube5("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	
	eo[0] = 0;
	eo[1] = 0;
	eo[2] = 0;
	eo[3] = 1;
	eo[4] = 0;
	eo[5] = 0;
	eo[6] = 1;
	eo[7] = 0;
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 1;
	eo[11] = 1;
	
	el[0] = 11;
	el[1] = 1;
	el[2] = 9;
	el[3] = 4;
	el[4] = 10;
	el[5] = 7;
	el[6] = 5;
	el[7] = 6;
	el[8] = 3;
	el[9] = 2;
	el[10] = 8;
	el[11] = 0;
	
	co[0] = 2;
	co[1] = 2;
	co[2] = 1;
	co[3] = 2;
	co[4] = 0;
	co[5] = 0;
	co[6] = 0;
	co[7] = 2;
	
	cl[0] = 0;
	cl[1] = 1;
	cl[2] = 2;
	cl[3] = 4;
	cl[4] = 6;
	cl[5] = 7;
	cl[6] = 3;
	cl[7] = 5;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("B");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[2] = 1;
	eo[6] = 0;
	eo[7] = 1;
	eo[10] = 0;
	
	el[2] = 8;
	el[6] = 6;
	el[7] = 5;
	el[10] = 9;
	
	co[2] = 2;
	co[3] = 0;
	co[6] = 2;
	co[7] = 1;
	
	cl[2] = 5;
	cl[3] = 3;
	cl[6] = 4;
	cl[7] = 2;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("B'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[2] = 5;
	eo[6] = 8;
	eo[7] = 9;
	eo[10] = 6;
	
	el[2] = 5;
	el[6] = 8;
	el[7] = 9;
	el[10] = 6;
	
	co[2] = 1;
	co[3] = 0;
	co[6] = 1;
	co[7] = 0;
	
	cl[2] = 3;
	cl[3] = 2;
	cl[6] = 5;
	cl[7] = 4;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("B2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("B'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("B");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("B2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("B");
	cube2.rotate("B");
	
	cube3.rotate("B'");
	cube3.rotate("B'");
	
	cube4.rotate("B2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

TEST(cube_d_rotations3, cube_rotations)
{
	Cube::initialize();
	
	array<uint8_t, NUM_EDGES> eo;
	array<uint8_t, NUM_EDGES> el;
	
	array<uint8_t, NUM_CORNERS> co;
	array<uint8_t, NUM_CORNERS> cl;
	
	Cube cube1("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube2("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube3("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube4("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	Cube cube5("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	
	eo[0] = 0;
	eo[1] = 0;
	eo[2] = 1;
	eo[3] = 1;
	eo[4] = 0;
	eo[5] = 0;
	eo[6] = 1;
	eo[7] = 0;
	eo[8] = 1;
	eo[9] = 0;
	eo[10] = 0;
	eo[11] = 0;
	
	el[0] = 11;
	el[1] = 1;
	el[2] = 6;
	el[3] = 4;
	el[4] = 10;
	el[5] = 7;
	el[6] = 9;
	el[7] = 8;
	el[8] = 0;
	el[9] = 3;
	el[10] = 2;
	el[11] = 5;
	
	co[0] = 2;
	co[1] = 2;
	co[2] = 0;
	co[3] = 1;
	co[4] = 1;
	co[5] = 0;
	co[6] = 0;
	co[7] = 0;
	
	cl[0] = 0;
	cl[1] = 1;
	cl[2] = 4;
	cl[3] = 5;
	cl[4] = 3;
	cl[5] = 6;
	cl[6] = 7;
	cl[7] = 2;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube2.rotate("D");
	
	ASSERT_TRUE(areEquals(cube1, cube2));
	
	eo[8] = 0;
	eo[9] = 0;
	eo[10] = 1;
	eo[11] = 0;
	
	el[8] = 2;
	el[9] = 5;
	el[10] = 0;
	el[11] = 3;
	
	co[4] = 0;
	co[5] = 0;
	co[6] = 1;
	co[7] = 0;
	
	cl[4] = 7;
	cl[5] = 2;
	cl[6] = 3;
	cl[7] = 6;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube3.rotate("D'");
	
	ASSERT_TRUE(areEquals(cube1, cube3));
	
	eo[8] = 0;
	eo[9] = 1;
	eo[10] = 0;
	eo[11] = 0;
	
	el[8] = 5;
	el[9] = 0;
	el[10] = 3;
	el[11] = 2;
	
	co[4] = 0;
	co[5] = 1;
	co[6] = 0;
	co[7] = 0;
	
	cl[4] = 2;
	cl[5] = 3;
	cl[6] = 6;
	cl[7] = 7;
	
	cube1.setEO(eo);
	cube1.setEL(el);
	cube1.setCO(co);
	cube1.setCL(cl);
	
	cube4.rotate("D2");
	
	ASSERT_TRUE(areEquals(cube1, cube4));
	
	cube2.rotate("D'");
	ASSERT_TRUE(areEquals(cube2, cube5));
	
	cube3.rotate("D");
	ASSERT_TRUE(areEquals(cube3, cube5));
	
	cube4.rotate("D2");
	ASSERT_TRUE(areEquals(cube4, cube5));
	
	cube2.rotate("D");
	cube2.rotate("D");
	
	cube3.rotate("D'");
	cube3.rotate("D'");
	
	cube4.rotate("D2");
	
	ASSERT_TRUE(areEquals(cube2, cube3));
	ASSERT_TRUE(areEquals(cube4, cube3));
}

//the following tests take random cubes and a solution and apply the solution and checks that they are solved
TEST(cube_solutions1, cube_rotations)
{
	Cube cube("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	
	vector<string> solutions = {"R2", "L'", "B'", "U'", "F2", "R'", "U2", "R", "U", "L", "D", "L'", "B2", "U", "F2", "L2", "D2", "F2", "R2", "F2"};
	
	for (string s : solutions) {
		cube.rotate(s);
	}
	
	ASSERT_TRUE(cube.isSolved());
}

TEST(cube_solutions2, cube_rotations)
{
	Cube cube("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURDRUUF");
	
	vector<string> solutions = {"U", "D'", "B'", "D2", "R", "D", "R'", "D", "F", "L'", "B'", "U2", "L'", "D2", "F2", "R2", "F2", "U", "R2", "L2"};
	
	for (string s : solutions) {
		cube.rotate(s);
	}
	
	ASSERT_TRUE(cube.isSolved());
}

TEST(cube_solutions3, cube_rotations)
{
	Cube cube("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	
	vector<string> solutions = {"U", "D2", "F'", "D'", "F'", "D2", "B2", "U'", "R'", "B", "D2", "B2", "R2", "F2", "U'", "L2", "D'", "R2", "U'", "B2"};
	
	for (string s : solutions) {
		cube.rotate(s);
	}
	
	ASSERT_TRUE(cube.isSolved());
}

TEST(cube_solutions4, cube_rotations)
{
	Cube::initialize();
	
	Cube cube("RBFLUBRRRBBUDLUUFBFFFFFDUUDDRURRFLLBLUDDBBLDRLRFLDUBLD");
	vector<string> solutions = {"L2", "D2", "R'", "D2", "F2", "L'", "B", "L'", "F", "U", "D'", "R'", "L2", "F2", "U", "B2", "R2", "U", "L2", "U'"};

	for (string s : solutions) {
		cube.rotate(s);
	}
	
	ASSERT_TRUE(cube.isSolved());
}

TEST(cube_solutions5, cube_rotations)
{
	Cube::initialize();
	
	Cube cube("BUFFUFLLDRDDULUDDRFDRFFBBLUFRUURBRDLLLURBRBLLDBFRDBBFU");
	vector<string> solutions = {"B'", "U2", "R", "D'", "B'", "U", "F", "R", "U'", "L", "B2", "U2", "L2", "B2", "D2", "R2", "U", "F2", "U'", "F2"};
	
	for (string s : solutions) {
		cube.rotate(s);
	}
	
	ASSERT_TRUE(cube.isSolved());
}

//checks the zobrist tables have no repeated values
TEST(no_zobrist_repeats, zobristKeys)
{
	Cube::initialize();
	
	unordered_set<uint64_t> seen;
	
	for (size_t i = 0; i < NUM_EDGES; i++) {
		for (size_t j = 0; j < NUM_EDGES; j++) {
			for (int k = 0; k < 2; k++) {
				ASSERT_TRUE(!seen.contains(zobristEdgeRef[i][j][k]));
				seen.insert(zobristEdgeRef[i][j][k]);
			}
		}
	}
	
	for (size_t i = 0; i < NUM_CORNERS; i++) {
		for (size_t j = 0; j < NUM_CORNERS; j++) {
			for (int k = 0; k < 2; k++) {
				ASSERT_TRUE(!seen.contains(zobristCornerRef[i][j][k]));
				seen.insert(zobristCornerRef[i][j][k]);
			}
		}
	}
}

//checks getZobrist() correctly calculates and returns zobrist keys for cubes
TEST(zobristKey_cube1, zobristKeys)
{
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	uint64_t key = 0;
	
	key ^= zobristEdgeRef[0][0][0];
	key ^= zobristEdgeRef[1][1][0];
	key ^= zobristEdgeRef[2][2][0];
	key ^= zobristEdgeRef[3][3][0];
	key ^= zobristEdgeRef[4][4][0];
	key ^= zobristEdgeRef[5][5][0];
	key ^= zobristEdgeRef[6][6][0];
	key ^= zobristEdgeRef[7][7][0];
	key ^= zobristEdgeRef[8][8][0];
	key ^= zobristEdgeRef[9][9][0];
	key ^= zobristEdgeRef[10][10][0];
	key ^= zobristEdgeRef[11][11][0];
	
	key ^= zobristCornerRef[0][0][0];
	key ^= zobristCornerRef[1][1][0];
	key ^= zobristCornerRef[2][2][0];
	key ^= zobristCornerRef[3][3][0];
	key ^= zobristCornerRef[4][4][0];
	key ^= zobristCornerRef[5][5][0];
	key ^= zobristCornerRef[6][6][0];
	key ^= zobristCornerRef[7][7][0];
	
	ASSERT_EQ(key, cube.getZobrist());
}

TEST(zobristKey_cube2, zobristKeys)
{
	Cube cube("LFRRUDLUBUBBLLLDLLDRLDFRFBDURUFRFRBUFDFUBUBFRDDBBDUFLR");
	
	uint64_t key = 0;
	
	key ^= zobristEdgeRef[0][1][0];
	key ^= zobristEdgeRef[1][9][0];
	key ^= zobristEdgeRef[2][8][1];
	key ^= zobristEdgeRef[3][6][1];
	key ^= zobristEdgeRef[4][11][0];
	key ^= zobristEdgeRef[5][5][1];
	key ^= zobristEdgeRef[6][0][0];
	key ^= zobristEdgeRef[7][3][0];
	key ^= zobristEdgeRef[8][10][0];
	key ^= zobristEdgeRef[9][2][0];
	key ^= zobristEdgeRef[10][4][1];
	key ^= zobristEdgeRef[11][7][0];

	key ^= zobristCornerRef[0][7][2];
	key ^= zobristCornerRef[1][3][2];
	key ^= zobristCornerRef[2][1][1];
	key ^= zobristCornerRef[3][0][2];
	key ^= zobristCornerRef[4][4][0];
	key ^= zobristCornerRef[5][6][2];
	key ^= zobristCornerRef[6][2][2];
	key ^= zobristCornerRef[7][5][1];
	
	ASSERT_EQ(key, cube.getZobrist());
}

TEST(zobristKey_cube3, zobristKeys)
{
	Cube cube("DFLBURDLRRURLLDBFBFDBRFBLURUBFDRFFUDULBDBBLLLDFURDRUUF");
	
	uint64_t key = 0;
	
	key ^= zobristEdgeRef[0][11][1];
	key ^= zobristEdgeRef[1][6][1];
	key ^= zobristEdgeRef[2][4][0];
	key ^= zobristEdgeRef[3][2][1];
	key ^= zobristEdgeRef[4][9][1];
	key ^= zobristEdgeRef[5][10][1];
	key ^= zobristEdgeRef[6][8][0];
	key ^= zobristEdgeRef[7][7][0];
	key ^= zobristEdgeRef[8][0][1];
	key ^= zobristEdgeRef[9][1][1];
	key ^= zobristEdgeRef[10][3][0];
	key ^= zobristEdgeRef[11][5][1];

	key ^= zobristCornerRef[0][5][0];
	key ^= zobristCornerRef[1][2][2];
	key ^= zobristCornerRef[2][0][2];
	key ^= zobristCornerRef[3][6][0];
	key ^= zobristCornerRef[4][7][0];
	key ^= zobristCornerRef[5][1][0];
	key ^= zobristCornerRef[6][4][2];
	key ^= zobristCornerRef[7][3][0];
	
	ASSERT_EQ(key, cube.getZobrist());
}

TEST(zobristKey_cube4, zobristKeys)
{
	Cube cube("FRDLUULDFRFFFLBUURULRDFBBLBURFLRDLBBLBDRBDRRBDUDFDULFU");
	
	uint64_t key = 0;
	
	key ^= zobristEdgeRef[0][11][0];
	key ^= zobristEdgeRef[1][1][0];
	key ^= zobristEdgeRef[2][6][1];
	key ^= zobristEdgeRef[3][4][1];
	key ^= zobristEdgeRef[4][10][0];
	key ^= zobristEdgeRef[5][7][0];
	key ^= zobristEdgeRef[6][9][1];
	key ^= zobristEdgeRef[7][8][0];
	key ^= zobristEdgeRef[8][3][0];
	key ^= zobristEdgeRef[9][2][0];
	key ^= zobristEdgeRef[10][5][0];
	key ^= zobristEdgeRef[11][0][1];

	key ^= zobristCornerRef[0][0][2];
	key ^= zobristCornerRef[1][1][2];
	key ^= zobristCornerRef[2][4][0];
	key ^= zobristCornerRef[3][5][1];
	key ^= zobristCornerRef[4][6][0];
	key ^= zobristCornerRef[5][7][0];
	key ^= zobristCornerRef[6][2][0];
	key ^= zobristCornerRef[7][3][1];
	
	ASSERT_EQ(key, cube.getZobrist());
}

TEST(zobristKey_cube5, zobristKeys)
{
	Cube cube("RBFLUBRRRBBUDLUUFBFFFFFDUUDDRURRFLLBLUDDBBLDRLRFLDUBLD");
	
	uint64_t key = 0;
	
	key ^= zobristEdgeRef[0][5][1];
	key ^= zobristEdgeRef[1][6][0];
	key ^= zobristEdgeRef[2][2][1];
	key ^= zobristEdgeRef[3][7][1];
	key ^= zobristEdgeRef[4][0][1];
	key ^= zobristEdgeRef[5][9][0];
	key ^= zobristEdgeRef[6][8][0];
	key ^= zobristEdgeRef[7][10][1];
	key ^= zobristEdgeRef[8][1][1];
	key ^= zobristEdgeRef[9][3][0];
	key ^= zobristEdgeRef[10][11][1];
	key ^= zobristEdgeRef[11][4][1];

	key ^= zobristCornerRef[0][1][1];
	key ^= zobristCornerRef[1][5][2];
	key ^= zobristCornerRef[2][0][1];
	key ^= zobristCornerRef[3][6][1];
	key ^= zobristCornerRef[4][3][1];
	key ^= zobristCornerRef[5][4][2];
	key ^= zobristCornerRef[6][7][0];
	key ^= zobristCornerRef[7][2][1];
	
	ASSERT_EQ(key, cube.getZobrist());
}

TEST(zobristKey_cube6, zobristKeys)
{
	Cube cube("BUFFUFLLDRDDULUDDRFDRFFBBLUFRUURBRDLLLURBRBLLDBFRDBBFU");
	
	uint64_t key = 0;
	
	key ^= zobristEdgeRef[0][11][1];
	key ^= zobristEdgeRef[1][5][0];
	key ^= zobristEdgeRef[2][3][0];
	key ^= zobristEdgeRef[3][8][1];
	key ^= zobristEdgeRef[4][0][1];
	key ^= zobristEdgeRef[5][2][1];
	key ^= zobristEdgeRef[6][6][1];
	key ^= zobristEdgeRef[7][1][1];
	key ^= zobristEdgeRef[8][7][0];
	key ^= zobristEdgeRef[9][10][1];
	key ^= zobristEdgeRef[10][4][0];
	key ^= zobristEdgeRef[11][9][1];

	key ^= zobristCornerRef[0][4][1];
	key ^= zobristCornerRef[1][5][0];
	key ^= zobristCornerRef[2][0][1];
	key ^= zobristCornerRef[3][2][1];
	key ^= zobristCornerRef[4][6][0];
	key ^= zobristCornerRef[5][1][2];
	key ^= zobristCornerRef[6][3][0];
	key ^= zobristCornerRef[7][7][1];
	
	ASSERT_EQ(key, cube.getZobrist());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}