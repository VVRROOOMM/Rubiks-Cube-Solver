#include "Cube.h"

#include <iostream>
#include <cmath>
#include <random>

uint8_t moveTableEL[NUM_MOVES][NUM_EDGES] = {};
uint8_t moveTableEO[NUM_MOVES][NUM_EDGES] = {};
uint8_t moveTableCL[NUM_MOVES][NUM_CORNERS] = {};
uint8_t moveTableCO[NUM_MOVES][NUM_CORNERS] = {};
bool half_turn[NUM_MOVES] = {false};

long factorials[NUM_EDGES + 1] = {};
uint16_t choose[NUM_EDGES][5] = {};

uint64_t zobristEdgeActual[NUM_EDGES][NUM_EDGES][2] = {};
uint64_t zobristCornerActual[NUM_CORNERS][NUM_CORNERS][3] = {};

const uint64_t (&zobristEdgeRef)[NUM_EDGES][NUM_EDGES][2] = zobristEdgeActual;
const uint64_t (&zobristCornerRef)[NUM_CORNERS][NUM_CORNERS][3] = zobristCornerActual;

vector<vector<int>> edge_regions;
vector<vector<int>> corner_regions;
	
unordered_map<int, int> edge_map;
unordered_map<int, int> corner_map;

vector<string> inverse_edge_map;
vector<string> inverse_corner_map;

//given a char return int value of the corresponding face
Face Cube::charToFace(char c)
{
	switch(c)
	{
		case 'U': return UP;
		case 'L': return LEFT;
		case 'F': return FRONT;
		case 'R': return RIGHT;
		case 'B': return BACK;
		case 'D': return DOWN;
		default:
			throw invalid_argument("c was not U, L, F, R, B or D");
	}
}

//given a string return int value of the corresponding edge
Edge Cube::stringToEdge(const string& s)
{	
	if (s == "UF") return UF;
	if (s == "UR") return UR;
	if (s == "UB") return UB;
	if (s == "UL") return UL;
	if (s == "FL") return FL;
	if (s == "FR") return FR;
	if (s == "BR") return BR;
	if (s == "BL") return BL;
	if (s == "DF") return DF;
	if (s == "DR") return DR;
	if (s == "DB") return DB;
	if (s == "DL") return DL;
	
	throw invalid_argument("s was not a valid edge");
}

//given a string return int value of the corresponding corner
Corner Cube::stringToCorner(const string& s)
{
	if (s == "UFL") return UFL;
	if (s == "UFR") return UFR;
	if (s == "UBR") return UBR;
	if (s == "UBL") return UBL;
	if (s == "DFL") return DFL;
	if (s == "DFR") return DFR;
	if (s == "DBR") return DBR;
	if (s == "DBL") return DBL;
	
	throw invalid_argument("s was not a valid corner");
}

//given a string return int value of the corresponding move
Moves Cube::stringToMoves(const string& s)
{
	if (s == "U") return U;
	if (s == "U'") return UPrime;
	if (s == "U2") return UDouble;
	if (s == "L") return L;
	if (s == "L'") return LPrime;
	if (s == "L2") return LDouble;
	if (s == "F") return F;
	if (s == "F'") return FPrime;
	if (s == "F2") return FDouble;
	if (s == "R") return R;
	if (s == "R'") return RPrime;
	if (s == "R2") return RDouble;
	if (s == "B") return B;
	if (s == "B'") return BPrime;
	if (s == "B2") return BDouble;
	if (s == "D") return D;
	if (s == "D'") return DPrime;
	if (s == "D2") return DDouble;
	
	throw invalid_argument("s was not a valid move");
}

Cube::Cube() = default;

//construct a cube from a string
Cube::Cube(string s)
{	
	//check to see it's divisible by the number of faces
	if (s.length() % NUM_FACES != 0) {
		cerr << "string not divisible by 6" << endl;
	}
	
	//check to see that it's length is right
	if (s.length() != E_PER_FACE * NUM_FACES) {
		cerr << "string isn't made of perfect squares" << endl;
	}
	
	stringToCube(s);
}

