//note that all load and generate functions have the exact same structure but just record
//tables in different places and to different files, explanations are in eo Transition Tables, and eoco Heuristic tables

#include "Tables.h"

#include <queue>

#include <unordered_set>

uint16_t eoTransitionTableActual[EO_SIZE][NUM_MOVES] = {};
uint16_t coTransitionTableActual[CO_SIZE][NUM_MOVES] = {};
uint16_t eTransitionTableActual[E_SIZE][NUM_MOVES] = {};

const uint16_t (*eoTransitionTableRef)[NUM_MOVES] = eoTransitionTableActual;
const uint16_t (*coTransitionTableRef)[NUM_MOVES] = coTransitionTableActual;
const uint16_t (*eTransitionTableRef)[NUM_MOVES] = eTransitionTableActual;

/*uint8_t eoHeuristicTableActual[EO_SIZE] = {};
uint8_t coHeuristicTableActual[CO_SIZE] = {};*/
uint8_t eHeuristicTableActual[E_SIZE] = {};

/*const uint8_t *eoHeuristicsTableRef = eoHeuristicTableActual;
const uint8_t *coHeuristicsTableRef = coHeuristicTableActual;*/
const uint8_t *eHeuristicsTableRef = eHeuristicTableActual;

uint16_t elTransitionTableActual[EL_SIZE][NUM_MOVES_P2] = {};
uint16_t clTransitionTableActual[CL_SIZE][NUM_MOVES_P2] = {};
uint8_t ep2TransitionTableActual[E_SIZE_P2][NUM_MOVES_P2] = {};

const uint16_t (*elTransitionTableRef)[NUM_MOVES_P2] = elTransitionTableActual;
const uint16_t (*clTransitionTableRef)[NUM_MOVES_P2] = clTransitionTableActual;
const uint8_t (*ep2TransitionTableRef)[NUM_MOVES_P2] = ep2TransitionTableActual;

/*uint8_t elHeuristicTableActual[EL_SIZE] = {};
uint8_t clHeuristicTableActual[CL_SIZE] = {};
uint8_t ep2HeuristicTableActual[E_SIZE_P2] = {};

const uint8_t *elHeuristicsTableRef = elHeuristicTableActual;
const uint8_t *clHeuristicsTableRef = clHeuristicTableActual;
const uint8_t *ep2HeuristicsTableRef = ep2HeuristicTableActual;*/

uint8_t eocoHeuristicTableActual[EO_SIZE * CO_SIZE] = {};
const uint8_t *eocoHeuristicsTableRef = eocoHeuristicTableActual;

uint8_t cleSliceHeuristicTableActual[CL_SIZE * E_SIZE_P2] = {};
const uint8_t *cleSliceHeuristicsTableRef = cleSliceHeuristicTableActual;

uint8_t eleSliceHeuristicTableActual[EL_SIZE * E_SIZE_P2] = {};
const uint8_t *eleSliceHeuristicsTableRef = eleSliceHeuristicTableActual;

/*
for single transition tables all functions are the exact same.
check if the specific file exists, if it does load it. otherwise
call the generator, save it, reload it in a temp table and verify that loaded and generated tables are the same
*/
int Tables::loadEOTransitionTables()
{
	int result = 0;
	
	if (filesystem::exists(eoTransitionTableFileName)) {
		loadUint16(&eoTransitionTableActual[0][0], NUM_MOVES * EO_SIZE, eoTransitionTableFileName);
	}
	else {
		//generate table
		generateEOTransitionTables();
		//save the table
		saveUint16(&eoTransitionTableActual[0][0], NUM_MOVES * EO_SIZE, eoTransitionTableFileName);
		
		//load the table into a temp table
		uint16_t temp[EO_SIZE][NUM_MOVES] = {};
		
		loadUint16(&temp[0][0], NUM_MOVES * EO_SIZE, eoTransitionTableFileName);
		
		//ensure tables are the same
		for (size_t i = 0; i < NUM_MOVES; i++) {
			for (size_t j = 0; j < EO_SIZE; j++) {
				if (temp[j][i] != eoTransitionTableActual[j][i]) {
					cout << "index at " << (int) j <<  " and " << (int) i << " doesn't match " << endl;
					result = 1;
				}
			}
		}
	}
	
	return result;
}

