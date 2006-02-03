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

/*
typedef int Socket;
inline int closesocket(Socket fd) {return close(fd);}
*/
#endif	//WIN32

#include "udp_func.h"
#include "stun_func.h"

#include <curl/curl.h>
#include "httptunnel.h"

#define OPT_REQ	\
"OPTIONS sip:nobody@nobody.com SIP/2.0\r\n\
Via: SIP/2.0/UDP 192.168.1.1:5060;rport;branch=123456789\r\n\
Route: <sip:voip.wengo.fr;lr>\r\n\
From: nobody <sip:nobody@nobody.com>;tag=123456789\r\n\
To: <sip:nobody@nobody.com>\r\n\
Call-ID: 000001@192.168.1.1\r\n\
CSeq: 20 OPTIONS\r\n\
Accept: application/sdp\r\n\
Content-Length: 0\r\n\r\n"

using namespace std;


typedef struct	HttpProxy_s
{
	int			port;
	char		*address;
	long		auth_type;
}				HttpProxy_t;

HttpProxy_t	_LocalProxy = {0, NULL, 0};

char *_cleanStr(char *str)
{
	int i;

	if (str && *str)
	{
		for (i = 0; str[i]; i++)
		{
			if (str[i] == '=')
				return (&str[i+1]);
		}

		return str;
	}

	return 0;
}

int _parseProxyUrl(char *url) 
{
	char * tmp;

	for (tmp = _cleanStr(url); *tmp && *tmp != ':'; tmp++);

	if (tmp && *tmp) 
	{
		_LocalProxy.address = (char *) malloc(tmp - url + 1);
		memcpy(_LocalProxy.address, url, tmp - url);
		_LocalProxy.address[tmp - url] = 0;

		for (url = ++tmp; *tmp && *tmp >= '0' && * tmp <= '9'; tmp++);
		*tmp = 0;
		_LocalProxy.port = atoi(url);
		return 0;
	}

	return -1;
}

int _getProxyAddress() 
{
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

	if (ret == ERROR_SUCCESS && enable == 1) 
	{
		if (_parseProxyUrl(url) == 0) 
		{
			RegCloseKey(result);
			return 0;
		}
	}

	ZeroMemory(url, 1024);
	size = 1024;
	ret = RegQueryValueEx(result, "AutoConfigURL", 0, 0, (LPBYTE)url, & size);
	RegCloseKey(result);

	if (ret != ERROR_SUCCESS) 
	{
		if (DetectAutoProxyUrl(url, size, PROXY_AUTO_DETECT_TYPE_DHCP | PROXY_AUTO_DETECT_TYPE_DNS_A) == false)
			return -1;
	}

	GetTempPathA(sizeof(TempPath), TempPath);
	GetTempFileNameA(TempPath, NULL, 0, TempFile);
	if (URLDownloadToFileA(NULL, url, TempFile, NULL, NULL) != S_OK)
		return -1;

	if (!(hModJS = LoadLibrary("jsproxy.dll")))
		return -1;

	if (!(pInternetInitializeAutoProxyDll = (pfnInternetInitializeAutoProxyDll)
		GetProcAddress(hModJS, "InternetInitializeAutoProxyDll")) ||
		!(pInternetDeInitializeAutoProxyDll = (pfnInternetDeInitializeAutoProxyDll)
		GetProcAddress(hModJS, "InternetDeInitializeAutoProxyDll")) ||
		!(pInternetGetProxyInfo = (pfnInternetGetProxyInfo)
		GetProcAddress(hModJS, "InternetGetProxyInfo"))) 
	{

		return -1;
	}

	if (!(returnVal = pInternetInitializeAutoProxyDll(0, TempFile, NULL, 0, NULL)))
		return -1;

	DeleteFileA(TempFile);

	if (!pInternetGetProxyInfo((LPSTR)url1, sizeof(url1),
								(LPSTR)host, sizeof(host),
								&proxy, &dwProxyHostNameLength)) 
	{
		return -1;
	}

	if (strncmp("PROXY ", proxy, 6) == 0)
	{
		if (_parseProxyUrl(proxy + 6) != 0)
			return -1;
	} 
	else
	{
		return -1;
	}

	if (!pInternetDeInitializeAutoProxyDll(NULL, 0))
		return -1;
#endif

	return 0;
}