//given a string update the cube's array representation
void Cube::stringToCube(string s)
{
	//convert the string to a 3d vector for easier parsing
	vector<vector<vector<char>>> faces(NUM_FACES);
	
	vector<int> temp;
	char c1 = ' ';
	char c2 = ' ';
	char c3 = ' ';
	int loc = 0;
	int orie = 0;
	//values for the E slice
	int middle_beginning = 4 * (E_PER_SIDE - 2);
	int middle_ending = 8 * (E_PER_SIDE - 2);
	
	//parse the string into the 3d vector
	for (size_t i = 0; i < NUM_FACES; i++) {
		faces[i].resize(E_PER_SIDE);
		for (size_t j = 0; j < E_PER_SIDE; j++) {
			faces[i][j].resize(E_PER_SIDE);
			for (size_t k = 0; k < E_PER_SIDE; k++) {
				faces[i][j][k] = s[i * E_PER_FACE + j * E_PER_SIDE + k];
			}
		}
	}
	
	//parse all edges into array representation
	for (size_t i = 0; i < edge_regions.size(); i++) {
		temp = edge_regions[i];
		c1 = faces[temp[0]][temp[1]][temp[2]];
		c2 = faces[temp[3]][temp[4]][temp[5]];
		
		//edge_map returns a value to where it wants to be on the cube
		loc = edge_map[(charToFace(c1) + 1) * (charToFace(c2) + 1)];
		
		//debug statement
		//cout << i << ": " << c1 << c2 << " or "  << (charToFace(c1) + 1) * (charToFace(c2) + 1) <<" maps to " << loc << endl;
		
		//checks if primary colour is on the U or D face or on the F or B face if in the E slice
		if (middle_beginning <= loc && loc < middle_ending) {
			if (c1 == 'F' || c1 == 'B') {
				orie = 0;
			}
			else {
				orie = 1;
			}
		}
		else {
			if (c1 == 'U' || c1 == 'D') {
				orie = 0;
			}
			else {
				orie = 1;
			}
		}
		
		this -> el[i] = loc;
		this -> eo[i] = orie;
	}
	
	//parse corners into array representation
	for (size_t i = 0; i < corner_regions.size(); i++) {
		temp = corner_regions[i];
		c1 = faces[temp[0]][temp[1]][temp[2]];
		c2 = faces[temp[3]][temp[4]][temp[5]];
		c3 = faces[temp[6]][temp[7]][temp[8]];
		
		//corner_map returns where the corner wants to be on a solved cube
		loc = corner_map[(charToFace(c1) + 1) * (charToFace(c2) + 1) * (charToFace(c3) + 1)];

		//debug statement
		//cout << i << ": " << c1 << c2 << c3 << " maps to " << loc << endl;

		//handles orientation parsing logic
		if (c1 == 'U' || c1 == 'D') {
			orie = 0;
		}
		else if (c2 == 'U' || c2 == 'D') {
			orie = 1;
		}
		else {
			orie = 2;
		}
		
		this -> cl[i] = loc;
		this -> co[i] = orie;
	}
}

//returns the string representation for a calling cube
string Cube::getStringKey()
{
	string s = "";
	s.reserve(NUM_FACES * E_PER_FACE);
	
	string temp = "";
	string faces = "ULFRBD";
	
	//3d char array for easy building of the string
	char cube_temp[NUM_FACES][E_PER_SIDE][E_PER_SIDE];
	vector<int> coords;
	
	//reverses edges
	for (size_t i = 0; i < NUM_EDGES; i++) {
		temp = inverse_edge_map[el[i]];
		coords = edge_regions[i];
				
		cube_temp[coords[0]][coords[1]][coords[2]] = temp[eo[i]];
		cube_temp[coords[3]][coords[4]][coords[5]] = temp[(eo[i] + 1) % 2];
	}

	//reverses strings
	for (size_t i = 0; i < NUM_CORNERS; i++) {
		temp = inverse_corner_map[cl[i]];
		coords = corner_regions[i];
		for (int r = 0; r < 3; r++) {
			int face = coords[3 * r];
			int row = coords[3 * r + 1];
			int col = coords[3 * r + 2];
			
			int index = (r + 3 - co[i]) % 3;
			cube_temp[face][row][col] = temp[index];
		}
	}

	//takes reversed info and puts it into the string
	for (size_t i = 0; i < NUM_FACES; i++) {
		for (size_t j = 0; j < E_PER_SIDE; j++) {
			for (size_t k = 0; k < E_PER_SIDE; k++) {
				if (j != 1 || k != 1) {
					s += cube_temp[i][j][k];
				}
				else {
					s += faces[i];
				}
			}
		}
	}
		
	return s;
}

array<uint8_t, NUM_EDGES> Cube::getEO() const
{
	return eo;
}

array<uint8_t, NUM_EDGES> Cube::getEL() const
{
	return el;
}

array<uint8_t, NUM_CORNERS> Cube::getCO() const
{
	return co;
}

array<uint8_t, NUM_CORNERS> Cube::getCL() const
{
	return cl;
}

void Cube::setEO(array<uint8_t, NUM_EDGES>& eo)
{
	for (size_t i = 0; i < NUM_EDGES; i++) {
		this -> eo[i] = eo[i];
	}
}

