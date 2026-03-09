#include "DatabaseLogger.h"

using namespace std;

//creates a database logger object
DatabaseLogger::DatabaseLogger(const string& path, double version)
{	
	int result = sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

	if (result != SQLITE_OK) {
		cerr << "ERROR CONNECTING TO DB" << endl;
		return;
	}

	const char* create_table = "CREATE TABLE IF NOT EXISTS solves (solve_id INTEGER PRIMARY KEY AUTOINCREMENT, initial TEXT NOT NULL, solution TEXT NOT NULL, solve_time REAL NOT NULL, version REAL NOT NULL, p1Nodes INTEGER NOT NULL, p2Nodes INTEGER NOT NULL);";

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

void DatabaseLogger::getReadings(long long readings[])
{
	ifstream reader("/proc/stat");
	string line;

	getline(reader, line);

	static int initial_offset = -1;

	if (initial_offset == -1) {
		initial_offset = 0;
		for (char& c : line) {
			if (c < '0' || c > '9') {
				initial_offset++;
				continue;			
			}
			break;
		}
	}

	line = line.substr(initial_offset);

	stringstream ss(line);

	for (int i = 0; i < 10; i++) {
		ss >> readings[i];
	}

	reader.close();
}

float DatabaseLogger::getCPUUsage(long long readings[])
{
	long long new_readings[10];

	long long total_cpu = 0;
	long long total_used_cpu = 0;
	long long temp = 0;

	getReadings(new_readings);

	for (int i = 0; i < 10; i++) {
		temp = new_readings[i] - readings[i];
		total_cpu += temp;
		if (i != 3 && i != 4) {
			total_used_cpu += temp;
		}
		readings[i] = new_readings[i];
	}

	if (total_cpu == 0) {
		return 0.0;
	}

	float ans = total_used_cpu * 100.0 / total_cpu;

	if (ans < 0) {
		return 0.0;	
	}
	else if (ans > 100) {
		return 100.0;
	}

	return ans;
}

float DatabaseLogger::getRAMUsage()
{
	ifstream reader("/proc/meminfo");
	string line;

	getline(reader, line);

	size_t offset = line.find_first_of("0123456789");
	long totalRAM =  strtol(line.substr(offset).c_str(), NULL, 10);

	getline(reader, line);
	getline(reader, line);

	offset = line.find_first_of("0123456789");
	long RAMLeft = strtol(line.substr(offset).c_str(), NULL, 10);

	reader.close();

	if (totalRAM == 0) {
		return 0.0;
	}

	return 100 - RAMLeft * 100.0 / totalRAM;
}

string DatabaseLogger::formatMessage(int cubes_done, int rank, int reportNum, long long readings[])
{
	string message;
	message.reserve(2048);

	//message.append("\"node" + to_string(rank) + "\": {");
	message.append("{\"nodeID\": " + to_string(rank));
	message.append(", \"cubesSolved\": "+ to_string(cubes_done));
	message.append(", \"cpuUsage\": " + to_string(getCPUUsage(readings)));
	message.append(", \"ramUsage\": " + to_string(getRAMUsage()));
	message.append(", \"reportNumber\": " + to_string(reportNum));
	message.append("}");

	return message;
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
	sqlite3_bind_double(stmt, 4, version);
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
int DatabaseLogger::sqlite3_log_db_multi(queue<DBCube>& to_log, mutex& m, atomic<bool>& end_program, bool mpi_used, int rank, int size)
{
	vector<DBCube> temp;
	temp.reserve(2000);

	int cubes_since_last_report = 0;
	int reportNum = 0;
	string message;

	const int reportStatsFlag = 42;
	const int statsTag = 420;

	const int endFlag = 69;
	const int endTag = 67;

	int flag = -1;
	int message_available = 0;

	bool firstMessage = true;
	bool log = false;

	long long cpuReadings[10];

	message.reserve(1024);

	vector<bool> inProgress;

	inProgress.resize(size, true);

	if (!mpi_used) {
		while (true) {
			log = false;
			this_thread::sleep_for(chrono::milliseconds(10));
			if (end_program) {
				break;
			}

			{
				lock_guard<mutex> lock(m);

				if (to_log.size() >= 1000) {
					log = true;
				}
			}

			//this doesn't really care about the exact size but once the queue is 1000 or more then we actually care
			if (log) {
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
	}
	else {
		int total_logged = 0;
		getReadings(cpuReadings);

		if (rank == 0) {
			int socket_description = socket(AF_INET, SOCK_STREAM, 0);
			struct sockaddr_in server;

			int nodes_left = size;
			char buffer[2048];

			server.sin_family = AF_INET;
			server.sin_port = htons(PORT_NUM);
			server.sin_addr.s_addr = inet_addr("127.0.0.1");

			if (socket_description == -1) {
				cout << "Socket wasn't created" << endl;
				return -1;
			}

			if (connect(socket_description, (struct sockaddr *)&server, sizeof(server)) < 0) {
				cerr << "Connecting to server has failed!" << endl;
				return 1;
			}

			while (true) {
				//sleep so we don't force workers to lock the queue a lot
				this_thread::sleep_for(chrono::milliseconds(1000));

				message_available = 0;
				firstMessage = true;

				for (int i = 1; i < size; i++) {
					MPI_Iprobe(i, endTag, MPI_COMM_WORLD, &message_available, MPI_STATUS_IGNORE);

					if (message_available == 1) {
						MPI_Recv(&flag, 1, MPI_INT, i, endTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						if (flag == endFlag) {
							inProgress[i] = false;
							nodes_left--;
							//cout << "Node " << i << " end flag has been read" << endl;
						}
					}
					else if (inProgress[i]) {
						MPI_Send(&reportStatsFlag, 1, MPI_INT, i, statsTag, MPI_COMM_WORLD);
					}
				}

				{
					//lock the mutex, then remove all DBCube objects from the queue into the temporary vector
					lock_guard<mutex> lock(m);
					
					while (!to_log.empty()) {
						temp.emplace_back(to_log.front());
						to_log.pop();
					}
				}

				cubes_since_last_report = temp.size();
				total_logged += cubes_since_last_report;

				//log the DBcubes, then clear the vector again
				sqlite3_log_db(temp);
				temp.clear();

				message = "{\"type\": \"report\", \"nodes\": [";

				//cout << "Node 0 message: " << message << endl;

				if (inProgress[0]) {
					message.append(formatMessage(cubes_since_last_report, rank, reportNum, cpuReadings));
					firstMessage = false;
				}

				if (end_program && inProgress[0]) {
					inProgress[0] = false;
					nodes_left--;
				}
				else if (nodes_left == 0) {
					close(socket_description);
					break;
				}

				for (int i = 1; i < size; i++) {
					if (inProgress[i]) {
						MPI_Recv(buffer, 2048, MPI_CHAR, i, statsTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
						if (!firstMessage) {
							message.append(",");
						}
						else {
							firstMessage = false;
						}
						message.append(buffer);

						//cout << "Node " << i << " message: " << buffer << endl;
					}
					else {
						MPI_Iprobe(i, statsTag, MPI_COMM_WORLD, &message_available, MPI_STATUS_IGNORE);

						if (message_available == 1) {
							MPI_Recv(buffer, 2048, MPI_CHAR, i, statsTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
							if (!firstMessage) {
								message.append(",");
							}
							else {
								firstMessage = false;
							}
							message.append(buffer);
							//cout << "Node " << i << " message: " << buffer << endl;
						}
						else {
							//cout << "Node " << i << " is done!" << endl;
						}
					}
				}

				message.append("]}\n");

				cout << "combined message: " << message << endl;

				if (send(socket_description, message.c_str(), message.length(), 0) < 0) {
					cout << "Report " << reportNum << " has not been sent correctly!" << endl;
				}
				else {
					cout << "Report " << reportNum << " has been sent correctly!" << endl;
				}

				reportNum++;
			}
		}
		else {
			while (true) {
				log = false;
				flag = -1;
				message_available = 0;

				if (end_program) {
					//once all workers leave the queue size may or may not be above 1000 in size, so we just empty it one more time and log what's left over
					while (!to_log.empty()) {
						temp.emplace_back(to_log.front());
						to_log.pop();
					}

					cubes_since_last_report += temp.size();
					total_logged += cubes_since_last_report;

					message = formatMessage(cubes_since_last_report, rank, reportNum, cpuReadings);
					MPI_Send(message.c_str(), message.length() + 1, MPI_CHAR, 0, statsTag, MPI_COMM_WORLD);

					sqlite3_log_db(temp);
					temp.clear();

					MPI_Send(&endFlag, 1, MPI_INT, 0, endTag, MPI_COMM_WORLD);
					//cout << "Node " << rank << " is done! sent final message then end flag" << endl;
					break;
				}

				this_thread::sleep_for(chrono::milliseconds(10));

				MPI_Iprobe(0, statsTag, MPI_COMM_WORLD, &message_available, MPI_STATUS_IGNORE);

				if (message_available == 1) {
					MPI_Recv(&flag, 1, MPI_INT, 0, statsTag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				}

				{
					lock_guard<mutex> lock(m);

					if (to_log.size() >= 1000) {
						log = true;
					}
				}

				//this doesn't really care about the exact size but once the queue is 1000 or more then we actually care
				if (log || flag == reportStatsFlag) {
					{
						//lock the mutex, then remove all DBCube objects from the queue into the temporary vector
						lock_guard<mutex> lock(m);
						
						while (!to_log.empty()) {
							temp.emplace_back(to_log.front());
							to_log.pop();
						}
					}
					
					cubes_since_last_report += temp.size();
					total_logged += cubes_since_last_report;

					if (flag == reportStatsFlag) {
						message = formatMessage(cubes_since_last_report, rank, reportNum, cpuReadings);
						reportNum++;
						cubes_since_last_report = 0;
						MPI_Send(message.c_str(), message.length() + 1, MPI_CHAR, 0, statsTag, MPI_COMM_WORLD);
					}

					//log the DBcubes, then clear the vector again
					sqlite3_log_db(temp);
					temp.clear();
				}
			}
		}

		cout << "Node " << rank << " has logged " << total_logged << endl;
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
void DatabaseLogger::sqlite3_delete_by_version(double version)
{
	string delete_by_version = "DELETE FROM solves WHERE version = " + to_string(version) + ";";
	
	if (sqlite3_exec(db, delete_by_version.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
		cerr << "delete failed!!!" << endl;
	}
}

//returns the number of DB entries that follow a given version, not the version of the logger class
int DatabaseLogger::sqlite3_count_by_version(double given_version)
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