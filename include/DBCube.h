//this class is just the columns of the sqlite3 database

#pragma once

#include <string>
#include <vector>

using namespace std;

class DBCube
{
	private:
		string initial_cube;
		string solution;
		double time;
		int phase1_nodes;
		int phase2_nodes;
	public:
		DBCube();
		DBCube(const string& initial_cube, const string& solution, double time, int phase1_nodes, int phase2_nodes);
		
		const string& get_initial_cube() const;
		const string& get_solution() const;
		double get_time() const;
		int get_phase1_nodes() const;
		int get_phase2_nodes() const;
		
		void set_initial_cube(string& initial_cube);
		void set_solution(string& solution);
		void set_time(double time);
		void set_phase1_nodes(int phase1_nodes);
		void set_phase2_nodes(int phase2_nodes);
		
		vector<string> solutionSplitter();
};