void Cube::setEL(array<uint8_t, NUM_EDGES>& el)
{
	for (size_t i = 0; i < NUM_EDGES; i++) {
		this -> el[i] = el[i];
	}
}

void Cube::setCO(array<uint8_t, NUM_CORNERS>& co)
{
	for (size_t i = 0; i < NUM_CORNERS; i++) {
		this -> co[i] = co[i];
	}
}

void Cube::setCL(array<uint8_t, NUM_CORNERS>& cl)
{
	for (size_t i = 0; i < NUM_CORNERS; i++) {
		this -> cl[i] = cl[i];
	}
}

//generates the regions used to parse and reverse string and cube representations
//edge regions use for loops as cubes can have varying edges but only 8 corners
void Cube::generateRegions(int size)
{	
	int iterations = size - 2;
	int top = 0;
	int bottom = size - 1;
	int left = 0;
	int right = size - 1;
	
	//creates the U face edge regions
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({0, bottom, 1 + i, 2, top, 1 + i});
	}
	
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({0, size - 2 - i, right, 3, top, 1 + i});
	}
	
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({0, top, size - 2 - i, 4, top, 1 + i});
	}
	
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({0, 1 + i, left, 1, top, 1 + i});
	}
	
	//creates the E slice edge regions
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({2, 1 + i, left, 1, 1 + i, right});
	}
	
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({2, 1 + i, right, 3, 1 + i, left});
	}
	
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({4, 1 + i, left, 3, 1 + i, right});
	}
	
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({4, 1 + i, right, 1, 1 + i, left});
	}

	//creates the D face edge regions
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({5, top, 1 + i, 2, bottom, 1 + i});
	}
	
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({5, 1 + i, right, 3, bottom, 1 + i});		
	}
	
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({5, bottom, size - 2 - i, 4, bottom, 1 + i});
	}
	
	for (int i = 0; i < iterations; i++) {
		edge_regions.push_back({5, size - 2 - i, left, 1, bottom, 1 + i});		
	}
	
	//creates corner regions
	corner_regions.push_back({0, bottom, left, 1, top, right, 2, top, left});
	corner_regions.push_back({0, bottom, right, 2, top, right, 3, top, left});
	corner_regions.push_back({0, top, right, 3, top, right, 4, top, left});
	corner_regions.push_back({0, top, left, 4, top, right, 1, top, left});
	
	corner_regions.push_back({5, top, left, 2, bottom, left, 1, bottom, right});
	corner_regions.push_back({5, top, right, 3, bottom, left, 2, bottom, right});
	corner_regions.push_back({5, bottom, right, 4, bottom, left, 3, bottom, right});
	corner_regions.push_back({5, bottom, left, 1, bottom, left, 4, bottom, right});
}

//generates mappings for edge/corner string <-> array representation of that edge/corner
void Cube::generateMaps()
{
	//mapping edge string to array representation
	edge_map[(charToFace('U') + 1) * (charToFace('F') + 1)] = 0;
	edge_map[(charToFace('U') + 1) * (charToFace('R') + 1)] = 1;
	edge_map[(charToFace('U') + 1) * (charToFace('B') + 1)] = 2;
	edge_map[(charToFace('U') + 1) * (charToFace('L') + 1)] = 3;
	
	edge_map[(charToFace('F') + 1) * (charToFace('L') + 1)] = 4;
	edge_map[(charToFace('F') + 1) * (charToFace('R') + 1)] = 5;
	edge_map[(charToFace('B') + 1) * (charToFace('R') + 1)] = 6;
	edge_map[(charToFace('B') + 1) * (charToFace('L') + 1)] = 7;
	
	edge_map[(charToFace('D') + 1) * (charToFace('F') + 1)] = 8;
	edge_map[(charToFace('D') + 1) * (charToFace('R') + 1)] = 9;
	edge_map[(charToFace('D') + 1) * (charToFace('B') + 1)] = 10;
	edge_map[(charToFace('D') + 1) * (charToFace('L') + 1)] = 11;
	
	//mapping array representation to edge strings
	inverse_edge_map.resize(12);
	
	inverse_edge_map[0] = "UF";
	inverse_edge_map[1] = "UR";
	inverse_edge_map[2] = "UB";
	inverse_edge_map[3] = "UL";
	
	inverse_edge_map[4] = "FL";
	inverse_edge_map[5] = "FR";
	inverse_edge_map[6] = "BR";
	inverse_edge_map[7] = "BL";
	
	inverse_edge_map[8] = "DF";
	inverse_edge_map[9] = "DR";
	inverse_edge_map[10] = "DB";
	inverse_edge_map[11] = "DL";
	
	//mapping corner string to array representation
	corner_map[(charToFace('U') + 1) * (charToFace('F') + 1) * (charToFace('L') + 1)] = 0;
	corner_map[(charToFace('U') + 1) * (charToFace('F') + 1) * (charToFace('R') + 1)] = 1;
	corner_map[(charToFace('U') + 1) * (charToFace('B') + 1) * (charToFace('R') + 1)] = 2;
	corner_map[(charToFace('U') + 1) * (charToFace('B') + 1) * (charToFace('L') + 1)] = 3;
	
	corner_map[(charToFace('D') + 1) * (charToFace('F') + 1) * (charToFace('L') + 1)] = 4;
	corner_map[(charToFace('D') + 1) * (charToFace('F') + 1) * (charToFace('R') + 1)] = 5;
	corner_map[(charToFace('D') + 1) * (charToFace('B') + 1) * (charToFace('R') + 1)] = 6;
	corner_map[(charToFace('D') + 1) * (charToFace('B') + 1) * (charToFace('L') + 1)] = 7;
	
	//mapping array representation to corner string
	inverse_corner_map.resize(8);
	
	inverse_corner_map[0] = "ULF";
	inverse_corner_map[1] = "UFR";
	inverse_corner_map[2] = "URB";
	inverse_corner_map[3] = "UBL";
	
	inverse_corner_map[4] = "DFL";
	inverse_corner_map[5] = "DRF";
	inverse_corner_map[6] = "DBR";
	inverse_corner_map[7] = "DLB";
}

