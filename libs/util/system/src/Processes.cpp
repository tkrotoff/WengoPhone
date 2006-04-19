/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include <system/Processes.h>

#ifdef WIN32
#include <windows.h>
#include <tlhelp32.h>
#endif /* WIN32 */

bool Processes::isRunning(const std::string & processName) {
#ifdef WIN32
    //check if another instance is already running
 	int index= 0;
 	int instance_counter = 0;
 	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
 	PROCESSENTRY32* processInfo =new PROCESSENTRY32;
 	processInfo->dwSize = sizeof(PROCESSENTRY32);
 	
 	while(Process32Next(hSnapShot,processInfo)!=FALSE) {
 	    std::string s(processInfo->szExeFile);
 	    if (s == processName) {
 	        instance_counter++;
 	    }
    }
 	        //if another instance is detected, exit
 	if (instance_counter == 2) {
	    return true;
    }
    return false;
#endif /* WIN32 */
}