/*
generating functions are the exact same.
starting from key 0 to key (table size - 1)
reverse it onto the cube, rotate the cube, and record the new index in the table so that solving doesn't require rotating a cube but a table lookup
*/
void Tables::generateEOTransitionTables()
{
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (int i = 0; i < EO_SIZE; i++) {
		for (size_t j = 0; j < NUM_MOVES; j++) {
			//reverse onto the cube
			cube.p1eoReverse(i);
			//rotate the cube
			cube.rotate(j);
			//record where the new coordinate is after rotating
			eoTransitionTableActual[i][j] = cube.p1eoIndex();
		}
	}
}

int Tables::loadCOTransitionTables()
{
	int result = 0;
	
	if (filesystem::exists(coTransitionTableFileName)) {
		loadUint16(&coTransitionTableActual[0][0], NUM_MOVES * CO_SIZE, coTransitionTableFileName);
	}
	else {
		generateCOTransitionTables();
		saveUint16(&coTransitionTableActual[0][0], NUM_MOVES * CO_SIZE, coTransitionTableFileName);
		
		uint16_t temp[CO_SIZE][NUM_MOVES] = {};
		
		loadUint16(&temp[0][0], NUM_MOVES * CO_SIZE, coTransitionTableFileName);
		
		for (size_t i = 0; i < NUM_MOVES; i++) {
			for (size_t j = 0; j < CO_SIZE; j++) {
				if (temp[j][i] != coTransitionTableActual[j][i]) {
					cout << "index at " << (int) i <<  ", " << (int) j << " doesn't match " << endl;
					result = 1;
				}
			}
		}
	}
	
	return result;
}

void Tables::generateCOTransitionTables()
{
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (int i = 0; i < CO_SIZE; i++) {
		for (size_t j = 0; j < NUM_MOVES; j++) {
			cube.p1coReverse(i);
			cube.rotate(j);
			coTransitionTableActual[i][j] = cube.p1coIndex();
		}
	}
}

int Tables::loadETransitionTables()
{
	int result = 0;
	
	if (filesystem::exists(eTransitionTableFileName)) {
		loadUint16(&eTransitionTableActual[0][0], NUM_MOVES * E_SIZE, eTransitionTableFileName);
	}
	else {
		generateETransitionTables();
		saveUint16(&eTransitionTableActual[0][0], NUM_MOVES * E_SIZE, eTransitionTableFileName);
		
		uint16_t temp[E_SIZE][NUM_MOVES] = {};
		
		loadUint16(&temp[0][0], NUM_MOVES * E_SIZE, eTransitionTableFileName);
		
		for (size_t i = 0; i < NUM_MOVES; i++) {
			for (size_t j = 0; j < E_SIZE; j++) {
				if (temp[j][i] != eTransitionTableActual[j][i]) {
					cout << "index at " << (int) i <<  ", " << (int) j << " doesn't match " << endl;
					result = 1;
				}
			}
		}
	}
	
	return result;
}

//this function sets all values in cube array is 0
void Tables::emptyCube(Cube& cube)
{
	array<uint8_t, NUM_EDGES>el = {};
	array<uint8_t, NUM_EDGES>eo = {};
	array<uint8_t, NUM_CORNERS>cl = {};
	array<uint8_t, NUM_CORNERS>co = {};
	
	cube.setEL(el);
	cube.setEO(eo);
	cube.setCL(cl);
	cube.setCO(co);
}

void Tables::generateETransitionTables()
{
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	for (int i = 0; i < E_SIZE; i++) {
		for (size_t j = 0; j < NUM_MOVES; j++) {
			//set all values to 0 to ensure that mapping stays consistent
			emptyCube(cube);
			cube.p1eSliceReverse(i);
			cube.rotate(j);
			eTransitionTableActual[i][j] = cube.p1eSliceIndex();
		}
	}
	
	cout << "E transition tables done" << endl;
}