//prints the current cube representation for debugging
void Cube::printInfo()
{
	cout << endl << "======== printing edges ========" << endl;
	
	for (size_t i = 0; i < NUM_EDGES; i++) {
		cout << "Slot " << (int) i << " contains edge: " << (int) el[i] << " which has orientation: " << (int)eo[i] << endl;
	}
	
	cout << endl << "======== printing corners ========" << endl;
	
	for (size_t i = 0; i < NUM_CORNERS; i++) {
		cout << "Slot " << (int) i << " contains corner: " << (int) cl[i] << " which has orientation: " << (int)co[i] << endl;
	}
	
	cout << endl;
}

//generate zobrist values
void Cube::generateZobrists()
{
	mt19937_64 gen(ZOBRIST_SEED);

	for (size_t i = 0; i < NUM_EDGES; i++) {
		for (size_t j = 0; j < NUM_EDGES; j++) {
			for (int k = 0; k < 2; k++) {
				zobristEdgeActual[i][j][k] = gen();
			}
		}
	}

	for (size_t i = 0; i < NUM_CORNERS; i++) {
		for (size_t j = 0; j < NUM_CORNERS; j++) {
			for (int k = 0; k < 3; k++) {
				zobristCornerActual[i][j][k] = gen();
			}
		}
	}
}

//generate factorial values
void Cube::generateFactorials()
{
	factorials[0] = 1;
	for (size_t i = 1; i <= NUM_EDGES; i++) {
		factorials[i] = factorials[i - 1] * i;
	}
}

//generate choose function values
void Cube::generateChoose()
{
	for (size_t i = 0; i < NUM_EDGES; i++) {
		for (size_t j = 0; j < 5; j++) {
			if (j <= i) {
				choose[i][j] = factorials[i] / (factorials[j] * factorials[i - j]);
			}
			else {
				choose[i][j] = 0;
			}
			//cout << "C(" << static_cast<int>(i) << "," << static_cast<int>(j) << ") = " << static_cast<int>(choose[i][j]) << endl;
		}
		//cout << endl;
	}
}

