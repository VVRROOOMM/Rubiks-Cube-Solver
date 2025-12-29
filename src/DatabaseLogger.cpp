#include "DatabaseLogger.h"

#include <iostream>

using namespace std;

DatabaseLogger::DatabaseLogger(const string& path, int version)
{	
	int result = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

	if (result != SQLITE_OK) {
		cerr << "ERROR CONNECTING TO DB" << endl;
		return;
	}

	const char* create_table = "CREATE TABLE IF NOT EXISTS main.solves (solve_id INTEGER PRIMARY KEY AUTOINCREMENT, initial TEXT NOT NULL, solution TEXT NOT NULL, solve_time REAL NOT NULL, version INTEGER NOT NULL, p1Nodes INTEGER NOT NULL, p2Nodes INTEGER NOT NULL);";

	sqlite3_exec(db, create_table, nullptr, nullptr, nullptr);
	
	const char* insert_command = "INSERT INTO main.solves (initial, solution, solve_time, version, p1Nodes, p2Nodes) VALUES (?, ?, ?, ?, ?, ?);";
	
	sqlite3_prepare_v2(db, insert_command, -1, &stmt, nullptr);
	
	this -> version = version;
}

DatabaseLogger::~DatabaseLogger()
{
	sqlite3_finalize(stmt);
	sqlite3_close_v2(db);
}

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
		cerr << "end transaction failed" << endl;
		return 1;
	}
	
	return 0;
}

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
		cerr << "sqlite3_step has returned " << result << endl;
		return 1;
	}
	
	sqlite3_clear_bindings(stmt);
	sqlite3_reset(stmt);
	return 0;
}

void DatabaseLogger::sqlite3_load_version(vector<DBCube>& data)
{
	string query = "SELECT initial, solution, solve_time, p1Nodes, p2Nodes FROM main.solves WHERE version = " + to_string(version) + ";";
	sqlite3_load(data, query);
}

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
	
	sqlite3_finalize(temp);
}

void DatabaseLogger::sqlite3_delete_by_version(int version)
{
	string delete_by_version = "DELETE FROM main.solves WHERE version = " + to_string(version) + ";";
	
	if (sqlite3_exec(db, delete_by_version.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
		cerr << "delete failed!!!" << endl;
	}
}

int DatabaseLogger::sqlite3_count_by_version(int version)
{
	int count = 0;
	string count_by_version = "SELECT COUNT(*) FROM main.solves WHERE version = " + to_string(version) + ";";
	sqlite3_stmt* temp;
	
	if (sqlite3_prepare_v2(db, count_by_version.c_str(), -1, &temp, nullptr) == SQLITE_OK) {
		if (sqlite3_step(temp) == SQLITE_ROW) {
			count = sqlite3_column_int(temp, 0);
		}
	}
	
	return count;
}