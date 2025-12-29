#include "DBCube.h"

using namespace std;

DBCube::DBCube() = default;

DBCube::DBCube(const string& initial_cube, const string& solution, double time, int phase1_nodes, int phase2_nodes)
{
	this -> initial_cube = initial_cube;
	this -> solution = solution;
	this -> time = time;
	this -> phase1_nodes = phase1_nodes;
	this -> phase2_nodes = phase2_nodes;
}

const string& DBCube::get_initial_cube() const
{
	return initial_cube;
}

const string& DBCube::get_solution() const
{
	return solution;
}

double DBCube::get_time() const
{
	return time;
}

int DBCube::get_phase1_nodes() const
{
	return phase1_nodes;
}

int DBCube::get_phase2_nodes() const
{
	return phase2_nodes;
}

void DBCube::set_initial_cube(string& initial_cube)
{
	this -> initial_cube = initial_cube;
}

void DBCube::set_solution(string& solution)
{
	this -> solution = solution;
}

void DBCube::set_time(double time)
{
	this -> time = time;
}

void DBCube::set_phase1_nodes(int phase1_nodes)
{
	this -> phase1_nodes = phase1_nodes;
}

void DBCube::set_phase2_nodes(int phase2_nodes)
{
	this -> phase2_nodes = phase2_nodes;
}

vector<string> DBCube::solutionSplitter()
{
	//from the loaded in string, we return a vector<string> of the moves
	vector<string> moves;
	moves.reserve(32);
	
	size_t l = 0;
	size_t r = 0;
	
	//the assumption unless modified is that the deliminator is a ' '. this code splits up the string into the vector
	while ((r = solution.find(' ', l)) != string::npos) {
		moves.emplace_back(solution.substr(l, r - l));
		l = r + 1;
	}
	
	if (l < solution.size()) {
		moves.emplace_back(solution.substr(l));
	}
	
	return moves;
}