//creates the rotation tables for rotating the cube
void Cube::moveTableGenerate()
{
	//getting Moves, Edge and Corner integer values from enums
	Moves U = stringToMoves("U");
	Moves UP = stringToMoves("U'");
	Moves UD = stringToMoves("U2");
	
	Moves L = stringToMoves("L");
	Moves LP = stringToMoves("L'");
	Moves LD = stringToMoves("L2");
	
	Moves F = stringToMoves("F");
	Moves FP = stringToMoves("F'");
	Moves FD = stringToMoves("F2");
	
	Moves R = stringToMoves("R");
	Moves RP = stringToMoves("R'");
	Moves RD = stringToMoves("R2");
	
	Moves B = stringToMoves("B");
	Moves BP = stringToMoves("B'");
	Moves BD = stringToMoves("B2");
	
	Moves D = stringToMoves("D");
	Moves DP = stringToMoves("D'");
	Moves DD = stringToMoves("D2");
	
	
	Edge UF = stringToEdge("UF");
	Edge UR = stringToEdge("UR");
	Edge UB = stringToEdge("UB");
	Edge UL = stringToEdge("UL");
	
	Edge FL = stringToEdge("FL");
	Edge FR = stringToEdge("FR");
	Edge BR = stringToEdge("BR");
	Edge BL = stringToEdge("BL");
	
	Edge DF = stringToEdge("DF");
	Edge DR = stringToEdge("DR");
	Edge DB = stringToEdge("DB");
	Edge DL = stringToEdge("DL");
	
	
	Corner UFL = stringToCorner("UFL");
	Corner UFR = stringToCorner("UFR");
	Corner UBR = stringToCorner("UBR");
	Corner UBL = stringToCorner("UBL");
	
	Corner DFL = stringToCorner("DFL");
	Corner DFR = stringToCorner("DFR");
	Corner DBR = stringToCorner("DBR");
	Corner DBL = stringToCorner("DBL");
	
	//creating edge rotation tables
	moveGenHelpEdges({UF, UR, UB, UL}, U);
	moveGenHelpEdges({UF, UL, UB, UR}, UP);
	moveGenHelpEdges({UF, UB, UL, UR}, UD);

	moveGenHelpEdges({UL, BL, DL, FL}, L);
	moveGenHelpEdges({UL, FL, DL, BL}, LP);
	moveGenHelpEdges({UL, DL, FL, BL}, LD);

	moveGenHelpEdges({UF, FL, DF, FR}, F);
	moveGenHelpEdges({UF, FR, DF, FL}, FP);
	moveGenHelpEdges({UF, DF, FR, FL}, FD);

	moveGenHelpEdges({UR, FR, DR, BR}, R);
	moveGenHelpEdges({UR, BR, DR, FR}, RP);
	moveGenHelpEdges({UR, DR, BR, FR}, RD);

	moveGenHelpEdges({UB, BR, DB, BL}, B);
	moveGenHelpEdges({UB, BL, DB, BR}, BP);
	moveGenHelpEdges({UB, DB, BL, BR}, BD);

	moveGenHelpEdges({DF, DL, DB, DR}, D);
	moveGenHelpEdges({DF, DR, DB, DL}, DP);
	moveGenHelpEdges({DF, DB, DR, DL}, DD);

	//creating corner rotation tables
	moveGenHelpCorners({UBL, UFL, UFR, UBR}, {0, 0, 0, 0}, U);
	moveGenHelpCorners({UBL, UBR, UFR, UFL}, {0, 0, 0, 0}, UP);
	moveGenHelpCorners({UBL, UFR, UBR, UFL}, {0, 0, 0, 0}, UD);

	moveGenHelpCorners({UBL, DBL, DFL, UFL}, {2, 1, 2, 1}, L);
	moveGenHelpCorners({UBL, UFL, DFL, DBL}, {2, 1, 2, 1}, LP);
	moveGenHelpCorners({UBL, DFL, UFL, DBL}, {0, 0, 0, 0}, LD);

	moveGenHelpCorners({UFL, DFL, DFR, UFR}, {2, 1, 2, 1}, F);
	moveGenHelpCorners({UFL, UFR, DFR, DFL}, {2, 1, 2, 1}, FP);
	moveGenHelpCorners({UFL, DFR, UFR, DFL}, {0, 0, 0, 0}, FD);

	moveGenHelpCorners({UFR, DFR, DBR, UBR}, {2, 1, 2, 1}, R);
	moveGenHelpCorners({UFR, UBR, DBR, DFR}, {2, 1, 2, 1}, RP);
	moveGenHelpCorners({UFR, DBR, UBR, DFR}, {0, 0, 0, 0}, RD);

	moveGenHelpCorners({UBR, DBR, DBL, UBL}, {2, 1, 2, 1}, B);
	moveGenHelpCorners({UBR, UBL, DBL, DBR}, {2, 1, 2, 1}, BP);
	moveGenHelpCorners({UBR, DBL, UBL, DBR}, {0, 0, 0, 0}, BD);

	moveGenHelpCorners({DFL, DBL, DBR, DFR}, {0, 0, 0, 0}, D);
	moveGenHelpCorners({DFL, DFR, DBR, DBL}, {0, 0, 0, 0}, DP);
	moveGenHelpCorners({DFL, DBR, DFR, DBL}, {0, 0, 0, 0}, DD);
}

