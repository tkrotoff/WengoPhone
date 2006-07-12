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

#include <localserver/LocalServer.h>

#include <cutil/global.h>
#include <util/String.h>

#ifdef OS_WINDOWS

#include <winsock2.h>
#include <windows.h>
#ifndef CC_MINGW
#include <wininet.h>
#include <urlmon.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include <time.h>

#ifndef snprintf
#define snprintf _snprintf
#endif

typedef SOCKET Socket;
#endif //CC_MINGW
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
inline int closesocket(Socket fd) {return close(fd);}

#endif	//WIN32

#ifndef MSG_NOSIGNAL
	#define MSG_NOSIGNAL 0
#endif

Socket _mainSock;
static std::list<Socket> _clientSockList;
typedef std::list<Socket>::iterator SockListIterator;

std::string LocalServer::listeningIp = "127.0.0.1";

LocalServer::LocalServer(int port) : _port(port) {
	_started = false;
}

void LocalServer::init() {
	if (createMainListeningSocket()) {
		_started = true;
		start();
	}
}

bool LocalServer::createMainListeningSocket() {
	
	struct sockaddr_in raddr;
	int option = 1;
	
	if (listeningIp.empty())
		raddr.sin_addr.s_addr = htons(INADDR_ANY);
	else
		raddr.sin_addr.s_addr = inet_addr(listeningIp.c_str());
	raddr.sin_port = htons(_port);
	raddr.sin_family = AF_INET;
	
	_mainSock = 0;
	if ((_mainSock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		LOG_DEBUG("cannot create main socket");
		return false;
	}

	setsockopt(_mainSock, SOL_SOCKET, SO_REUSEADDR, (void*)&option, sizeof(int));

	if (bind(_mainSock, (struct sockaddr *)&raddr, sizeof (raddr)) < 0)
	{
		LOG_DEBUG("cannot bind main socket");
		closesocket(_mainSock);
		_mainSock = 0;
		return false;
	}

	listen(_mainSock, 5);

	return true;
}

LocalServer::~LocalServer() {

	if (_started) {
		_started = false;
		
		if (_mainSock) {
			closesocket(_mainSock);
		}

		SockListIterator it;
		for (it = _clientSockList.begin(); it != _clientSockList.end(); it++) {
			closesocket(*it);
			_clientSockList.erase(it);
		}
	}
}

bool LocalServer::closeAndRemoveFromList(const std::string & connectionId) {
	Socket sockId = (Socket) String(connectionId).toInteger();

	SockListIterator it;
	for (it = _clientSockList.begin(); it != _clientSockList.end(); it++) {
		if (*it == sockId)
		{
			closesocket(*it);
			_clientSockList.erase(it);
			return true;
		}
	}

	return false;
}

bool LocalServer::checkConnectionId(const std::string & connectionId) {
	Socket sockId = (Socket) String(connectionId).toInteger();

	SockListIterator it;
	for (it = _clientSockList.begin(); it != _clientSockList.end(); it++) {
		if (*it == sockId)
			return true;
	}

	return false;
}
	
bool LocalServer::writeToClient(const std::string & connectionId, const std::string & data) {
	int i;
	int size;
	Socket sockId = (Socket) String(connectionId).toInteger();
	Error error = UnknownError;
	
	if (checkConnectionId(connectionId)) {
		for (i = 0, size = data.length() + 1; size; size -= i) {
			i = send(sockId, data.c_str(), size, MSG_NOSIGNAL);
			if (i < 1) {
				break;
			}
		}
		
		if (size == 0)
			error = NoError;
	}

	closeAndRemoveFromList(connectionId);
	writeStatusEvent(this, connectionId, error);
	return (error == NoError);
}

int LocalServer::getHighestSocket()
{
	int highest = 0;

	SockListIterator it;
	for (it = _clientSockList.begin(); it != _clientSockList.end(); it++) {
		if (((int) *it) > highest)
			highest = (int) *it;
	}

	if (((int) _mainSock) > highest)
		highest = _mainSock;

	return highest;
}

int	LocalServer::getRequest(int sockId, char *buff, unsigned int buffsize)
{
	struct timeval	timeout;
	fd_set	rfds;
	int ret;
	int nbytes = 0;
	Socket sock = (Socket) sockId;

	memset(buff, 0, buffsize);

	while (1)
	{
		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_SET(sockId, &rfds);

		ret = select(sock + 1, &rfds, 0, 0, &timeout);
		
		if (ret <= 0)
			return -1;

		if (FD_ISSET(sock, &rfds))
		{
			ret = recv(sock, buff + nbytes, 1, 0);
			
			if (ret <= 0)
				return -1;
			else
				nbytes += ret;

			if (nbytes == buffsize)
				return nbytes;
			
			if (strncmp("\0", buff + nbytes - 1, 1) == 0)
				break;
		}
	}

	return nbytes;
}

void LocalServer::run() {
	Socket sock;
	struct sockaddr from;
	socklen_t fromlen = sizeof(from);
	fd_set rfds;
	struct timeval to;
	int received = 0;
	int err = 0;
	char buff[256];

	serverStatusEvent(this, NoError);

	while (_started) {
		SockListIterator it;

		FD_ZERO(&rfds);
		FD_SET(_mainSock, &rfds);

		for (it = _clientSockList.begin(); it != _clientSockList.end(); it++) {
			FD_SET(*it, &rfds);
		}
		
		err = select(getHighestSocket() + 1, &rfds, 0, 0, 0);

		received = 0;
		if (err && FD_ISSET(_mainSock, &rfds)) {
			sock = accept(_mainSock, &from, &fromlen);
			if (sock > 0) {
				_clientSockList.push_back(sock);
				connectionEvent(this, String::fromNumber((int)sock));
			}
		}
		else if (err) {
			SockListIterator it;
			for (it = _clientSockList.begin(); it != _clientSockList.end(); it++) {
				if (FD_ISSET(*it, &rfds)) {
					if (getRequest(*it, buff, sizeof(buff)) == -1) {
						LOG_DEBUG("error reading socket");
						closesocket(*it);
						_clientSockList.erase(it);
					} else {
						incomingRequestEvent(this, String::fromNumber((int)*it), std::string(buff));
					}
					break;
				}
			}
		}
	}

	serverStatusEvent(this, UnknownError);
}
