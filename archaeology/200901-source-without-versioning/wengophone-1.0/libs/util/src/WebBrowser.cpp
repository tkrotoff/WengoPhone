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

#include <WebBrowser.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef Q_WS_MACX
#include <cstdlib>
#endif

#include <qprocess.h>

#include <iostream>
using namespace std;
#include <stdlib.h>

extern int errno;

/**
 * Portable openUrl method.
 * The X11 & MacOS parts are taken from the "FreeGo" project
 *
 * TODO: find a real method to determine the default browser under.
 * 		Linux systems.
 */
bool WebBrowser::openUrl(const std::string & url) {
	
	bool return_value = false;

#ifdef WIN32
	static const char * browserCommand = "open";

	//Uses shell to invoke the web browser
	//typedef void * HINSTANCE;
	int ret = (int) ::ShellExecuteA(NULL, browserCommand, url.c_str(),
				NULL, NULL, SW_SHOWNORMAL);

	if (ret > 32) {
		return_value = true;
	} else {
		return_value = false;
	}
#endif	//WIN32

#ifdef Q_WS_X11
	QProcess * pProcess = new QProcess( 0 );
		
	// If we have Gnome
	pProcess->clearArguments();
	pProcess->addArgument( "open-url" );
	pProcess->addArgument( url );
	return_value = pProcess->start();

	// If we have KDE
	if ( !return_value ) {
		pProcess->clearArguments();
		pProcess->addArgument( "kfmclient" );
		pProcess->addArgument( "exec" );
		pProcess->addArgument( url );
		return_value = pProcess->start();
	}
	
	// Try with FireFox
	if ( !return_value ) {
		pProcess->clearArguments();
		pProcess->addArgument( "mozilla-firefox" );
		pProcess->addArgument( url );
		return_value = pProcess->start();
	}
	// Try with Mozilla
	else if ( !return_value ) {
		pProcess->clearArguments();
		pProcess->addArgument( "mozilla" );
		pProcess->addArgument( url );
		return_value = pProcess->start();
	}
	// Try with galeon
	else if ( !return_value ) {
		pProcess->clearArguments();
		pProcess->addArgument( "galeon" );
		pProcess->addArgument( url );
		return_value = pProcess->start();
	}	
#endif	//Q_WS_X11

#ifdef Q_WS_MACX
	// Running on a Mac in OS X
	Q_UNUSED( 0 );
	QString command( "open \"" );
	command += url;
	command += "\"";
	return_value = ( system( command.latin1() ) == 0 );
#endif //Q_WS_MACX
	return return_value;
}
