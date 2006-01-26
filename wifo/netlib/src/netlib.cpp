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

#include <netlib.h>

#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>

#ifdef WIN32

#include <winsock2.h>
#include <windows.h>
#include <wininet.h>
#include <urlmon.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include <time.h>

typedef struct {
	BOOL(* IsResolvable) (LPSTR lpszHost);
	DWORD(* GetIPAddress) (LPSTR lpszIPAddress, LPDWORD lpdwIPAddressSize);
	BOOL(* ResolveHostName) (LPSTR lpszHostName, LPSTR lpszIPAddress, LPDWORD lpdwIPAddressSize);
	void(* IsInNet) (LPSTR lpszIPAddress, LPSTR lpszDest, LPSTR lpszMask);
} AutoProxyHelperVtbl;

typedef struct {
	AutoProxyHelperVtbl * lpVtbl;
} AutoProxyHelperFunctions;

typedef struct {
	DWORD dwStructSize;
	LPSTR lpszScriptBuffer;
	DWORD dwScriptBufferSize;
} AUTO_PROXY_SCRIPT_BUFFER;

typedef BOOL(CALLBACK * pfnInternetInitializeAutoProxyDll) (
	DWORD dwVersion,
	LPSTR lpszDownloadedTempFile,
	LPSTR lpszMime,
	AutoProxyHelperFunctions * lpAutoProxyCallbacks,
	AUTO_PROXY_SCRIPT_BUFFER * lpAutoProxyScriptBuffer);

typedef BOOL(CALLBACK * pfnInternetDeInitializeAutoProxyDll) (
	LPSTR lpszMime,
	DWORD dwReserved);

typedef BOOL(CALLBACK * pfnInternetGetProxyInfo) (
	LPCSTR lpszUrl,
	DWORD dwUrlLength,
	LPSTR lpszUrlHostName,
	DWORD dwUrlHostNameLength,
	LPSTR * lplpszProxyHostName,
	LPDWORD lpdwProxyHostNameLength);

#define PROXY_AUTO_DETECT_TYPE_DNS_A 2
#define PROXY_AUTO_DETECT_TYPE_DHCP 1

#else

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#endif	//WIN32

#include "udp_func.h"
#include "stun_func.h"

using namespace std;

class HttpProxy {
public:

	HttpProxy();

	char * getAddress() {
		if (!_detectionDone) {
			if (getProxyAddress() == -1) {
				_address = NULL;
				_port = -1;
			}
		}
		return _address;
	}

	int getPort() {
		if (!_detectionDone) {
			if (getProxyAddress() == -1) {
				_address = NULL;
				_port = -1;
			}
		}
		return _port;
	}

private:

	int parseProxyUrl(char * url);

	int getProxyAddress();

	char * _address;

	int _port;

	bool _detectionDone;
};


HttpProxy::HttpProxy() {
	_port = -1;
	_address = NULL;
	_detectionDone = false;
}

int HttpProxy::parseProxyUrl(char * url) {
	char * tmp;

	for (tmp = url; * tmp && * tmp != ':'; tmp++);

	if (* tmp) {
		_address = (char *) malloc(tmp - url + 1);
		memcpy(_address, url, tmp - url);
		_address[tmp - url] = 0;

		for (url = ++tmp; * tmp && * tmp >= '0' && * tmp <= '9'; tmp++);
		* tmp = 0;
		_port = atoi(url);
		return 0;
	}

	return -1;
}

