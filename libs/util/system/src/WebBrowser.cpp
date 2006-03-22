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

#include <system/WebBrowser.h>

#include <cutil/global.h>

#ifdef OS_WINDOWS
	#include <windows.h>
#endif

bool WebBrowser::openUrl(const std::string & url) {
#ifdef OS_WINDOWS
	static const char * browserCommand = "open";

	//Uses shell to invoke the web browser
	//typedef void * HINSTANCE;
	int ret = (int) ::ShellExecuteA(NULL, browserCommand, url.c_str(),
				NULL, NULL, SW_SHOWNORMAL);

	if (ret > 32) {
		return true;
	}

	return false;
#else
	return false;
#endif	//OS_WINDOWS
}
