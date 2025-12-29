#pragma once

#include <vector>
#include <string>

#include "DBCube.h"

#include <sqlite3.h>

using namespace std;

class DatabaseLogger
{
	private:
		sqlite3* db = nullptr;
		sqlite3_stmt* stmt = nullptr;
		int version;
	public:
		DatabaseLogger(const string& path, int version);
		~DatabaseLogger();
	
		int sqlite3_log_db(vector<DBCube>& cubes);
		int sqlite3_log_db(DBCube& cube);
		
		void sqlite3_load(vector<DBCube>& data, string& query);
		void sqlite3_load_version(vector<DBCube>& data);
		
		void sqlite3_delete_by_version(int version);
		
		int sqlite3_count_by_version(int version);
};