void _get_proxy_auth_type(const char *url)
{
		CURL *curl_tmp;
		char buff[2048];
		int ret;

		ret = 0;
		curl_tmp = curl_easy_init();

		sprintf(buff, "http://%s", url);
		curl_easy_setopt(curl_tmp, CURLOPT_URL, strdup(buff));

		sprintf(buff, "%s:%d", _LocalProxy.address, _LocalProxy.port);
		curl_easy_setopt(curl_tmp, CURLOPT_PROXY, strdup(buff));

		curl_easy_setopt(curl_tmp, CURLOPT_HTTPPROXYTUNNEL, 1);
		ret = curl_easy_perform(curl_tmp);

		curl_easy_getinfo(curl_tmp, CURLINFO_PROXYAUTH_AVAIL, &(_LocalProxy.auth_type));
		
		// free the Curl tmp handle
		curl_easy_cleanup(curl_tmp);
}


NETLIB_BOOLEAN is_udp_port_opened(const char *stun_server, int port)
{
	NatType natType = get_nat_type(stun_server);
	return (natType > StunTypeUnknown && natType < StunTypeBlocked ? 1 : 0);
}

NETLIB_BOOLEAN is_local_udp_port_used(const char *itf, int port)
{
	struct sockaddr_in  raddr;
	Socket localsock;
	
	if (!itf)
		raddr.sin_addr.s_addr = htons(INADDR_ANY);
	else
		raddr.sin_addr.s_addr = inet_addr(itf);
	raddr.sin_port = htons((short)port);
	raddr.sin_family = AF_INET;
	
	if ((localsock = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
		return NETLIB_TRUE;
	
	if (bind(localsock, (struct sockaddr *)&raddr, sizeof (raddr)) < 0)
	{
		closesocket(localsock);
		return NETLIB_TRUE;
	}

	closesocket(localsock);
	return NETLIB_FALSE;
}

NETLIB_BOOLEAN sip_ping(Socket sock, int ping_timeout)
{
	fd_set rfds;
	int ret;
	char buff[1024];
	struct timeval	timeout;

	send(sock, OPT_REQ, strlen(OPT_REQ), 0);

	timeout.tv_sec = ping_timeout / 1000;
	timeout.tv_usec = (ping_timeout % 1000) * 1000;
	FD_ZERO(&rfds);
	FD_SET(sock, &rfds);

	ret = select(sock + 1, &rfds, 0, 0, &timeout);

	if (ret < 1 || !FD_ISSET(sock, &rfds))
		return NETLIB_FALSE;

	ret = recv(sock, buff, sizeof (buff), 0);
	if (ret < 1)
		return NETLIB_FALSE;
	else
		return NETLIB_TRUE;
}

NETLIB_BOOLEAN udp_sip_ping(const char *sip_server, int sip_port, int ping_timeout)
{
	Socket sock;
	struct sockaddr_in addr;
	NETLIB_BOOLEAN ret;
	int i;

	sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock <= 0)
		return -1;
	
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = inet_addr(sip_server);
	addr.sin_port = htons(sip_port);

	i = connect(sock,(struct sockaddr *)&addr, sizeof(struct sockaddr));
	if (i < 0)
	{
		closesocket(sock);
		return NETLIB_FALSE;
	}

	ret = sip_ping(sock, ping_timeout);
	closesocket(sock);
	
	return (ret ? NETLIB_TRUE : NETLIB_FALSE);
}


HttpRet is_http_conn_allowed(const char *url, 
							  const char *proxy_addr, int proxy_port, 
							  const char *proxy_login, const char *proxy_passwd,
							  NETLIB_BOOLEAN ssl)
{
	char buff[1024];
	CURL *mcurl;
	int ret;
	long http_resp_code;

	mcurl = curl_easy_init();
	
	if (ssl)
	{
		sprintf(buff, "https://%s", url);
		curl_easy_setopt(mcurl, CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv3);
		curl_easy_setopt(mcurl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(mcurl, CURLOPT_SSL_VERIFYHOST, 0);
	}
	else
		sprintf(buff, "http://%s", url);
	
	curl_easy_setopt(mcurl, CURLOPT_URL, strdup(buff));
 
	if (proxy_addr)
	{
		if (proxy_login)
		{
			if (!_LocalProxy.auth_type)
				_get_proxy_auth_type(url);

			sprintf(buff, "%s:%s", proxy_login, proxy_passwd);
			curl_easy_setopt(mcurl, CURLOPT_PROXYUSERPWD, strdup(buff));
			
			if ((_LocalProxy.auth_type & CURLAUTH_BASIC) == CURLAUTH_BASIC)
				curl_easy_setopt(mcurl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC);
			else if ((_LocalProxy.auth_type & CURLAUTH_DIGEST) == CURLAUTH_DIGEST)
				curl_easy_setopt(mcurl, CURLOPT_PROXYAUTH, CURLAUTH_DIGEST);
			else if ((_LocalProxy.auth_type & CURLAUTH_NTLM) == CURLAUTH_NTLM)
				curl_easy_setopt(mcurl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM);
		}
		sprintf(buff, "%s:%d", proxy_addr, proxy_port);
		curl_easy_setopt(mcurl, CURLOPT_PROXY, strdup(buff));
	}

	ret = curl_easy_perform(mcurl);
	curl_easy_getinfo(mcurl, CURLINFO_RESPONSE_CODE, &http_resp_code);

	curl_easy_cleanup(mcurl);
	
	if (http_resp_code == 200)
		return HTTP_OK;
	else if (http_resp_code == 407 || http_resp_code == 401)
		return HTTP_AUTH;
	else
		return HTTP_NOK;
}

HttpRet is_tunnel_conn_allowed(const char *http_gate_addr, int http_gate_port,
								const char *sip_addr, int sip_port,
								const char *proxy_addr, int proxy_port, 
								const char *proxy_login, const char *proxy_passwd,
								NETLIB_BOOLEAN ssl, NETLIB_BOOLEAN ping,
								int ping_timeout)
{

	http_sock_t *hs;
	int http_code;

	if (proxy_addr)
		http_tunnel_init_proxy(proxy_addr, proxy_port, proxy_login, proxy_passwd);

	http_tunnel_init_host(http_gate_addr, http_gate_port, ssl);
	hs = (http_sock_t *)http_tunnel_open(sip_addr, sip_port, HTTP_TUNNEL_VAR_MODE, &http_code);

	if (hs == NULL)
	{
		if (http_code == 407 || http_code == 401)
			return HTTP_AUTH;
		else
			return HTTP_NOK;
	}

	if (ping)
	{
		if (sip_ping(hs->fd, ping_timeout) == NETLIB_FALSE)
			return HTTP_NOK;
	}

	return HTTP_OK;
}


NETLIB_BOOLEAN is_proxy_auth_needed(const char *proxy_addr, int proxy_port)
{
	HttpRet ret;

	ret = is_http_conn_allowed("google.com:80", proxy_addr, proxy_port, 
								NULL, NULL, NETLIB_FALSE);

	return (ret == HTTP_AUTH ? NETLIB_TRUE : NETLIB_FALSE);
}


char *get_local_http_proxy_address() 
{
	if (_LocalProxy.address == NULL)
		_getProxyAddress();

	return _LocalProxy.address;
}

int get_local_http_proxy_port() 
{
	return _LocalProxy.port;
}

NatType get_nat_type(const char *stun_server)
{
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