int HttpProxy::getProxyAddress() {
	_detectionDone = true;

#ifdef WIN32
	long ret;
	HKEY result;
	char url[1024];
	DWORD size = 1024;
	char TempPath[MAX_PATH];
	char TempFile[MAX_PATH];
	HMODULE hModJS;
	DWORD enable;
	DWORD enablesize = sizeof(DWORD);

	/* MSDN EXAMPLE */

	char url1[1025] = "http://www.google.fr/about.html";
	char host[256] = "http://www.google.fr";
	char proxyBuffer[1024];
	char * proxy = proxyBuffer;
	ZeroMemory(proxy, 1024);
	DWORD dwProxyHostNameLength = 1024;
	DWORD returnVal;
	// Declare function pointers for the three autoproxy functions
	pfnInternetInitializeAutoProxyDll pInternetInitializeAutoProxyDll;
	pfnInternetDeInitializeAutoProxyDll pInternetDeInitializeAutoProxyDll;
	pfnInternetGetProxyInfo pInternetGetProxyInfo;

	/* ************ */

	ret = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_QUERY_VALUE, & result);
	ret = RegQueryValueEx(result, "ProxyEnable", 0, 0, (LPBYTE) & enable, & enablesize);
	ret = RegQueryValueEx(result, "ProxyServer", 0, 0, (LPBYTE)url, & size);

	if (ret == ERROR_SUCCESS && enable == 1) {
		if (parseProxyUrl(url) == 0) {
			RegCloseKey(result);
			return 0;
		}
	}

	ZeroMemory(url, 1024);
	size = 1024;
	ret = RegQueryValueEx(result, "AutoConfigURL", 0, 0, (LPBYTE)url, & size);
	RegCloseKey(result);

	if (ret != ERROR_SUCCESS) {
		if (DetectAutoProxyUrl(url, size, PROXY_AUTO_DETECT_TYPE_DHCP | PROXY_AUTO_DETECT_TYPE_DNS_A) == false)  {
			return -1;
		}
	}

	GetTempPathA(sizeof(TempPath), TempPath);
	GetTempFileNameA(TempPath, NULL, 0, TempFile);
	if (URLDownloadToFileA(NULL, url, TempFile, NULL, NULL) != S_OK) {
		return -1;
	}

	if (!(hModJS = LoadLibrary("jsproxy.dll"))) {
		return -1;
	}

	if (!(pInternetInitializeAutoProxyDll = (pfnInternetInitializeAutoProxyDll)
		GetProcAddress(hModJS, "InternetInitializeAutoProxyDll")) ||
		!(pInternetDeInitializeAutoProxyDll = (pfnInternetDeInitializeAutoProxyDll)
		GetProcAddress(hModJS, "InternetDeInitializeAutoProxyDll")) ||
		!(pInternetGetProxyInfo = (pfnInternetGetProxyInfo)
		GetProcAddress(hModJS, "InternetGetProxyInfo"))) {

		return -1;
	}

	if (!(returnVal = pInternetInitializeAutoProxyDll(0, TempFile, NULL, 0, NULL))) {
		return -1;
	}

	DeleteFileA(TempFile);

	if (!pInternetGetProxyInfo((LPSTR)url1, sizeof(url1),
		(LPSTR)host, sizeof(host),
		& proxy, & dwProxyHostNameLength)) {

		return -1;
	}

	if (strncmp("PROXY ", proxy, 6) == 0) {
		if (parseProxyUrl(proxy + 6) != 0) {
			return -1;
		}
	} else {
		return -1;
	}

	if (!pInternetDeInitializeAutoProxyDll(NULL, 0)) {
		return -1;
	}
#endif

	return 0;
}

NETLIB_BOOLEAN is_tcp_port_open(const char * host, int port) {
#ifdef WIN32
	Socket localsock;
	struct sockaddr_in addr;

	localsock = socket(PF_INET, SOCK_STREAM, 0);
	if (localsock == -1) {
		cerr << "Socket() in TcpPortTest : error = " << getErrno() << endl;
		return -1;
	}

	addr.sin_port = (unsigned short) htons(port);
	addr.sin_addr.s_addr = inet_addr(host);
	addr.sin_family = PF_INET;

	if (connect(localsock, (struct sockaddr *) & addr, sizeof(addr)) == -1) {
		cerr << "Connect() in TcpPortTest : error = " << getErrno() << endl;
		closesocket(localsock);
		return false;
	}

	closesocket(localsock);
	return true;
#endif

	return -1;
}

NETLIB_BOOLEAN is_udp_port_open(const char * stun_server, int port) {
	switch (port) {
	case SIP_PORT:
		NatType natType = get_nat_type(stun_server);
		return (natType > StunTypeUnknown && natType < StunTypeBlocked ? 1 : 0);
	}

	return -1;
}

HttpProxy httpProxy;

char * get_local_http_proxy_address() {
	return httpProxy.getAddress();
}

int get_local_http_proxy_port() {
	return httpProxy.getPort();
}

NatType get_nat_type(const char * stun_server) {
	static const int CTRL_PORT = 3478;

	initNetwork();

	StunAddress4 stunServerAddr;
	stunServerAddr.addr = 0;

	StunAddress4 sAddr[1];
	sAddr[0].addr = 0;
	sAddr[0].port = 0;

	bool ret = stunParseServerName((char *) stun_server, stunServerAddr, CTRL_PORT);
	if (!ret) {
		return StunTypeFailure;
	}

	bool presPort = false;
	bool hairpin = false;
	return stunNatType(stunServerAddr, true, & presPort, & hairpin, 0, & sAddr[0]);
}