//these are old heuristic table generators and loaders, only use if you want to reduce memory but increase solve times
/*
int Tables::loadEOHeuristicTables()
{
	int result = 0;
	
	if (filesystem::exists(eoHeuristicTableFileName)) {
		loadUint8(eoHeuristicTableActual, EO_SIZE, eoHeuristicTableFileName);
	}
	else {
		generateEOHeuristicTables();
		saveUint8(eoHeuristicTableActual, EO_SIZE, eoHeuristicTableFileName);
		
		uint8_t temp[EO_SIZE] = {};
		
		loadUint8(temp, EO_SIZE, eoHeuristicTableFileName);
		
		for (size_t i = 0; i < EO_SIZE; i++) {
			if (temp[i] != eoHeuristicTableActual[i]) {
				cout << "index at " << (int) i << " doesn't match " << endl;
				result = 1;
			}
		}
	}
	
	return result;
}

void Tables::generateEOHeuristicTables()
{
	for (size_t i = 0; i < EO_SIZE; i++) {
		eoHeuristicTableActual[i] = 255;
	}
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	queue<uint16_t> toDo;
	
	int count = 1;
	int depth = 0;
	
	uint16_t key = cube.p1eoIndex();
	uint16_t temp_key = 0;
	
	eoHeuristicTableActual[key] = depth;

	toDo.push(key);
	
	cout << "EOHeuristics" << endl;
	
	while (count != EO_SIZE) {
		int n = toDo.size();
		
		cout << depth << " depth reached, found: " << count << endl;
		
		depth++;
		
		for (int i = 0; i < n && count != EO_SIZE; i++) {
			key = toDo.front();
			toDo.pop();
			
			for (size_t move = 0; move < NUM_MOVES; move++) {
				temp_key = eoTransitionTableActual[key][move];
				
				if (eoHeuristicTableActual[temp_key] == 255) {
					eoHeuristicTableActual[temp_key] = depth;
					count++;
					toDo.push(temp_key);
				}
			}
		}
	}
}

int Tables::loadCOHeuristicTables()
{
	int result = 0;
	
	if (filesystem::exists(coHeuristicTableFileName)) {
		loadUint8(coHeuristicTableActual, CO_SIZE, coHeuristicTableFileName);
	}
	else {
		generateCOHeuristicTables();
		saveUint8(coHeuristicTableActual, CO_SIZE, coHeuristicTableFileName);
		
		uint8_t temp[CO_SIZE] = {};
		
		loadUint8(temp, CO_SIZE, coHeuristicTableFileName);
		
		for (size_t i = 0; i < CO_SIZE; i++) {
			if (temp[i] != coHeuristicTableActual[i]) {
				cout << "index at " << (int) i << " doesn't match " << endl;
				result = 1;
			}
		}
	}
	
	return result;
}

void Tables::generateCOHeuristicTables()
{
	for (size_t i = 0; i < CO_SIZE; i++) {
		coHeuristicTableActual[i] = 255;
	}
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	queue<uint16_t> toDo;
	
	int count = 1;
	int depth = 0;
	
	uint16_t key = cube.p1coIndex();
	uint16_t temp_key = 0;
	
	coHeuristicTableActual[key] = depth;

	toDo.push(key);
	
	cout << "COHeuristics" << endl;
	
	while (count != CO_SIZE) {
		int n = toDo.size();
		
		cout << depth << " depth reached, found: " << count << endl;
		
		depth++;
		
		for (int i = 0; i < n && count != CO_SIZE; i++) {
			key = toDo.front();
			toDo.pop();
			
			for (size_t move = 0; move < NUM_MOVES; move++) {
				temp_key = coTransitionTableActual[key][move];
				
				if (coHeuristicTableActual[temp_key] == 255) {
					coHeuristicTableActual[temp_key] = depth;
					count++;
					toDo.push(temp_key);
				}
			}
		}
	}
}
*/

int Tables::loadEHeuristicTables()
{
	int result = 0;
	
	if (filesystem::exists(eHeuristicTableFileName)) {
		loadUint8(eHeuristicTableActual, E_SIZE, eHeuristicTableFileName);
	}
	else {
		generateEHeuristicTables();
		saveUint8(eHeuristicTableActual, E_SIZE, eHeuristicTableFileName);
		
		uint8_t temp[E_SIZE] = {};
		
		loadUint8(temp, E_SIZE, eHeuristicTableFileName);
		
		for (size_t i = 0; i < E_SIZE; i++) {
			if (temp[i] != eHeuristicTableActual[i]) {
				cout << "index at " << (int) i << " doesn't match " << endl;
				result = 1;
			}
		}
	}
	
	return result;
}

void Tables::generateEHeuristicTables()
{
	for (size_t i = 0; i < E_SIZE; i++) {
		eHeuristicTableActual[i] = 255;
	}
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	queue<uint16_t> toDo;
	
	int count = 1;
	int depth = 0;
	
	uint16_t key = cube.p1eSliceIndex();
	uint16_t temp_key = 0;
	
	eHeuristicTableActual[key] = depth;

	toDo.push(key);
	
	cout << "EHeuristics" << endl;
	
	while (count < E_SIZE) {
		int n = toDo.size();
		
		cout << depth << " depth reached, found: " << count << endl;
		
		depth++;
		
		for (int i = 0; i < n && count != E_SIZE; i++) {
			key = toDo.front();
			toDo.pop();
			
			for (size_t move = 0; move < NUM_MOVES; move++) {
				temp_key = eTransitionTableActual[key][move];
				
				if (eHeuristicTableActual[temp_key] == 255) {
					eHeuristicTableActual[temp_key] = depth;
					count++;
					toDo.push(temp_key);
				}
			}
		}
	}
	
	cout << "E heuristics done" << endl;
}