//creates edge rotation tables
void Cube::moveGenHelpEdges(vector<int> faces, int move)
{
	uint8_t flip_val = 0;
	
	//records in half_turn if this move is a half turn like F2, R2 etc.
	if ((move + 1) % 3 == 0) {
		half_turn[move] = true;
	}
	
	//records the values given into the rotation tables
	for (size_t i = 0; i < faces.size(); i++) {
		moveTableEL[move][i] = faces[i];
		moveTableEO[move][i] = flip_val;
	}
}

//creates corner rotation tables
void Cube::moveGenHelpCorners(vector<int> faces, vector<int> flips, int move)
{
	//records given values into corner rotation tables
	for (size_t i = 0; i < faces.size(); i++) {
		moveTableCL[move][i] = faces[i];
		moveTableCO[move][i] = flips[i];
	}
}

//rotates the function
void Cube::rotate(int move)
{
	//pointers to the relevant arrays
	uint8_t* cl_info = moveTableCL[move];
	uint8_t* co_info = moveTableCO[move];
	
	uint8_t* el_info = moveTableEL[move];
	uint8_t o_info = moveTableEO[move][0];
	
	uint8_t temp_l = 0;
	uint8_t temp_o = 0;
	
	//for non half turns we need to update orientation
	if (!half_turn[move]) {
		//cycles through edges and records values to their new locations
		temp_l = el[el_info[0]];
		temp_o = eo[el_info[0]];
		
		#pragma GCC unroll 3
		for (int i = 0; i < 3; i++) {
			el[el_info[i]] = el[el_info[i + 1]];
			eo[el_info[i]] = eo[el_info[i + 1]] ^ o_info;
		}
		
		el[el_info[3]] = temp_l;
		eo[el_info[3]] = temp_o ^ o_info;

		//cycles through corners and records values to their new locations
		temp_l = cl[cl_info[0]];
		temp_o = co[cl_info[0]];

		#pragma GCC unroll 3
		for (int i = 0; i < 3; i++) {
			cl[cl_info[i]] = cl[cl_info[i + 1]];
			co[cl_info[i]] = (co[cl_info[i + 1]] + co_info[i + 1]) % 3;
		}
		
		cl[cl_info[3]] = temp_l;
		co[cl_info[3]] = (temp_o + co_info[0]) % 3;
	}
	//for half turns no orientation updates are needed also cycles are different
	else {
		//cycle between 2 opposing edges and record new values
		#pragma GCC unroll 2
		for (int i = 0; i < 2; i++) {
			temp_l = el[el_info[i * 2]];
			temp_o = eo[el_info[i * 2]];
			
			el[el_info[i * 2]] = el[el_info[i * 2 + 1]];
			eo[el_info[i * 2]] = eo[el_info[i * 2 + 1]];
			
			el[el_info[i * 2 + 1]] = temp_l;
			eo[el_info[i * 2 + 1]] = temp_o;
		}
		
		//cycle between 2 opposing corners and record new values
		#pragma GCC unroll 2
		for (int i = 0; i < 2; i++) {
			temp_l = cl[cl_info[i * 2]];
			temp_o = co[cl_info[i * 2]];
			
			cl[cl_info[i * 2]] = cl[cl_info[i * 2 + 1]];
			co[cl_info[i * 2]] = co[cl_info[i * 2 + 1]];
			
			cl[cl_info[i * 2 + 1]] = temp_l;
			co[cl_info[i * 2 + 1]] = temp_o;
		}
	}
}

//rotate(int) wrapper but for strings
void Cube::rotate(string move)
{
	rotate(stringToMoves(move));
}

//checks if a cube is solved, all edges and corners match edge and corner locations
//and orientation is 0, (el[i] = i, el[i] = 0,  0 <= i < NUM_EDGES, cl[j] = j, co[j] = 0, 0 <= j < NUM_CORNERS
bool Cube::isSolved()
{
	//this allows for debug statement to print all mismatches and not just the first mismatch
	bool ans = true;
	
	for (size_t i = 0; i < NUM_EDGES; i++) {
		if (eo[i] != 0 || el[i] != i) {
			ans = false;
			//debug statement
			//cout << "edge: " << (int) i << " has orientation: " << (int) eo[i] << " and location: " << (int) el[i] << endl;
		}
	}
	
	for (size_t i = 0; i < NUM_CORNERS; i++) {
		if (co[i] != 0 || cl[i] != i) {
			ans = false;
			//debug statement
			//cout << "corner: " << (int) i << " has orientation: " << (int) co[i] << " and location: " << (int) cl[i] << endl;
		}
	}
	
	return ans;
}

