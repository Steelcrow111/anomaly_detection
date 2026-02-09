/*
 * database.h
 *
 *  Created on: 9 Feb 2026
 *      Author: fionn
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include "sqlite3.h"

int initDatabase(const std::string& fileName, sqlite3*& db);

#endif /* DATABASE_H_ */