int Tables::loadELTransitionTables()
{
	int result = 0;
	
	if (filesystem::exists(elTransitionTableFileName)) {
		loadUint16(&elTransitionTableActual[0][0], NUM_MOVES_P2 * EL_SIZE, elTransitionTableFileName);
	}
	else {
		generateELTransitionTables();
		saveUint16(&elTransitionTableActual[0][0], NUM_MOVES_P2 * EL_SIZE, elTransitionTableFileName);
		
		uint16_t temp[EL_SIZE][NUM_MOVES_P2] = {};
		
		loadUint16(&temp[0][0], NUM_MOVES_P2 * EL_SIZE, elTransitionTableFileName);
		
		for (size_t i = 0; i < NUM_MOVES_P2; i++) {
			for (size_t j = 0; j < EL_SIZE; j++) {
				if (temp[j][i] != elTransitionTableActual[j][i]) {
					cout << "index at " << (int) i << " doesn't match " << endl;
					result = 1;
				}
			}
		}
	}
	
	return result;
}

void Tables::generateELTransitionTables()
{
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	vector<string> moves = {"U", "U'", "U2", "L2", "F2", "R2", "B2", "D", "D'", "D2"};
	
	for (int i = 0; i < EL_SIZE; i++) {
		for (size_t j = 0; j < moves.size(); j++) {
			cube.p2elReverse(i);
			cube.rotate(moves[j]);
			elTransitionTableActual[i][j] = cube.p2elIndex();
		}
	}
}

int Tables::loadCLTransitionTables()
{
	int result = 0;
	
	if (filesystem::exists(clTransitionTableFileName)) {
		loadUint16(&clTransitionTableActual[0][0], NUM_MOVES_P2 * CL_SIZE, clTransitionTableFileName);
	}
	else {
		generateCLTransitionTables();
		saveUint16(&clTransitionTableActual[0][0], NUM_MOVES_P2 * CL_SIZE, clTransitionTableFileName);
		
		uint16_t temp[CL_SIZE][NUM_MOVES_P2] = {};
		
		loadUint16(&temp[0][0], NUM_MOVES_P2 * CL_SIZE, clTransitionTableFileName);
		
		for (size_t i = 0; i < NUM_MOVES_P2; i++) {
			for (size_t j = 0; j < CL_SIZE; j++) {
				if (temp[j][i] != clTransitionTableActual[j][i]) {
					cout << "index at " << (int) i <<  ", " << (int) j << " doesn't match " << endl;
					result = 1;
				}
			}
		}
	}
	
	return result;
}

void Tables::generateCLTransitionTables()
{
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	vector<string> moves = {"U", "U'", "U2", "L2", "F2", "R2", "B2", "D", "D'", "D2"};
	
	for (int i = 0; i < CL_SIZE; i++) {
		for (size_t j = 0; j < moves.size(); j++) {
			cube.p2clReverse(i);
			cube.rotate(moves[j]);
			clTransitionTableActual[i][j] = cube.p2clIndex();
		}
	}
}

int Tables::loadEP2TransitionTables()
{
	int result = 0;
	
	if (filesystem::exists(ep2TransitionTableFileName)) {
		loadUint8(&ep2TransitionTableActual[0][0], NUM_MOVES_P2 * E_SIZE_P2, ep2TransitionTableFileName);
	}
	else {
		generateEP2TransitionTables();
		saveUint8(&ep2TransitionTableActual[0][0], NUM_MOVES_P2 * E_SIZE_P2, ep2TransitionTableFileName);
		
		uint8_t temp[E_SIZE_P2][NUM_MOVES_P2] = {};
		
		loadUint8(&temp[0][0], NUM_MOVES_P2 * E_SIZE_P2, ep2TransitionTableFileName);
		
		for (size_t i = 0; i < NUM_MOVES_P2; i++) {
			for (size_t j = 0; j < E_SIZE_P2; j++) {
				if (temp[j][i] != ep2TransitionTableActual[j][i]) {
					cout << "index at " << (int) i <<  ", " << (int) j << " doesn't match " << endl;
					result = 1;
				}
			}
		}
	}
	
	return result;
}

