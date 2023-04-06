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

#ifndef OWSOCKETCOMMON_H
#define OWSOCKETCOMMON_H

#include <cutil/global.h>

#ifdef OS_WINDOWS
	#include <winsock2.h>
	#include <windows.h>

	#ifndef CC_MINGW
		#include <wininet.h>
		#include <urlmon.h>
		#include <stdio.h>
		#include <time.h>

		#ifndef snprintf
			#define snprintf _snprintf
		#endif
	#endif

	#include <ws2tcpip.h>

	typedef SOCKET Socket;
#else
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <errno.h>
	#include <net/if.h>
	#include <sys/ioctl.h>

	typedef int Socket;
	inline int closesocket(Socket fd) { return close(fd); }
#endif

#ifndef MSG_NOSIGNAL
	#define MSG_NOSIGNAL 0
#endif

#endif	//OWSOCKETCOMMON_H
