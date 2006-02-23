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

#ifndef snprintf
#define snprintf _snprintf
#endif

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

inline int strncasecmp(const char *str1, const char *str2, int size) {return strnicmp(str1, str2, size);}

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
#include <netlib.h>
#include "netlib_util.h"


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
	long		proxy_auth_type;
	char		**proxy_exceptions;
}				HttpProxy_t;

HttpProxy_t	_LocalProxy = {0, NULL, 0, 0};

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

	for (tmp = _cleanStr(url), url = tmp; *tmp && *tmp != ':'; tmp++);

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

void _get_proxy_auth_type(const char *url, int timeout)
{
		CURL *curl_tmp;
		char buff[2048];
		int ret;

		ret = 0;
		curl_tmp = curl_easy_init();

		snprintf(buff, sizeof(buff), "http://%s", url);
		curl_easy_setopt(curl_tmp, CURLOPT_URL, strdup(buff));

		snprintf(buff, sizeof(buff), "%s:%d", _LocalProxy.address, _LocalProxy.port);
		curl_easy_setopt(curl_tmp, CURLOPT_PROXY, strdup(buff));

		if (timeout > 0)
			curl_easy_setopt(curl_tmp, CURLOPT_TIMEOUT, timeout);

		curl_easy_setopt(curl_tmp, CURLOPT_HTTPPROXYTUNNEL, 1);
		ret = curl_easy_perform(curl_tmp);

		curl_easy_getinfo(curl_tmp, CURLINFO_PROXYAUTH_AVAIL, &(_LocalProxy.proxy_auth_type));
		
		// free the Curl tmp handle
		curl_easy_cleanup(curl_tmp);
}

void _get_auth_type(const char *url, int timeout)
{
		CURL *curl_tmp;
		char buff[2048];
		int ret;

		ret = 0;
		curl_tmp = curl_easy_init();

		snprintf(buff, sizeof(buff), "http://%s", url);
		curl_easy_setopt(curl_tmp, CURLOPT_URL, strdup(buff));

		if (timeout > 0)
			curl_easy_setopt(curl_tmp, CURLOPT_TIMEOUT, timeout);

		ret = curl_easy_perform(curl_tmp);

		curl_easy_getinfo(curl_tmp, CURLINFO_HTTPAUTH_AVAIL, &(_LocalProxy.auth_type));
		
		// free the Curl tmp handle
		curl_easy_cleanup(curl_tmp);
}

char **internet_explorer_proxyless_exception_list()
{
#ifdef WIN32
	char **list;
	long ret;
	HKEY result;
	char buff[1024];
	DWORD buff_size = sizeof(buff);

	ret = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_QUERY_VALUE, &result);
	ret = RegQueryValueEx(result, "ProxyOverride", 0, 0, (LPBYTE)buff, &buff_size);
	RegCloseKey(result);

	list = my_split(buff, ';');
	return list;
#else
	return 0;
#endif
}

NETLIB_BOOLEAN is_url_proxyless_exception(const char *url)
{
	int i;

	if (!url)
		return NETLIB_FALSE;

	if (!_LocalProxy.proxy_exceptions)
		_LocalProxy.proxy_exceptions = internet_explorer_proxyless_exception_list();

	if (!_LocalProxy.proxy_exceptions)
		return NETLIB_FALSE;

	for (i = 0; _LocalProxy.proxy_exceptions[i]; i++)
	{
		if (domain_url_cmp((char *)url, _LocalProxy.proxy_exceptions[i]) == false)
		{
			return NETLIB_TRUE;
		}
	}

	return NETLIB_FALSE;
}

