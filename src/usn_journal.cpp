/*
 * usn_journal.cpp
 *
 *  Created on: 9 Feb 2026
 *      Author: fionn
 */

#include <iostream>

#include "usn_journal.h"

void getHandleToVol(HANDLE& volHandle){
	volHandle = CreateFileW(
			L"\\\\.\\D:",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE	,
			nullptr,
			OPEN_EXISTING,
			0,
			nullptr
		);
}

