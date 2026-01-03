#include "DatabaseLogger.h"

#include <iostream>

using namespace std;

//creates a database logger object
DatabaseLogger::DatabaseLogger(const string& path, int version)
{	
	int result = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

	if (result != SQLITE_OK) {
		cerr << "ERROR CONNECTING TO DB" << endl;
		return;
	}

	const char* create_table = "CREATE TABLE IF NOT EXISTS solves (solve_id INTEGER PRIMARY KEY AUTOINCREMENT, initial TEXT NOT NULL, solution TEXT NOT NULL, solve_time REAL NOT NULL, version INTEGER NOT NULL, p1Nodes INTEGER NOT NULL, p2Nodes INTEGER NOT NULL);";

	result = sqlite3_exec(db, create_table, nullptr, nullptr, nullptr);
	
	if (result != SQLITE_OK) {
		cerr << "TABLE CREATION FAILED: " << sqlite3_errmsg(db) << endl;
	}
	
	const char* insert_command = "INSERT INTO solves (initial, solution, solve_time, version, p1Nodes, p2Nodes) VALUES (?, ?, ?, ?, ?, ?);";
	
	result = sqlite3_prepare_v2(db, insert_command, -1, &stmt, nullptr);
	
	if (result != SQLITE_OK) {
		cerr << "PREPARE FAILED: " << sqlite3_errmsg(db) << endl;
	}
	
	this -> version = version;
}

//destroys the database logger object, specifically the statement and database so that the database isn't locked
DatabaseLogger::~DatabaseLogger()
{
	sqlite3_finalize(stmt);
	sqlite3_close_v2(db);
}

//log a vector of cubes through a transaction so it's faster
int DatabaseLogger::sqlite3_log_db(vector<DBCube>& cubes)
{
	if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != 0) {
		cerr << "begin transaction failed" << endl;
		return 1;
	}
	
	for (DBCube& cube : cubes) {
		sqlite3_log_db(cube);
	}
	
	if (sqlite3_exec(db, "END TRANSACTION;", nullptr, nullptr, nullptr) != 0) {
		cerr << "end transaction failed" << endl << "message: " << sqlite3_errmsg(db) << endl;
		return 1;
	}
	
	return 0;
}

//log individual cubes called by the above method
int DatabaseLogger::sqlite3_log_db(DBCube& cube)
{
	sqlite3_bind_text(stmt, 1, cube.get_initial_cube().c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, cube.get_solution().c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_double(stmt, 3, cube.get_time());
	sqlite3_bind_int(stmt, 4, version);
	sqlite3_bind_int(stmt, 5, cube.get_phase1_nodes());
	sqlite3_bind_int(stmt, 6, cube.get_phase2_nodes());
	
	int result = sqlite3_step(stmt);
	
	if (result != SQLITE_DONE) {
		cerr << "sqlite3_step has returned " << result << ", " << sqlite3_errmsg(db) << endl;
		return 1;
	}
	
	sqlite3_clear_bindings(stmt);
	sqlite3_reset(stmt);
	return 0;
}

//this is the logger for when you use option 3 or multiple threads
//the worker threads push into the to_log queue, mutex m is shared
int DatabaseLogger::sqlite3_log_db_multi(queue<DBCube>& to_log, mutex& m, atomic<bool>& end_program)
{
	vector<DBCube> temp;
	temp.reserve(2000);
	
	while (true) {
		if (end_program) {
			break;
		}
		//this doesn't really care about the exact size but once the queue is 1000 or more then we actually care
		if (to_log.size() >= 1000) {
			{
				//lock the mutex, then remove all DBCube objects from the queue into the temporary vector
				lock_guard<mutex> lock(m);
				
				while (!to_log.empty()) {
					temp.emplace_back(to_log.front());
					to_log.pop();
				}
			}
			
			//log the DBcubes, then clear the vector again
			sqlite3_log_db(temp);
			temp.clear();
		}
	}
	
	//once all workers leave the queue size may or may not be above 1000 in size, so we just empty it one more time and log what's left over
	while (!to_log.empty()) {
		temp.emplace_back(to_log.front());
		to_log.pop();
	}

	sqlite3_log_db(temp);
	
	return 0;
}

//this is a wrapper and this loads the specific cubes in a version number (version is a field/member variable
void DatabaseLogger::sqlite3_load_version(vector<DBCube>& data)
{
	string query = "SELECT initial, solution, solve_time, p1Nodes, p2Nodes FROM solves WHERE version = " + to_string(version) + ";";
	sqlite3_load(data, query);
}

//load all cubes into DBCube form given a query
void DatabaseLogger::sqlite3_load(vector<DBCube>& data, string& query)
{
	sqlite3_stmt* temp;
	
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &temp, nullptr) != SQLITE_OK) {
		cerr << "error with preparing the load" << endl;
		return;
	}
	
	while (sqlite3_step(temp) == SQLITE_ROW) {
		data.emplace_back(
			reinterpret_cast<const char*>(sqlite3_column_text(temp, 0)),
			reinterpret_cast<const char*>(sqlite3_column_text(temp, 1)),
			sqlite3_column_double(temp, 2),
			sqlite3_column_int(temp, 3),
			sqlite3_column_int(temp, 4)
		);
	}
	
	//important to not lock the database finalize it
	sqlite3_finalize(temp);
}

//deletes based on version BE CAREFUL USING THIS
//this is mainly used to delete the test data as it's repeated for some test cases
void DatabaseLogger::sqlite3_delete_by_version(int version)
{
	string delete_by_version = "DELETE FROM solves WHERE version = " + to_string(version) + ";";
	
	if (sqlite3_exec(db, delete_by_version.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
		cerr << "delete failed!!!" << endl;
	}
}

//returns the number of DB entries that follow a given version, not the version of the logger class
int DatabaseLogger::sqlite3_count_by_version(int given_version)
{
	int count = 0;
	string count_by_version = "SELECT COUNT(*) FROM solves WHERE version = " + to_string(given_version) + ";";
	sqlite3_stmt* temp;
	
	if (sqlite3_prepare_v2(db, count_by_version.c_str(), -1, &temp, nullptr) == SQLITE_OK) {
		if (sqlite3_step(temp) == SQLITE_ROW) {
			count = sqlite3_column_int(temp, 0);
		}
	}
	
	//dont lock the database so finalize the statement
	sqlite3_finalize(temp);
	
	return count;
}