NETLIB_BOOLEAN is_udp_port_opened(const char *stun_server, int port, NatType *nType)
{
	*nType = get_nat_type(stun_server);
	return (*nType > StunTypeUnknown && *nType < StunTypeBlocked ? 
			NETLIB_TRUE : NETLIB_FALSE);
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

int get_local_free_udp_port(const char *itf)
{
	struct sockaddr_in  raddr;
	struct sockaddr_in name;
	int name_size = sizeof (struct sockaddr_in);
	Socket localsock;
	
	if (!itf)
		raddr.sin_addr.s_addr = htons(INADDR_ANY);
	else
		raddr.sin_addr.s_addr = inet_addr(itf);
	raddr.sin_port = htons(0);
	raddr.sin_family = AF_INET;
	
	if ((localsock = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
		return -1;
	
	if (bind(localsock, (struct sockaddr *)&raddr, sizeof (raddr)) < 0)
	{
		closesocket(localsock);
		return -1;
	}

	if (getsockname(localsock, (struct sockaddr *) &name, (socklen_t *)&name_size) < 0)
	{
		closesocket(localsock);
		return -1;
	}

	closesocket(localsock);
	return ntohs(name.sin_port);
}


NETLIB_BOOLEAN sip_ping(Socket sock, int ping_timeout)
{
	fd_set rfds;
	int ret;
	char buff[1024];
	struct timeval	timeout;

	send(sock, OPT_REQ, strlen(OPT_REQ), 0);

	timeout.tv_sec = ping_timeout;
	timeout.tv_usec = 0;
	FD_ZERO(&rfds);
	FD_SET(sock, &rfds);

	ret = select(sock + 1, &rfds, 0, 0, &timeout);

	if (ret < 1 || !FD_ISSET(sock, &rfds))
		return NETLIB_FALSE;

	ret = recv(sock, buff, sizeof (buff), 0);
	if (ret < 1)
	{
#if defined(WIN32)
		ret = GetLastError();
#else
		ret = errno;
#endif
		return NETLIB_FALSE;
	}
	else
		return NETLIB_TRUE;
}

NETLIB_BOOLEAN sip_ping2(http_sock_t *hs, int ping_timeout)
{
	fd_set rfds;
	int ret;
	char buff[1024];
	struct timeval	timeout;

	http_tunnel_send(hs, OPT_REQ, strlen(OPT_REQ));

	timeout.tv_sec = ping_timeout;
	timeout.tv_usec = 0;
	FD_ZERO(&rfds);
	FD_SET(hs->fd, &rfds);

	ret = select(hs->fd + 1, &rfds, 0, 0, &timeout);

	if (ret < 1 || !FD_ISSET(hs->fd, &rfds))
		return NETLIB_FALSE;

	ret = http_tunnel_recv(hs, buff, sizeof (buff));
	if (ret < 1)
	{
#if defined(WIN32)
		ret = GetLastError();
#else
		ret = errno;
#endif
		return NETLIB_FALSE;
	}
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

NETLIB_BOOLEAN is_https(const char *url)
{
	if (strncasecmp(url, "https", 5) == 0)
		return NETLIB_TRUE;
	else
		return NETLIB_FALSE;
}

HttpRet is_http_conn_allowed(const char *url, 
							  const char *proxy_addr, int proxy_port, 
							  const char *proxy_login, const char *proxy_passwd,
							  NETLIB_BOOLEAN ssl, int timeout)
{
	char buff[1024];
	CURL *mcurl;
	int ret;
	long http_resp_code;
	char *redir_tmp;
	char *redir_url;

	mcurl = curl_easy_init();
	
	if (ssl)
	{
		snprintf(buff, sizeof(buff), "https://%s", url);
		curl_easy_setopt(mcurl, CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv3);
		curl_easy_setopt(mcurl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(mcurl, CURLOPT_SSL_VERIFYHOST, 0);
	}
	else
		snprintf(buff, sizeof(buff), "http://%s", url);
	
	curl_easy_setopt(mcurl, CURLOPT_URL, strdup(buff));

	if (timeout > 0)
		curl_easy_setopt(mcurl, CURLOPT_TIMEOUT, timeout);

	/* FOLLOW REDIRECTION */
	curl_easy_setopt(mcurl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(mcurl, CURLOPT_UNRESTRICTED_AUTH, 1);
	/* ****************** */
	curl_easy_setopt(mcurl, CURLOPT_VERBOSE, 1);
 
	if (proxy_addr)
	{
		if (proxy_login)
		{
			if (!_LocalProxy.proxy_auth_type)
				_get_proxy_auth_type(url, timeout);

			snprintf(buff, sizeof(buff), "%s:%s", proxy_login, proxy_passwd);
			curl_easy_setopt(mcurl, CURLOPT_PROXYUSERPWD, strdup(buff));
			
			if ((_LocalProxy.proxy_auth_type & CURLAUTH_BASIC) == CURLAUTH_BASIC)
				curl_easy_setopt(mcurl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC);
			else if ((_LocalProxy.proxy_auth_type & CURLAUTH_DIGEST) == CURLAUTH_DIGEST)
				curl_easy_setopt(mcurl, CURLOPT_PROXYAUTH, CURLAUTH_DIGEST);
			else if ((_LocalProxy.proxy_auth_type & CURLAUTH_NTLM) == CURLAUTH_NTLM)
				curl_easy_setopt(mcurl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM);
		}
		snprintf(buff, sizeof(buff), "%s:%d", proxy_addr, proxy_port);
		curl_easy_setopt(mcurl, CURLOPT_PROXY, strdup(buff));
	}
	else
	{
		if (proxy_login)
		{
			if (!_LocalProxy.auth_type)
				_get_auth_type(url, timeout);

			snprintf(buff, sizeof(buff), "%s:%s", proxy_login, proxy_passwd);
			curl_easy_setopt(mcurl, CURLOPT_USERPWD, strdup(buff));

			if ((_LocalProxy.proxy_auth_type & CURLAUTH_BASIC) == CURLAUTH_BASIC)
				curl_easy_setopt(mcurl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
			else if ((_LocalProxy.proxy_auth_type & CURLAUTH_DIGEST) == CURLAUTH_DIGEST)
				curl_easy_setopt(mcurl, CURLOPT_HTTPAUTH, CURLAUTH_DIGEST);
			else if ((_LocalProxy.proxy_auth_type & CURLAUTH_NTLM) == CURLAUTH_NTLM)
				curl_easy_setopt(mcurl, CURLOPT_HTTPAUTH, CURLAUTH_NTLM);
		}

	}

	ret = curl_easy_perform(mcurl);
	curl_easy_getinfo(mcurl, CURLINFO_RESPONSE_CODE, &http_resp_code);

	curl_easy_getinfo(mcurl, CURLINFO_EFFECTIVE_URL, &redir_tmp);

	redir_url = strdup(redir_tmp);

	curl_easy_cleanup(mcurl);

	if ((http_resp_code / 100) == 3)
	{
		NETLIB_BOOLEAN is_ssl;
		char *tmp;

		if ((is_ssl= is_https(redir_url)) == NETLIB_TRUE)
			tmp = redir_url + 8;
		else
			tmp = redir_url + 7;

		if (is_url_proxyless_exception(tmp))
		{
			return is_http_conn_allowed(tmp, NULL, 0, 
										proxy_login, proxy_passwd, is_ssl, timeout);
		}
		else
		{
			return is_http_conn_allowed(tmp, proxy_addr, proxy_port, 
										proxy_login, proxy_passwd, is_ssl, timeout);
		}
	}
	
	if (http_resp_code == 200)
		return HTTP_OK;
	else if (http_resp_code != 404 && http_resp_code != 200)
		return HTTP_AUTH;
	else
		return HTTP_NOK;
}

HttpRet is_tunnel_conn_allowed(const char *http_gate_addr, int http_gate_port,
								const char *sip_addr, int sip_port,
								const char *proxy_addr, int proxy_port, 
								const char *proxy_login, const char *proxy_passwd,
								NETLIB_BOOLEAN ssl, int timeout,
								NETLIB_BOOLEAN ping, int ping_timeout)
{

	http_sock_t *hs;
	int http_code;

	if (proxy_addr)
		http_tunnel_init_proxy(proxy_addr, proxy_port, proxy_login, proxy_passwd);

	http_tunnel_init_host(http_gate_addr, http_gate_port, ssl);
	hs = (http_sock_t *)http_tunnel_open(sip_addr, sip_port, HTTP_TUNNEL_VAR_MODE, &http_code, timeout);

	if (hs == NULL)
	{
		if (http_code == 404)
			return HTTP_NOK;
		else
			return HTTP_AUTH;
	}

	if (ping)
	{
		if (sip_ping2(hs, ping_timeout) == NETLIB_FALSE)
			return HTTP_NOK;
	}

	return HTTP_OK;
}


NETLIB_BOOLEAN is_proxy_auth_needed(const char *proxy_addr, int proxy_port, int timeout)
{
	HttpRet ret;

	if (_LocalProxy.auth_type)
		return NETLIB_TRUE;

	ret = is_http_conn_allowed("www.google.com:80", proxy_addr, proxy_port, 
								NULL, NULL, NETLIB_FALSE, timeout);

	return (ret == HTTP_AUTH ? NETLIB_TRUE : NETLIB_FALSE);
}

NETLIB_BOOLEAN is_proxy_auth_ok(const char *proxy_addr, int proxy_port,
								const char *proxy_login, const char *proxy_passwd,
								int timeout)
{
	HttpRet ret;

	ret = is_http_conn_allowed("www.google.com:80", proxy_addr, proxy_port, 
								proxy_login, proxy_passwd, 
								NETLIB_FALSE, timeout);

	return (ret == HTTP_OK ? NETLIB_TRUE : NETLIB_FALSE);
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
