//this function handles loading, saving and generating transition and heuristic tables
//note edge orientation, corner orientation, edge location, corner location and p2eSlice tables aren't being used, but are commented out
//in order to allow for the decreasing in memory usage but increasing solve times

#pragma once

#include <fstream>
#include <iostream>
#include <filesystem>

#include "Cube.h"

//file names for transition and heuristic tables
constexpr const char* eoTransitionTableFileName = "tables/eoTransitionTable.bin";
constexpr const char* coTransitionTableFileName = "tables/coTransitionTable.bin";
constexpr const char* eTransitionTableFileName = "tables/eTransitionTable.bin";

/*constexpr const char* eoHeuristicTableFileName = "tables/eoHeuristicTable.bin";
constexpr const char* coHeuristicTableFileName = "tables/coHeuristicTable.bin";*/
constexpr const char* eHeuristicTableFileName = "tables/eHeuristicTable.bin";

constexpr const char* elTransitionTableFileName = "tables/elTransitionTable.bin";
constexpr const char* clTransitionTableFileName = "tables/clTransitionTable.bin";
constexpr const char* ep2TransitionTableFileName = "tables/ep2TransitionTable.bin";

/*constexpr const char* elHeuristicTableFileName = "tables/elHeuristicTable.bin";
constexpr const char* clHeuristicTableFileName = "tables/clHeuristicTable.bin";
constexpr const char* ep2HeuristicTableFileName = "tables/ep2HeuristicTable.bin";*/

constexpr const char* eocoHeuristicTableFileName = "tables/eocoHeuristicTable.bin";

constexpr const char* elesliceHeuristicTableFileName = "tables/eleSliceHeuristicTable.bin";
constexpr const char* clesliceHeuristicTableFileName = "tables/cleSliceHeuristicTable.bin";

//constants for sizes of the tables needed
constexpr uint16_t CO_SIZE = 2187;
constexpr uint16_t E_SIZE = 495;
constexpr uint16_t EO_SIZE = 2048;

constexpr uint16_t EL_SIZE = 40320;
constexpr uint16_t CL_SIZE = 40320;
constexpr uint16_t E_SIZE_P2 = 24;

using namespace std;

//actual tables and read only references to the tables
extern const uint16_t eoTransitionTable[EO_SIZE][NUM_MOVES];
extern const uint16_t coTransitionTable[CO_SIZE][NUM_MOVES];
extern const uint16_t eTransitionTable[E_SIZE][NUM_MOVES];

extern const uint16_t (*eoTransitionTableRef)[NUM_MOVES];
extern const uint16_t (*coTransitionTableRef)[NUM_MOVES];
extern const uint16_t (*eTransitionTableRef)[NUM_MOVES];

//commented out old heurstic tables
/*extern const uint8_t eoHeuristicsTable[EO_SIZE];
extern const uint8_t coHeuristicsTable[CO_SIZE];*/
extern const uint8_t eHeuristicsTable[E_SIZE];

/*extern const uint8_t *eoHeuristicsTableRef;
extern const uint8_t *coHeuristicsTableRef;*/
extern const uint8_t *eHeuristicsTableRef;

extern const uint16_t elTransitionTable[EL_SIZE][NUM_MOVES_P2];
extern const uint16_t clTransitionTable[CL_SIZE][NUM_MOVES_P2];
extern const uint8_t ep2TransitionTable[E_SIZE_P2][NUM_MOVES_P2];

extern const uint16_t (*elTransitionTableRef)[NUM_MOVES_P2];
extern const uint16_t (*clTransitionTableRef)[NUM_MOVES_P2];
extern const uint8_t (*ep2TransitionTableRef)[NUM_MOVES_P2];

//commented out old heuristic tables
/*extern const uint8_t elHeuristicsTable[EL_SIZE];
extern const uint8_t clHeuristicsTable[CL_SIZE];
extern const uint8_t ep2HeuristicsTable[E_SIZE_P2];

extern const uint8_t *elHeuristicsTableRef;
extern const uint8_t *clHeuristicsTableRef;
extern const uint8_t *ep2HeuristicsTableRef;*/

extern const uint8_t eocoHeuristicTable[EO_SIZE * CO_SIZE];
extern const uint8_t *eocoHeuristicsTableRef;

extern const uint8_t cleSlicdHeuristicTable[CL_SIZE * E_SIZE_P2];
extern const uint8_t *cleSliceHeuristicsTableRef;

extern const uint8_t eleSliceHeuristicTable[EL_SIZE * E_SIZE_P2];
extern const uint8_t *eleSliceHeuristicsTableRef;

class Tables
{
	public:
		static int loadTables();
		
		static int loadEOTransitionTables();
		static int loadCOTransitionTables();
		static int loadETransitionTables();
		
		static void generateEOTransitionTables();
		static void generateCOTransitionTables();
		static void generateETransitionTables();
		
		//commented out old heuristic table functions
		/*static int loadEOHeuristicTables();
		static int loadCOHeuristicTables();*/
		static int loadEHeuristicTables();
		
		/*static void generateEOHeuristicTables();
		static void generateCOHeuristicTables();*/
		static void generateEHeuristicTables();
		
		static int loadELTransitionTables();
		static int loadCLTransitionTables();
		static int loadEP2TransitionTables();
		
		static void generateELTransitionTables();
		static void generateCLTransitionTables();
		static void generateEP2TransitionTables();
		
		//commented out old heuristic table functions
		/*static int loadELHeuristicTables();
		static int loadCLHeuristicTables();
		static int loadEP2HeuristicTables();
		
		static void generateELHeuristicTables();
		static void generateCLHeuristicTables();
		static void generateEP2HeuristicTables();*/
		
		static int loadEOCOHeuristicTables();
		static void generateEOCOHeuristicTables();
		
		static int loadCLESliceHeuristicTables();
		static void generateCLESliceHeuristicTables();
		
		static int loadELESliceHeuristicTables();
		static void generateELESliceHeuristicTables();
		
		static void saveUint8(uint8_t* table, size_t size, const string& fileName);
		static void loadUint8(uint8_t* table, size_t size, const string& fileName);

		static void saveUint16(uint16_t* table, size_t size, const string& fileName);
		static void loadUint16(uint16_t* table, size_t size, const string& fileName);
		
		static void emptyCube(Cube& cube);
};