/*
 * filesystem_scan.cpp
 *
 *  Created on: 9 Feb 2026
 *      Author: fionn
 */

#include <windows.h>
#include <iostream>

#include "filesystem_scan.h"
#include "types.h"

using namespace std;
namespace fs = std::filesystem;

void getAttributes(DWORD fileAttrs) {
	for( Attr attr : attrs){
		if (fileAttrs & attr.flag){
			wcout << attr.name << L" - ";
		}
	}
	wcout << L"\n";
}

void printDirectory(vector<unique_ptr<FSItem>>& items){
	for (auto& item : items){
		wstring typeStr;
		 switch (item->type) {
			case ItemType::File: typeStr = L"File"; break;
			case ItemType::Directory: typeStr = L"Directory"; break;
			case ItemType::Symlink: typeStr = L"Symlink"; break;
		}
		wcout << typeStr << L"\n";
		wcout << L"Name: " << item->fileName << L"\n";
		wcout << L"Path: " << item->fullPath << L"\n";
		wcout << L"Attributes: ";
		getAttributes(item->attributes);
		if (item->type == ItemType::File){
			wcout << L"File size: " << item->byteSize << L"\n";
		}
		wcout << L"\n";
	}
}

void scanDirectory(const fs::path& rootPath, FSItem* parent, vector<unique_ptr<FSItem>>& container){


	HANDLE fileHandle;
	WIN32_FIND_DATAW fileData;

	fs::path search = rootPath / L"*";

	fileHandle = FindFirstFileW(search.c_str(), &fileData);

	if (INVALID_HANDLE_VALUE == fileHandle){
		wcerr << L"Failed to open directory.\n";
		return;
	}
	do
	{
		if (wcscmp(fileData.cFileName, L".") == 0
				|| wcscmp(fileData.cFileName, L"..") == 0) continue;

		auto newItem = make_unique<FSItem>();

		//creating file name
		newItem->fileName = fileData.cFileName;

		//creating file path
		newItem->fullPath = rootPath / fileData.cFileName;

		//getting time
		newItem->LastAccessTime = fileData.ftLastAccessTime;
		newItem->LastWriteTime = fileData.ftLastWriteTime;

		//getting file attributes
		newItem->attributes = fileData.dwFileAttributes;

		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
			newItem->type = ItemType::Symlink;
		else if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			newItem->type = ItemType::Directory;
		else
			newItem->type = ItemType::File;


		if(newItem->type != ItemType::Symlink){
			HANDLE hFile;
			if(newItem->type == ItemType::File){
				newItem->byteSize = (fileData.nFileSizeHigh * (MAXDWORD+1)) + fileData.nFileSizeLow;

				hFile = CreateFileW(
						newItem->fullPath.c_str(),
						GENERIC_READ,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						nullptr,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS,
						nullptr
					);
			}

			else{
				scanDirectory(newItem->fullPath, newItem.get(), newItem->children);
				hFile = CreateFileW(
						newItem->fullPath.c_str(),
						FILE_LIST_DIRECTORY,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						nullptr,
						OPEN_EXISTING,
						FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
						nullptr
					);
			}
			if(hFile == INVALID_HANDLE_VALUE){
				DWORD lastError = GetLastError();
				wcout << L"Error opening file. Error code: " << lastError << L" for " << newItem->fullPath << L"\n";
			}
			else{
				BY_HANDLE_FILE_INFORMATION info;
				if (GetFileInformationByHandle(hFile, &info)) {
					newItem->id.volumeSerial = info.dwVolumeSerialNumber;
					newItem->id.fileIndexHigh = info.nFileIndexHigh;
					newItem->id.fileIndexLow = info.nFileIndexLow;
				}
				CloseHandle(hFile);
			}
		}
		container.push_back(move(newItem));

	} while (FindNextFileW(fileHandle, &fileData));
	FindClose(fileHandle);
	printDirectory(container);
	cout << "*************************************\n";
}