//checks if a cube has completed phase 1 of the solved
//all UD edges are on the U or D faces, E slice edges are
//in the E slice and all edges and corners have orientation 0
bool Cube::phase1Solved()
{
	//allows for debug statement to print all mismatches
	bool ans = true;
	
	for (size_t i = 0; i < NUM_EDGES; i++) {
		if (eo[i] != 0) {
			ans = false;
			//debug statement for edge not having orientation 0
			//cout << "edge: " << (int) i << " has orientation: " << (int) eo[i] << endl;
		}
		
		if (i < 4 || 7 < i) {
			if (4 <= el[i] && el[i] <= 7) {
				ans = false;
				//debug statement for E slice edge not being in the E slice
				//cout << "edge: " << (int) i << " has location: " << (int) el[i] << endl;
			}
		}
		else {
			if (el[i] < 4 || 7 < el[i]) {
				ans = false;
				//debug statement for UD slice edge being in the E slice
				//cout << "edge: " << (int) i << " has location: " << (int) el[i] << endl;
			}
		}
	}
	
	for (size_t i = 0; i < NUM_CORNERS; i++) {
		if (co[i] != 0) {
			ans = false;
			//debug statement for corner not having orientation 0
			//cout << "corner: " << (int) i << " has orientation: " << (int) co[i] << endl;
		}
	}
	
	return ans;
}

//same behaviour as phase1Solved but for coordinates rather then a cube
bool Cube::phase1Solved(uint16_t eoKey, uint16_t coKey, uint16_t eKey)
{
	//all these coordinates mean phase 1 is complete
	return eoKey == 0 && coKey == 0 && eKey == 69;
}

//calculates zobrist key of a cube
uint64_t Cube::getZobrist()
{
	uint64_t key = 0;
	
	for (size_t i = 0; i < NUM_EDGES; i++) {
		key ^= zobristEdgeActual[i][el[i]][eo[i]];
	}
	
	for (size_t i = 0; i < NUM_CORNERS; i++) {
		key ^= zobristCornerActual[i][cl[i]][co[i]];
	}
	
	return key;
}

//indexer for edge orientation of phase 1, converts the first 11 values of
//eo array into a binary string and converts to decimal
uint16_t Cube::p1eoIndex()
{
	uint16_t key = 0;
	
	for (size_t i = 0; i < NUM_EDGES - 1; i++) {
		key = key * 2 + eo[i];
	}
	
	return key;
}

//indexer for corner orientation of phase 1, convert the first 7 values of
//co array into a trinary string and converts to decimal
uint16_t Cube::p1coIndex()
{
	uint16_t key = 0;
	
	for (size_t i = 0; i < NUM_CORNERS - 1; i++) {
		key = key * 3 + co[i];
	}
	
	return key;
}

//indexer for e slice, takes the locations of the 4 E slice edges, sorts them
//then runs the sum of C(el[i], 4 - i), where i is an E slice edge (combinatorial index)
uint16_t Cube::p1eSliceIndex()
{
	uint16_t key = 0;
	
	int count = 4;
	
	for (int i = 11; i >= 0 && count > 0; i--) {
		if (el[i] >= 4 && el[i] <= 7) {
			key += choose[i][count];
			count--;
		}
	}
	
	return key;
}

//updates a cube given an eo key
void Cube::p1eoReverse(uint16_t key)
{
	uint8_t count = 0;
	for (int i = 10; i >= 0; i--) {
		eo[i] = key % 2;
		count += eo[i];
		key /= 2;
	}
	
	eo[11] = count % 2;
}

//updates a cube given a co key
void Cube::p1coReverse(uint16_t key)
{
	uint8_t count = 0;
	
	for (int i = 6; i >= 0; i--) {
		co[i] = key % 3;
		count += co[i];
		key /= 3;
	}
	
	co[7] = (3 - count % 3) % 3;
}

//updates a cube given an eSlice key, however this makes assumptions of the edges appear in ascending order
//do not rely on this method to preserve order of E slice!!!
void Cube::p1eSliceReverse(uint16_t key)
{
    int count = 4;
	int piece = 4;
	
	for (int i = 11; i >= 0 && piece <= 7; i--) {
		if (choose[i][count] <= key) {
			key -= choose[i][count];
			count--;
			el[i] = piece;
			piece++;
		}
	}
}

//uses Lehmer codes to index the UD edge locations for phase 2
uint16_t Cube::p2elIndex()
{
	uint16_t key = 0;
	int count = 0;
	for (size_t i = 0; i < NUM_EDGES; i++) {
		count = 0;
		
		//skip E slice
		if (4 <= i && i <= 7) {
			continue;
		}
		
		for (size_t j = i + 1; j < NUM_EDGES; j++) {
			//skip E slice
			if (4 <= j && j <= 7) {
				continue;
			}

			if (el[j] < el[i]) {
				count++;
			}
			
		}
		
		if (i < 4) {
			key += count * factorials[NUM_EDGES - 1 - i - 4];
		}
		else {
			key += count * factorials[NUM_EDGES - 1 - i];
		}
	}
	
	return key;
}

