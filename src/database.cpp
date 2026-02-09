/*
 * database.cpp
 *
 *  Created on: 9 Feb 2026
 *      Author: fionn
 */
#include <iostream>

#include "database.h"


using namespace std;

int initDatabase(const string& fileName, sqlite3*& db) {

	string sql = "CREATE TABLE IF NOT EXISTS Files("
	                      "id 		INT PRIMARY KEY NOT NULL, "
	                      "name 	TEXT 	NOT NULL, "
	                      "type 	TEXT 	NOT NULL, "
	                      "path		TEXT 	NOT NULL UNIQUE, "
						  "last_access INT  NOT NULL"
						  "last_access INT  NOT NULL"
						  ");";

	int exit = 0;
	exit = sqlite3_open(fileName.c_str(), &db);


	if (exit) {
		cerr << "Error open DB " << sqlite3_errmsg(db) << "\n";
		return (-1);
	}
	else
		std::cout << "Opened Database Successfully!" << "\n";

	return 0;

}


