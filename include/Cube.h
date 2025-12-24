// this cube handles representing a cube, rotating the cube, loading 
// and returning string representation and indexes used by the solver

#pragma once

#include <vector>
#include <cstdint>
#include <array>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <random>

//constants for a 3x3 cube
//how many edge faces are on a face
constexpr size_t E_PER_FACE = 9;
//how many edge faces are on one side
constexpr size_t E_PER_SIDE = 3;
constexpr size_t NUM_MOVES = 18;
constexpr size_t NUM_MOVES_P2 = 10;
constexpr size_t NUM_FACES = 6;
constexpr size_t NUM_CORNERS = 8;
constexpr size_t NUM_EDGES = 12 * (E_PER_SIDE - 2);

//zobrist key seed that's used for bfs testing
constexpr uint64_t ZOBRIST_SEED = 100;

using namespace std;

//rotation logic tables
extern uint8_t moveTableEL[NUM_MOVES][NUM_EDGES];
extern uint8_t moveTableEO[NUM_MOVES][NUM_EDGES];
extern uint8_t moveTableCL[NUM_MOVES][NUM_CORNERS];
extern uint8_t moveTableCO[NUM_MOVES][NUM_CORNERS];
extern bool half_turn[NUM_MOVES];

//precomputed values for factorials and choose functions
extern long factorials[NUM_EDGES + 1];
extern uint16_t choose[NUM_EDGES][5];

//zobrist key constants
extern const uint64_t zobristEdge[NUM_EDGES][NUM_EDGES][2];
extern const uint64_t zobristCorner[NUM_CORNERS][NUM_CORNERS][3];

extern const uint64_t (&zobristEdgeRef)[NUM_EDGES][NUM_EDGES][2];
extern const uint64_t (&zobristCornerRef)[NUM_CORNERS][NUM_CORNERS][3];

//tables to load in a cube from a string and return string representation of the cube
extern vector<vector<int>> edge_regions;
extern vector<vector<int>> corner_regions;
extern unordered_map<int, int> edge_map;
extern unordered_map<int, int> corner_map;
extern vector<string> inverse_edge_map;
extern vector<string> inverse_corner_map;

//DOWN is 6 becuse if it was 5, the DL or (1 + 1) * (5 + 1) = 12, but also FR or (2 + 1) * (3 + 1) = 12, that way there is no overlap for corners and edges
enum Face {
	UP,
	LEFT,
	FRONT,
	RIGHT,
	BACK,
	DOWN = 6
};

//order of edges from 0 - 11
enum Edge {
	UF,
	UR,
	UB,
	UL,
	FL,
	FR,
	BR,
	BL,
	DF,
	DR,
	DB,
	DL
};

//order of corners from 0 - 7
enum Corner {
	UFL,
	UFR,
	UBR,
	UBL,
	DFL,
	DFR,
	DBR,
	DBL
};

//order of moves from 0-17
enum Moves {
	U,
	UPrime,
	UDouble,
	L,
	LPrime,
	LDouble,
	F,
	FPrime,
	FDouble,
	R,
	RPrime,
	RDouble,
	B,
	BPrime,
	BDouble,
	D,
	DPrime,
	DDouble					
};

class Cube
{
	private:
		//representation of the cube
		//el[i] = val, represents that in the cube's ith slot, the edge that wants to be in slot "val" is occupying it,
		//for instance el[2] = 5 means the 2nd slot (for current implementation is UD) contains the 5th edge (UR)
		array<uint8_t, NUM_EDGES> el;
		//eo[i] = orie means the edge in the i'th slot has an orientation of "orie" so eo[2] = 0 means the 2nd slot has an orientation of 0
		array<uint8_t, NUM_EDGES> eo;
		//the same applies for cl and co
		array<uint8_t, NUM_CORNERS> cl;
		array<uint8_t, NUM_CORNERS> co;
	public:
		Cube();
		Cube(string s);
		
		void stringToCube(string s);
		string getStringKey();

		static Face charToFace(char c);
		static Edge stringToEdge(const string& s);
		static Corner stringToCorner(const string& s);
		static Moves stringToMoves(const string& s);
		
		void printInfo();
		
		static void moveGenHelpEdges(vector<int> faces, int move, bool flip);
		static void moveGenHelpCorners(vector<int> faces, vector<int> flips, int move);
		static void moveTableGenerate();
		static void generateChoose();
		static void generateFactorials();
		static void generateZobrists();
		static void generateRegions(int size);
		static void generateMaps();
		
		array<uint8_t, NUM_EDGES> getEO() const;
		array<uint8_t, NUM_EDGES> getEL() const;
		array<uint8_t, NUM_CORNERS> getCO() const;
		array<uint8_t, NUM_CORNERS> getCL() const;
		
		void setEO(array<uint8_t, NUM_EDGES>& eo);
		void setEL(array<uint8_t, NUM_EDGES>& el);
		void setCO(array<uint8_t, NUM_CORNERS>& co);
		void setCL(array<uint8_t, NUM_CORNERS>& cl);
		
		void rotate(int move);
		void rotate(string move);
		
		uint64_t getZobrist();
				
		bool isSolved();
		bool phase1Solved();
		static bool phase1Solved(uint16_t eoKey, uint16_t coKey, uint16_t eKey);
		
		uint16_t p1eoIndex();
		uint16_t p1coIndex();
		uint16_t p1eSliceIndex();
		
		void p1eoReverse(uint16_t key);
		void p1coReverse(uint16_t key);
		void p1eSliceReverse(uint16_t key);
		
		uint16_t p2elIndex();
		uint16_t p2clIndex();
		uint8_t p2eSliceIndex();
		
		void p2elReverse(uint16_t key);
		void p2clReverse(uint16_t key);
		void p2eSliceReverse(uint8_t key);
		
		static void generateCubes(vector<Cube>& cubes, uint32_t scrambleSeed, int num_cubes);
		
		void scramble(uint32_t seed);
		
		static void initialize();
};