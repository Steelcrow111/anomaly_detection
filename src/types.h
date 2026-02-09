/*
 * types.h
 *
 *  Created on: 9 Feb 2026
 *      Author: fionn
 */

#ifndef TYPES_H_
#define TYPES_H_



using namespace std;

enum class ItemType {
    File,
    Directory,
    Symlink
};

struct FileID  {
	DWORD volumeSerial;
	DWORD fileIndexHigh;
	DWORD fileIndexLow;
};

struct FSItem {
	ItemType type;
	wstring fileName;
	wstring fullPath;
	FILETIME LastAccessTime;
	FILETIME LastWriteTime;
	DWORD attributes;
	FSItem* parent = nullptr;

	//File Only
	int byteSize = 0;
	FileID id = {};

	//Folder Only
	fs::path parentPath;

	vector<unique_ptr<FSItem>> children;

};

struct Attr{
	DWORD flag;
	wstring name;
};

static const Attr attrs[] = {
	{FILE_ATTRIBUTE_READONLY, L"READONLY"},
	{FILE_ATTRIBUTE_HIDDEN, L"HIDDEN"},
	{FILE_ATTRIBUTE_SYSTEM, L"SYSTEM"},
	{FILE_ATTRIBUTE_DIRECTORY, L"DIRECTORY"},
	{FILE_ATTRIBUTE_ARCHIVE, L"ARCHIVE"},
	{FILE_ATTRIBUTE_DEVICE, L"DEVICE"},
	{FILE_ATTRIBUTE_NORMAL, L"NORMAL"},
	{FILE_ATTRIBUTE_TEMPORARY, L"TEMPORARY"},
	{FILE_ATTRIBUTE_SPARSE_FILE, L"SPARSE_FILE"},
	{FILE_ATTRIBUTE_REPARSE_POINT, L"REPARSE_POINT"},
	{FILE_ATTRIBUTE_COMPRESSED, L"COMPRESSED"}
};

struct Reason {
    DWORD mask;
    const wchar_t* name;
};

static const Reason usnReasons[] = {
    { USN_REASON_DATA_OVERWRITE,       L"DATA_OVERWRITE" },
    { USN_REASON_DATA_EXTEND,          L"DATA_EXTEND" },
    { USN_REASON_DATA_TRUNCATION,      L"DATA_TRUNCATION" },
    { USN_REASON_FILE_CREATE,          L"FILE_CREATE" },
    { USN_REASON_FILE_DELETE,          L"FILE_DELETE" },
    { USN_REASON_RENAME_NEW_NAME,      L"RENAME_NEW_NAME" },
    { USN_REASON_RENAME_OLD_NAME,      L"RENAME_OLD_NAME" },
    { USN_REASON_BASIC_INFO_CHANGE,    L"BASIC_INFO_CHANGE" },
    { USN_REASON_ENCRYPTION_CHANGE,    L"ENCRYPTION_CHANGE" },
    { USN_REASON_REPARSE_POINT_CHANGE, L"REPARSE_POINT_CHANGE" }
};


#endif /* TYPES_H_ */