void Tables::generateEP2TransitionTables()
{
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	vector<string> moves = {"U", "U'", "U2", "L2", "F2", "R2", "B2", "D", "D'", "D2"};
	
	for (int i = 0; i < E_SIZE_P2; i++) {
		for (size_t j = 0; j < NUM_MOVES_P2; j++) {
			cube.p2eSliceReverse(i);
			cube.rotate(moves[j]);
			ep2TransitionTableActual[i][j] = cube.p2eSliceIndex();
		}
	}
}

//old heuristic tables, only use if you want to decrease memory usage but increase solve times
/*
int Tables::loadELHeuristicTables()
{
	int result = 0;
	
	if (filesystem::exists(elHeuristicTableFileName)) {
		loadUint8(elHeuristicTableActual, EL_SIZE, elHeuristicTableFileName);
	}
	else {
		generateELHeuristicTables();
		saveUint8(elHeuristicTableActual, EL_SIZE, elHeuristicTableFileName);
		
		uint8_t temp[EL_SIZE] = {};
		
		loadUint8(temp, EL_SIZE, elHeuristicTableFileName);
		
		for (size_t i = 0; i < EL_SIZE; i++) {
			if (temp[i] != elHeuristicTableActual[i]) {
				cout << "index at " << (int) i << " doesn't match " << endl;
				result = 1;
			}
		}
	}
	
	return result;
}

void Tables::generateELHeuristicTables()
{
	for (size_t i = 0; i < EL_SIZE; i++) {
		elHeuristicTableActual[i] = 255;
	}
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	queue<uint16_t> toDo;
	
	int count = 1;
	int depth = 0;
	
	uint16_t key = cube.p2elIndex();
	uint16_t temp_key = 0;
	
	elHeuristicTableActual[key] = depth;

	toDo.push(key);
	
	cout << "EL Heuristics" << endl;
	
	while (count != EL_SIZE) {
		int n = toDo.size();
		
		cout << depth << " depth reached, found: " << count << endl;
		
		depth++;
		
		for (int i = 0; i < n && count != EL_SIZE; i++) {
			key = toDo.front();
			toDo.pop();
			
			for (size_t move = 0; move < NUM_MOVES_P2; move++) {
				temp_key = elTransitionTableActual[key][move];
				
				if (elHeuristicTableActual[temp_key] == 255) {
					elHeuristicTableActual[temp_key] = depth;
					count++;
					toDo.push(temp_key);
				}
			}
		}
	}
}

int Tables::loadCLHeuristicTables()
{
	int result = 0;
	
	if (filesystem::exists(clHeuristicTableFileName)) {
		loadUint8(clHeuristicTableActual, CL_SIZE, clHeuristicTableFileName);
	}
	else {
		generateCLHeuristicTables();
		saveUint8(clHeuristicTableActual, CL_SIZE, clHeuristicTableFileName);
		
		uint8_t temp[CL_SIZE] = {};
		
		loadUint8(temp, CL_SIZE, clHeuristicTableFileName);
		
		for (size_t i = 0; i < CL_SIZE; i++) {
			if (temp[i] != clHeuristicTableActual[i]) {
				cout << "index at " << (int) i << " doesn't match " << endl;
				result = 1;
			}
		}
	}
	
	return result;
}

void Tables::generateCLHeuristicTables()
{
	for (size_t i = 0; i < CL_SIZE; i++) {
		clHeuristicTableActual[i] = 255;
	}
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	queue<uint16_t> toDo;
	
	int count = 1;
	int depth = 0;
	
	uint16_t key = cube.p2clIndex();
	uint16_t temp_key = 0;
	
	clHeuristicTableActual[key] = depth;

	toDo.push(key);
	
	cout << "CL Heuristics" << endl;
	
	while (count != CL_SIZE) {
		int n = toDo.size();
		
		cout << depth << " depth reached, found: " << count << endl;
		
		depth++;
		
		for (int i = 0; i < n && count != CL_SIZE; i++) {
			key = toDo.front();
			toDo.pop();
			
			for (size_t move = 0; move < NUM_MOVES_P2; move++) {
				temp_key = clTransitionTableActual[key][move];
				
				if (clHeuristicTableActual[temp_key] == 255) {
					clHeuristicTableActual[temp_key] = depth;
					count++;
					toDo.push(temp_key);
				}
			}
		}
	}
}

int Tables::loadEP2HeuristicTables()
{
	int result = 0;
	
	if (filesystem::exists(ep2HeuristicTableFileName)) {
		loadUint8(ep2HeuristicTableActual, E_SIZE_P2, ep2HeuristicTableFileName);
	}
	else {
		generateEP2HeuristicTables();
		saveUint8(ep2HeuristicTableActual, E_SIZE_P2, ep2HeuristicTableFileName);
		
		uint8_t temp[E_SIZE_P2] = {};
		
		loadUint8(temp, E_SIZE_P2, ep2HeuristicTableFileName);
		
		for (size_t i = 0; i < E_SIZE_P2; i++) {
			if (temp[i] != ep2HeuristicTableActual[i]) {
				cout << "index at " << (int) i << " doesn't match " << endl;
				result = 1;
			}
		}
	}
	
	return result;
}

void Tables::generateEP2HeuristicTables()
{
	for (size_t i = 0; i < E_SIZE_P2; i++) {
		ep2HeuristicTableActual[i] = 255;
	}
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
	
	queue<uint16_t> toDo;
	
	int count = 1;
	int depth = 0;
	
	uint16_t key = cube.p2eSliceIndex();
	uint16_t temp_key = 0;
	
	ep2HeuristicTableActual[key] = depth;

	toDo.push(key);
	
	cout << "E P2 Heuristics" << endl;
	
	while (count < E_SIZE_P2) {
		int n = toDo.size();
		
		cout << depth << " depth reached, found: " << count << endl;
		
		depth++;
		
		for (int i = 0; i < n && count != E_SIZE_P2; i++) {
			key = toDo.front();
			toDo.pop();
			
			for (size_t move = 0; move < NUM_MOVES_P2; move++) {
				temp_key = ep2TransitionTableActual[key][move];
				
				if (ep2HeuristicTableActual[temp_key] == 255) {
					ep2HeuristicTableActual[temp_key] = depth;
					count++;
					toDo.push(temp_key);
				}
			}
		}
	}
}
*/

