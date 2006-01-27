/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * Check http://www.codeproject.com/debug/postmortemdebug_standalone1.asp
 */

#include <win32/MemoryDump.h>

#include <Logger.h>
#include <StringList.h>

#include <shlwapi.h>

#include <cstring>
#include <ctime>
#include <cstdio>
#include <iostream>
using namespace std;

char * MemoryDump::_applicationName = NULL;
char * MemoryDump::_styleName = NULL;
char * MemoryDump::_languageFilename = NULL;

static const char * DBGHELP_DLL = "dbghelp.dll";

MemoryDump::MemoryDump(const char * applicationName) {
	//If this assert fires then you have two instances of MemoryDump
	//which is not allowed
	if (_applicationName) {
		LOG_FATAL("two instances of MemoryDump are not allowed");
	}

	_applicationName = strdup(applicationName);

	::SetUnhandledExceptionFilter(topLevelFilter);
}

MemoryDump::~MemoryDump() {
	//FIXME why this crashes under Visual C++ 2003
	/*free(_applicationName);
	free(_styleName);
	free(_languageFilename);*/
	_applicationName = NULL;
	_styleName = NULL;
	_languageFilename = NULL;
}

HMODULE MemoryDump::loadDebugHelpLibrary() {
	//Firstly see if dbghelp.dll is around and has the function we need
	//look next to the .exe first, as the one in the system32 directory
	//might be old (e.g. Windows 2000)
	HMODULE hDll = NULL;
	char dbghelpPath[_MAX_PATH];

	if (GetModuleFileNameA(NULL, dbghelpPath, _MAX_PATH)) {
		char * pSlash = strrchr(dbghelpPath, '\\');
		if (pSlash) {
			strcpy(pSlash + 1, DBGHELP_DLL);
			hDll = ::LoadLibraryA(dbghelpPath);
		}
	}

	if (hDll == NULL) {
		//Load any version we can
		hDll = ::LoadLibraryA(DBGHELP_DLL);
	}

	if (!hDll) {
		LOG_ERROR_C(String(DBGHELP_DLL) + " not found");
	}

	return hDll;
}

MINIDUMPWRITEDUMP MemoryDump::loadMiniDumpWriteDumpFunction() {
	MINIDUMPWRITEDUMP pDump = NULL;
	HMODULE hDll = loadDebugHelpLibrary();
	if (hDll) {
		pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");
		if (!pDump) {
			LOG_ERROR_C(String(DBGHELP_DLL) + " too old");
		}
	}

	return pDump;
}

void MemoryDump::setStyle(const char * styleName) {
	_styleName = strdup(styleName);
}

void MemoryDump::setLanguage(const char * languageFilename) {
	_languageFilename = strdup(languageFilename);
}

long MemoryDump::topLevelFilter(struct _EXCEPTION_POINTERS * pExceptionInfo) {
	long ret = EXCEPTION_CONTINUE_SEARCH;

	MINIDUMPWRITEDUMP pDump = loadMiniDumpWriteDumpFunction();
	if (pDump) {
		//Wait a bit because several minidump can be created
		Sleep(1000);

		//Name of the memory dump
		//Use current path
		char memoryDumpName[_MAX_PATH];
		strcpy(memoryDumpName, _applicationName);
		strcat(memoryDumpName, "-");
		strcat(memoryDumpName, getCurrentDateTime());
		strcat(memoryDumpName, ".dmp");

		//GetModuleFileName retrieves the path of the executable file of the current process.
		char memoryDumpFile[_MAX_PATH];
		GetModuleFileNameA(NULL, memoryDumpFile, sizeof(memoryDumpFile));
		PathRemoveFileSpecA(memoryDumpFile);
		PathAddBackslashA(memoryDumpFile);
		strcat(memoryDumpFile, memoryDumpName);

		//Create the file
		HANDLE hFile = ::CreateFileA(memoryDumpFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
						FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile != INVALID_HANDLE_VALUE) {
			_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

			ExInfo.ThreadId = ::GetCurrentThreadId();
			ExInfo.ExceptionPointers = pExceptionInfo;
			ExInfo.ClientPointers = NULL;

			//Write the dump
			BOOL ok = pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
			if (ok) {
				LOG_DEBUG_C("dump file saved to: " + String(memoryDumpName));
				ret = EXCEPTION_EXECUTE_HANDLER;
			} else {
				LOG_ERROR_C("failed to save dump file to: " + String(memoryDumpName) + " " + String::fromNumber(GetLastError()));
			}
			::CloseHandle(hFile);
		} else {
			LOG_ERROR_C("failed to create dump file: " + String(memoryDumpName) + " " + String::fromNumber(GetLastError()));
		}

		//Launch memorydump.exe
		char commandLine[_MAX_PATH];
		strcpy(commandLine, "memorydump");
		if (_styleName != NULL) {
			strcat(commandLine, " -style=");
			strcat(commandLine, _styleName);
		}
		strcat(commandLine, " \"");
		strcat(commandLine, memoryDumpFile);
		strcat(commandLine, "\" ");
		strcat(commandLine, _applicationName);
		if (_languageFilename != NULL) {
			strcat(commandLine, " \"");
			strcat(commandLine, _languageFilename);
			strcat(commandLine, "\"");
		}
		cerr << "MemoryDump: " << commandLine << endl;
		executeProcess(commandLine);
	}

	return ret;
}

BOOL MemoryDump::executeProcess(char * commandLine) {
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	GetStartupInfoA(&si);

	BOOL success = ::CreateProcessA(NULL,
		commandLine,	//Name of app to launch
		NULL,	//Default process security attributes
		NULL,	//Default thread security attributes
		FALSE,	//Don't inherit handles from the parent
		0,	//Normal priority
		NULL,	//Use the same environment as the parent
		NULL,	//Launch in the current directory
		&si,	//Startup Information
		&pi);	//Process information stored upon return

	return success;
}

char * MemoryDump::getCurrentDateTime() {
	time_t rawtime;
	time(&rawtime);
	struct tm * timeinfo = localtime(&rawtime);

	//Example: 02/11 23:46:13
	//0211234613
	int length = 14;
	char * dateTime = (char *) malloc(length * sizeof(char));

	int month = timeinfo->tm_mon + 1;
	char monthStr[2];
	if (month < 10) {
		sprintf(monthStr, "0%d", month);
	} else {
		sprintf(monthStr, "%d", month);
	}

	int day = timeinfo->tm_mday;
	char dayStr[2];
	if (day < 10) {
		sprintf(dayStr, "0%d", day);
	} else {
		sprintf(dayStr, "%d", day);
	}

	int hour = timeinfo->tm_hour;
	char hourStr[2];
	if (hour < 10) {
		sprintf(hourStr, "0%d", hour);
	} else {
		sprintf(hourStr, "%d", hour);
	}

	int minutes = timeinfo->tm_min;
	char minutesStr[2];
	if (minutes < 10) {
		sprintf(minutesStr, "0%d", minutes);
	} else {
		sprintf(minutesStr, "%d", minutes);
	}

	int seconds = timeinfo->tm_sec;
	char secondsStr[2];
	if (seconds < 10) {
		sprintf(secondsStr, "0%d", seconds);
	} else {
		sprintf(secondsStr, "%d", seconds);
	}

	sprintf(dateTime, "%s%s%s%s%s", monthStr, dayStr, hourStr, minutesStr, secondsStr);

	return dateTime;
}
