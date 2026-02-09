//============================================================================
// Name        : anomaly_detection.cpp
// Author      : Fionn Hughes
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <windows.h>
#include <winioctl.h>
#include <Timezoneapi.h>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#include "filesystem_scan.h"

using namespace std;
namespace fs = std::filesystem;

constexpr DWORD BUFFER_SIZE = 1024 * 1024;

FILE_ID_DESCRIPTOR getFileIdDescriptor(const DWORDLONG fileId)
{
    FILE_ID_DESCRIPTOR fileDescriptor;
    fileDescriptor.Type = FileIdType;
    fileDescriptor.FileId.QuadPart = fileId;
    fileDescriptor.dwSize = sizeof(fileDescriptor);

    return fileDescriptor;
}

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


int main() {

	vector<unique_ptr<FSItem>> rootItems;
	fs::path targetPath = L"\\\\?\\D:\\Games\\SuperTux";

	HANDLE volHandle;
	getHandleToVol(volHandle);

	if(volHandle == INVALID_HANDLE_VALUE){
		wcout << L"Error opening file. Error code: " << GetLastError() << L" for " << "\\\\.\\D:" << L"\n";
	}

	USN_JOURNAL_DATA journalData{};
	DWORD bytesReturned = 0;

	BOOL queryOk = DeviceIoControl(
		volHandle,
	    FSCTL_QUERY_USN_JOURNAL,
	    nullptr,
	    0,
	    &journalData,
	    sizeof(journalData),
	    &bytesReturned,
	    nullptr
	);

	if (!queryOk) {
	    cerr << "Failed to query USN journal. It may not exist. Error: " << GetLastError() << "\n";
	    return 1;
	} else {
	    cout << "USN journal queried successfully.\n";

	}

	DWORDLONG lastUsn = journalData.NextUsn;

	string lastUsnStr;

	fstream lastUsnFile("LastUSN.txt", ios::in);
	if (lastUsnFile.is_open()) {
	    getline(lastUsnFile, lastUsnStr);
	    lastUsnFile.close();
	}
	if (lastUsnStr.empty()){
		ofstream outFile("LastUSN.txt", ios::out);
		outFile << lastUsn;
		outFile.close();
		cout << "Writing to database\n";
	}
	else {
		lastUsn = stoull(lastUsnStr);
		cout << "Reading from database\n";
	}

	//lastUsn = 156853440;
	lastUsn = 156856088;
	cout << lastUsn << "\n";

	READ_USN_JOURNAL_DATA readData{};
	readData.StartUsn       = lastUsn;
	readData.ReasonMask     = 0xFFFFFFFF;   // all reasons
	readData.UsnJournalID   = journalData.UsnJournalID;

	BYTE readBuffer[BUFFER_SIZE];

	BOOL readOk = DeviceIoControl(
			volHandle,
			FSCTL_READ_USN_JOURNAL,
		    &readData,
		    sizeof(readData),
		    &readBuffer,
		    sizeof(readBuffer),
		    &bytesReturned,
		    nullptr
		);

	if (!readOk) {
		cerr << "Failed to read USN journal. Error: " << GetLastError() << "\n";
		return 1;
	} else {
		cout << "USN journal read successfully.\n";
	}

	DWORD offset = sizeof(USN);

	cout << "Bytes returned: "<< bytesReturned << "\n";

	while (offset < bytesReturned) {
		cout << "Offset: " << offset << "\n";
	    USN_RECORD* UsnRecord = (USN_RECORD*)(readBuffer + offset);

	    FILE_ID_DESCRIPTOR fileId = getFileIdDescriptor(UsnRecord->FileReferenceNumber);

	    TCHAR filePath[MAX_PATH];
	    HANDLE fileHandle = OpenFileById(
	        volHandle,
	        &fileId,
	        GENERIC_READ,
	        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
	        nullptr,
	        FILE_FLAG_BACKUP_SEMANTICS
	    );
	    GetFinalPathNameByHandle(fileHandle,filePath, MAX_PATH, 0);


	    fs::path p(filePath);

	    if (p.native().rfind(targetPath.native(), 0) == 0) {  // path starts with targetDir

			DWORD reason = UsnRecord->Reason;
			bool relevantReason = false;

			for (auto& r : usnReasons) {
				if (reason & r.mask) {
					wcout << L"Change detected: " << r.name << L"\n";
					relevantReason = true;
				}
			}
			if(relevantReason){
				cout << p.filename() << "\n";
				wcout << p << L"\n";
			}
		}
	    offset += UsnRecord->RecordLength;
	    lastUsn = UsnRecord->Usn;
	    CloseHandle(fileHandle);
	    cout << "\n";
	}



	CloseHandle(volHandle);


	ofstream outFile("LastUSN.txt", ios::out);
	outFile << lastUsn;
	outFile.close();
	cout << "Writing new lastUsn: " << lastUsn << " to database\n";

	cout << lastUsn << "\n";


	//scanDirectory(targetPath, nullptr, rootItems);

	//printDirectory(rootItems);

	return 0;
}