/*
all (joint) coordinate heuristic tables follow the same structure,
single coordinate heuristic tables start from a solved cube, uses the transitions to simulate rotating a cube and recording the depth value from the BFS.
joint coordinates get original keys from the joint keys, use both transition tables, recalculate the new joint key and record the new depth value from the BFS.
*/
int Tables::loadEOCOHeuristicTables()
{
	int result = 0;
	
	if (filesystem::exists(eocoHeuristicTableFileName)) {
		loadUint8(eocoHeuristicTableActual, EO_SIZE * CO_SIZE, eocoHeuristicTableFileName);
	}
	else {
		//generate the table
		generateEOCOHeuristicTables();
		//save the table
		saveUint8(eocoHeuristicTableActual, EO_SIZE * CO_SIZE, eocoHeuristicTableFileName);
		
		//use a vector to load in the table to not crash the program
		vector<uint8_t> temp(EO_SIZE * CO_SIZE);
				
		loadUint8(temp.data(), EO_SIZE * CO_SIZE, eocoHeuristicTableFileName);
		
		//ensure all values are the same
		for (size_t i = 0; i < EO_SIZE * CO_SIZE; i++) {
			if (temp[i] != eocoHeuristicTableActual[i]) {
				cout << "index at " << (int) i << " doesn't match " << endl;
				result = 1;
			}
		}
	}
	
	return result;
}

void Tables::generateEOCOHeuristicTables()
{
	for (size_t i = 0; i < EO_SIZE * CO_SIZE; i++) {
		eocoHeuristicTableActual[i] = 255;
	}
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
		
	queue<uint32_t> toDo;
	
	int count = 1;
	int depth = 0;
	
	uint16_t eoKey = cube.p1eoIndex();
	uint16_t coKey = cube.p1coIndex();
	
	uint32_t key = eoKey * CO_SIZE + coKey;
	uint32_t temp_key = 0;
	
	eocoHeuristicTableActual[key] = depth;

	toDo.push(key);
	
	cout << "EO CO Heuristics" << endl;

	//while we haven't filled out the table keep BFS-ing
	while (count < EO_SIZE * CO_SIZE) {
		int n = toDo.size();
		
		cout << depth << " depth reached, found: " << count << endl;
		
		depth++;
		
		//expand all nodes at this level
		for (int i = 0; i < n && count != EO_SIZE * CO_SIZE; i++) {
			//get the old joint key
			key = toDo.front();
			toDo.pop();
			
			//get the single coordinate keys from the joint key
			eoKey = key / CO_SIZE;
			coKey = key % CO_SIZE;
			
			//apply the transitions for each move to the single coordinates and recalculate the joint key
			for (size_t move = 0; move < NUM_MOVES; move++) {
				temp_key = eoTransitionTableActual[eoKey][move] * CO_SIZE + coTransitionTableActual[coKey][move];
				
				//if the table has a default value record the depth
				if (eocoHeuristicTableActual[temp_key] == 255) {
					eocoHeuristicTableActual[temp_key] = depth;
					count++;
					toDo.push(temp_key);
				}
			}
		}
	}
}

