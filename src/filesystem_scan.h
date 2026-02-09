/*
 * filesystem_scan.h
 *
 *  Created on: 9 Feb 2026
 *      Author: fionn
 */

#ifndef FILESYSTEM_SCAN_H_
#define FILESYSTEM_SCAN_H_

#include "types.h"

using namespace std;

void scanDirectory(const fs::path& rootPath, FSItem* parent, vector<unique_ptr<FSItem>>& container);

#endif /* FILESYSTEM_SCAN_H_ */
