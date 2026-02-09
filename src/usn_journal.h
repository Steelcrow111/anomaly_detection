/*
 * usn_journal.h
 *
 *  Created on: 9 Feb 2026
 *      Author: fionn
 */

#ifndef USN_JOURNAL_H_
#define USN_JOURNAL_H_

#include <windows.h>
#include <filesystem>
#include "types.h"

HANDLE openVolume(const wstring& volPath);

int queryJournal(HANDLE volume, USN_JOURNAL_DATA& journalData, DWORD& bytesReturned);

void loadLastUsn(string fileName, DWORDLONG& lastUsn);

void saveLastUsn(string fileName, DWORDLONG& lastUsn);

void readJournalSince(HANDLE& volume, USN_JOURNAL_DATA& journal, DWORDLONG& lastUsn, const fs::path volPath);

#endif /* USN_JOURNAL_H_ */