//uses Lehmer codes to index the corner location for phase 2
uint16_t Cube::p2clIndex()
{
	uint16_t key = 0;
	int count = 0;
	for (size_t i = 0; i < NUM_CORNERS; i++) {
		count = 0;

		for (size_t j = i + 1; j < NUM_CORNERS; j++) {
			if (cl[j] < cl[i]) {
				count++;
			}
		}
		key += count * factorials[NUM_CORNERS - 1 - i];
	}
	
	return key;
}

//uses Lehmer codes to index E slice edges for phase 2
uint8_t Cube::p2eSliceIndex()
{
	uint8_t key = 0;
	int count = 0;
	for (size_t i = 4; i <= 7; i++) {
		count = 0;
		
		for (size_t j = i + 1; j <= 7; j++) {
			if (el[j] < el[i]) {
				count++;
			}
		}
		key += count * factorials[7 - i];
	}
	
	return key;
}

//updates the cube given an el key
void Cube::p2elReverse(uint16_t key)
{
	vector<int> edges = {0, 1, 2, 3, 8, 9, 10, 11};
	
	for (int i = 0; i < 4; i++) {
		el[i] = edges[key/factorials[7 - i]];
		key = key % factorials[7 - i];
		edges.erase(find(edges.begin(), edges.end(), el[i]));
	}
	
	for (int i = 8; i < 12; i++) {
		el[i] = edges[key/factorials[11 - i]];
		key = key % factorials[11 - i];
		edges.erase(find(edges.begin(), edges.end(), el[i]));
	}
}

//update the cube given a cl key
void Cube::p2clReverse(uint16_t key)
{
	vector<int> corners = {0, 1, 2, 3, 4, 5, 6, 7};
	
	for (size_t i = 0; i < NUM_CORNERS; i++) {
		cl[i] = corners[key/factorials[NUM_CORNERS - 1 - i]];
		key = key % factorials[NUM_CORNERS - 1 - i];
		corners.erase(find(corners.begin(), corners.end(), cl[i]));
	}
}

//update the cube given an eSlice p2 key
void Cube::p2eSliceReverse(uint8_t key)
{
	vector<int> edges = {4, 5, 6, 7};
	
	for (int i = 4; i < 8; i++) {
		el[i] = edges[key/factorials[7 - i]];
		key = key % factorials[7 - i];
		edges.erase(find(edges.begin(), edges.end(), el[i]));
	}
}

//given a scrambleSeed, and the number of cubes this generates a big vector of scrambled cubes
void Cube::generateCubes(vector<Cube>& cubes, uint32_t scrambleSeed, int num_cubes)
{
	mt19937 gen(scrambleSeed);
	uniform_int_distribution<int> dist(0, NUM_MOVES - 1);
	
	//adjust rotations_per_cube to the desired number of rotations you apply to a cube
	int rotations_per_cube = 40;
	int curr_move = -10;
	int last_move = -10;
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (int i = 0; i < num_cubes; i++) {
		Cube toDo = cube;
		curr_move = -10;
		last_move = -10;
		for (int j = 0; j < rotations_per_cube; j++) {
			curr_move = dist(gen);
			//ensures we don't apply same face moves in a row like F then F2 or R and R' etc.
			while (curr_move/3 == last_move/3) {
				curr_move = dist(gen);
			}
			toDo.rotate(curr_move);
			last_move = curr_move;
		}
		cubes.push_back(toDo);
	}
}

//scrambles the calling cube given a seed
void Cube::scramble(uint32_t seed)
{
	mt19937 gen(seed);
	uniform_int_distribution<int> dist(0, NUM_MOVES - 1);
	
	//adjust rotations_per_cube to the desired number of rotations to scramble
	int rotations_per_cube = 40;
	int curr_move = -10;
	int last_move = -10;
	
	for (int i = 0; i < rotations_per_cube; i++) {
		curr_move = dist(gen);
		while (curr_move/3 == last_move/3) {
			curr_move = dist(gen);
		}
		rotate(curr_move);
		last_move = curr_move;
	}
}

//initializes all tables to make Cube objects functional
void Cube::initialize()
{
	//without this calling Cube::initialize() more then once like in the tests will break the program
	static bool guard = false;

	if (!guard) {
		guard = true;
		moveTableGenerate();
		generateFactorials();
		generateChoose();
		generateZobrists();
		generateRegions(E_PER_SIDE);
		generateMaps();
	}
}