int Tables::loadELESliceHeuristicTables()
{
	int result = 0;
	
	if (filesystem::exists(elesliceHeuristicTableFileName)) {
		loadUint8(eleSliceHeuristicTableActual, EL_SIZE * E_SIZE_P2, elesliceHeuristicTableFileName);
	}
	else {
		generateELESliceHeuristicTables();
		saveUint8(eleSliceHeuristicTableActual, EL_SIZE * E_SIZE_P2, elesliceHeuristicTableFileName);
		
		vector<uint8_t> temp(EL_SIZE * E_SIZE_P2);
				
		loadUint8(temp.data(), EL_SIZE * E_SIZE_P2, elesliceHeuristicTableFileName);
		
		for (size_t i = 0; i < EL_SIZE * E_SIZE_P2; i++) {
			if (temp[i] != eleSliceHeuristicTableActual[i]) {
				cout << "index at " << (int) i << " doesn't match " << endl;
				result = 1;
			}
		}
	}
	
	return result;
}

void Tables::generateELESliceHeuristicTables()
{
	for (size_t i = 0; i < EL_SIZE * E_SIZE_P2; i++) {
		eleSliceHeuristicTableActual[i] = 255;
	}
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
		
	queue<uint32_t> toDo;
	
	int count = 1;
	int depth = 0;
	
	uint8_t eKey = cube.p2eSliceIndex();
	uint16_t elKey = cube.p2elIndex();
	
	uint32_t key = elKey * E_SIZE_P2 + eKey;
	uint32_t temp_key = 0;
	
	eleSliceHeuristicTableActual[key] = depth;

	toDo.push(key);
	
	cout << "EL E Slice Heuristics" << endl;
	
	while (count < EL_SIZE * E_SIZE_P2) {
		int n = toDo.size();
		
		cout << depth << " depth reached, found: " << count << endl;
		
		depth++;
		
		for (int i = 0; i < n && count != EL_SIZE * E_SIZE_P2; i++) {
			key = toDo.front();
			toDo.pop();
			
			elKey = key / E_SIZE_P2;
			eKey = key % E_SIZE_P2;
			
			for (size_t move = 0; move < NUM_MOVES_P2; move++) {
				temp_key = elTransitionTableActual[elKey][move] * E_SIZE_P2 + ep2TransitionTableActual[eKey][move];
				
				if (eleSliceHeuristicTableActual[temp_key] == 255) {
					eleSliceHeuristicTableActual[temp_key] = depth;
					count++;
					toDo.push(temp_key);
				}
			}
		}
	}
}

int Tables::loadCLESliceHeuristicTables()
{
	int result = 0;
	
	if (filesystem::exists(clesliceHeuristicTableFileName)) {
		loadUint8(cleSliceHeuristicTableActual, CL_SIZE * E_SIZE_P2, clesliceHeuristicTableFileName);
	}
	else {
		generateCLESliceHeuristicTables();
		saveUint8(cleSliceHeuristicTableActual, CL_SIZE * E_SIZE_P2, clesliceHeuristicTableFileName);
		
		vector<uint8_t> temp(CL_SIZE * E_SIZE_P2);
				
		loadUint8(temp.data(), CL_SIZE * E_SIZE_P2, clesliceHeuristicTableFileName);
		
		for (size_t i = 0; i < CL_SIZE * E_SIZE_P2; i++) {
			if (temp[i] != cleSliceHeuristicTableActual[i]) {
				cout << "index at " << (int) i << " doesn't match " << endl;
				result = 1;
			}
		}
	}
	
	return result;
}

