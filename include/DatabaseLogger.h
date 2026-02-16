//this handles the database logging
//only create one version per database or else you may lock the sqlite database

#pragma once

#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <cstdlib>
#define OMPI_SKIP_MPICXX
#include <mpi.h>

#include "DBCube.h"

#include <sqlite3.h>

using namespace std;

class DatabaseLogger
{
	private:
		sqlite3* db = nullptr;
		sqlite3_stmt* stmt = nullptr;
		double version;
	public:
		DatabaseLogger(const string& path, double version);
		~DatabaseLogger();
	
		int sqlite3_log_db(vector<DBCube>& cubes);
		int sqlite3_log_db(DBCube& cube);
		int sqlite3_log_db_multi(queue<DBCube>& to_log, mutex& m, atomic<bool>& end_program, bool mpi_used, int rank, int size);
		
		void sqlite3_load(vector<DBCube>& data, string& query);
		void sqlite3_load_version(vector<DBCube>& data);
		
		void sqlite3_delete_by_version(double version);
		
		int sqlite3_count_by_version(double version);

		void getReadings(long long readings[]);
		float getCPUUsage(long long readings[]);
		float getRAMUsage();
		string formatMessage(int cubes_done, int rank, int reportNum, long long readings[]);
};