void Tables::generateCLESliceHeuristicTables()
{
	for (size_t i = 0; i < CL_SIZE * E_SIZE_P2; i++) {
		cleSliceHeuristicTableActual[i] = 255;
	}
	
	Cube cube("UUUUUUUUULLLLLLLLLFFFFFFFFFRRRRRRRRRBBBBBBBBBDDDDDDDDD");
		
	queue<uint32_t> toDo;
	
	int count = 1;
	int depth = 0;
	
	uint8_t eKey = cube.p2eSliceIndex();
	uint16_t clKey = cube.p2clIndex();
	
	uint32_t key = clKey * E_SIZE_P2 + eKey;
	uint32_t temp_key = 0;
	
	cleSliceHeuristicTableActual[key] = depth;

	toDo.push(key);
	
	cout << "CL E Slice Heuristics" << endl;
	
	while (count < CL_SIZE * E_SIZE_P2) {
		int n = toDo.size();
		
		cout << depth << " depth reached, found: " << count << endl;
		
		depth++;
		
		for (int i = 0; i < n && count != CL_SIZE * E_SIZE_P2; i++) {
			key = toDo.front();
			toDo.pop();
			
			clKey = key / E_SIZE_P2;
			eKey = key % E_SIZE_P2;
			
			for (size_t move = 0; move < NUM_MOVES_P2; move++) {
				temp_key = clTransitionTableActual[clKey][move] * E_SIZE_P2 + ep2TransitionTableActual[eKey][move];
				
				if (cleSliceHeuristicTableActual[temp_key] == 255) {
					cleSliceHeuristicTableActual[temp_key] = depth;
					count++;
					toDo.push(temp_key);
				}
			}
		}
	}
}

//saves uint8_t tables
void Tables::saveUint8(uint8_t* table, size_t size, const string& fileName)
{
	ofstream saver(fileName, ios::binary);
	saver.write(reinterpret_cast<char*>(table), size);
}

//loads uint8_t tables
void Tables::loadUint8(uint8_t* table, size_t size, const string& fileName)
{
	ifstream loader(fileName, ios::binary);
	loader.read(reinterpret_cast<char*>(table), size);
}

//saves uint16_t tables
void Tables::saveUint16(uint16_t* table, size_t size, const string& fileName)
{
	ofstream saver(fileName, ios::binary);
	saver.write(reinterpret_cast<char*>(table), size * sizeof(uint16_t));
}

//loads uint16_t tables
void Tables::loadUint16(uint16_t* table, size_t size, const string& fileName)
{
	ifstream loader(fileName, ios::binary);
	loader.read(reinterpret_cast<char*>(table), size * sizeof(uint16_t));
}

int Tables::loadTables()
{	
	if (loadEOTransitionTables() != 0) {
		cerr << "loading eoTransitionTables ran into errors" << endl;
		return 1;
	}
	
	if (loadCOTransitionTables() != 0) {
		cerr << "loading coTransitionTables ran into errors" << endl;
		return 2;
	}
	
	if (loadETransitionTables() != 0) {
		cerr << "loading eTransitionTables ran into errors" << endl;
		return 3;
	}
	
	//uncomment this code and relevant global variables and functions to load in old heuristic tables
	/*if (loadEOHeuristicTables() != 0) {
		cerr << "loading eoHeuristicTables ran into errors" << endl;
		return 4;
	}
	
	if (loadCOHeuristicTables() != 0) {
		cerr << "loading coHeuristicTables ran into errors" << endl;
		return 5;
	}*/
	
	if (loadEHeuristicTables() != 0) {
		cerr << "loading eHeuristicTables ran into errors" << endl;
		return 6;
	}
	
	if (loadELTransitionTables() != 0) {
		cerr << "loading elTransitionTables ran into errors" << endl;
		return 7;
	}
	
	if (loadCLTransitionTables() != 0) {
		cerr << "loading clTransitionTables ran into errors" << endl;
		return 8;
	}
	
	if (loadEP2TransitionTables() != 0) {
		cerr << "loading ep2TransitionTables ran into errors" << endl;
		return 9;
	}
	
	//uncomment this code and relevant global variables and functions to load in old heuristic tables
	/*if (loadELHeuristicTables() != 0) {
		cerr << "loading elHeuristicTables ran into errors" << endl;
		return 10;
	}
	
	if (loadCLHeuristicTables() != 0) {
		cerr << "loading clHeuristicTables ran into errors" << endl;
		return 11;
	}
	
	if (loadEP2HeuristicTables() != 0) {
		cerr << "loading ep2HeuristicTables ran into errors" << endl;
		return 12;
	}*/
	
	if (loadEOCOHeuristicTables() != 0) {
		cerr << "loading eocoHeuristicTables ran into errors" << endl;
		return 13;
	}

	if (loadELESliceHeuristicTables() != 0) {
		cerr << "loading eleSliceHeuristicTables ran into errors" << endl;
		return 14;
	}

	if (loadCLESliceHeuristicTables() != 0) {
		cerr << "loading eleSliceHeuristicTables ran into errors" << endl;
		return 14;
	}

	return 0;
}

