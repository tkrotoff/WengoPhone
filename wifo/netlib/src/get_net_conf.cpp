#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>   

/*
#ifdef WIN32
#include <winsock2.h>
#include <time.h>
#include <wininet.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#endif
*/


#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <wininet.h>
#include <urlmon.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include <time.h>

typedef struct 
{
  BOOL (* IsResolvable)(LPSTR lpszHost);
  DWORD (* GetIPAddress)(LPSTR lpszIPAddress,LPDWORD lpdwIPAddressSize);
  BOOL (* ResolveHostName)(LPSTR lpszHostName,LPSTR lpszIPAddress, LPDWORD lpdwIPAddressSize);
  void (* IsInNet)(LPSTR lpszIPAddress,LPSTR lpszDest,LPSTR lpszMask);
} AutoProxyHelperVtbl;

typedef struct 
{
  AutoProxyHelperVtbl *lpVtbl;
} AutoProxyHelperFunctions;

typedef struct {
  DWORD dwStructSize;
  LPSTR lpszScriptBuffer;
  DWORD dwScriptBufferSize;
} AUTO_PROXY_SCRIPT_BUFFER;

typedef BOOL (CALLBACK *pfnInternetInitializeAutoProxyDll)(
    DWORD dwVersion,
    LPSTR lpszDownloadedTempFile,
    LPSTR lpszMime,
    AutoProxyHelperFunctions* lpAutoProxyCallbacks,
    AUTO_PROXY_SCRIPT_BUFFER* lpAutoProxyScriptBuffer );

typedef BOOL (CALLBACK *pfnInternetDeInitializeAutoProxyDll)(
	LPSTR lpszMime,
	DWORD dwReserved );

typedef BOOL (CALLBACK *pfnInternetGetProxyInfo)(
	LPCSTR lpszUrl,
	DWORD dwUrlLength,
	LPSTR lpszUrlHostName,
	DWORD dwUrlHostNameLength,
	LPSTR* lplpszProxyHostName,
	LPDWORD lpdwProxyHostNameLength);


#define PROXY_AUTO_DETECT_TYPE_DNS_A 2
#define PROXY_AUTO_DETECT_TYPE_DHCP  1

#else
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#endif

#define CTRL_PORT	3478
#define AUDIO_PORT	3479
#define	HTTP_PORT	80
#define HTTPS_PORT	443


#include "udp_func.h"
#include "stun_func.h"
#include "get_net_conf.h"


using namespace std;

int		GetNatType(const char *host, const int port)
{
	StunAddress4 sAddr[1];
	StunAddress4 stunServerAddr;
	bool presPort = false;
    bool hairpin = false;
	
	stunServerAddr.addr = 0;
	sAddr[0].addr = 0; 
    sAddr[0].port = 0; 

	bool ret = stunParseServerName((char *) host, stunServerAddr, port);
	if (ret == false)	
		return -1;

	NatType stype = stunNatType(stunServerAddr, true, &presPort, &hairpin, 0, &sAddr[0]);

	return stype;
}

int ParseProxyUrl(char *url, net_info_t *ptr)
{
	char *tmp;

	for (tmp = url; *tmp && *tmp != ':'; tmp++);

	if (*tmp)
	{
		ptr->Proxy_IP = (char *) malloc(tmp - url + 1);
		memcpy(ptr->Proxy_IP, url, tmp - url);
		ptr->Proxy_IP[tmp - url] = 0;

		for (url = ++tmp; *tmp && *tmp >= '0' && *tmp <= '9'; tmp++);
		*tmp = 0;
		ptr->Proxy_Port = atoi(url);
		return 0;
	}

	return -1;
}

#ifdef WIN32

int		GetProxyAddr(net_info_t *ptr)
{
	long	ret;
	HKEY	result;
	char	url[1024];
	DWORD 	size = 1024;
	char	TempPath[MAX_PATH];
	char	TempFile[MAX_PATH];
	HMODULE	hModJS;
	DWORD	enable;
	DWORD	enablesize = sizeof(DWORD);

	/* EXEMPLE MSDN */	
	char url1[1025] = "http://www.google.fr/about.html";
	char host[256] = "http://www.google.fr";
	char proxyBuffer[1024];
	char *proxy = proxyBuffer;
	ZeroMemory( proxy, 1024 );
	DWORD dwProxyHostNameLength = 1024;
	DWORD returnVal;
	// Declare function pointers for the three autoproxy functions
	pfnInternetInitializeAutoProxyDll    pInternetInitializeAutoProxyDll;
	pfnInternetDeInitializeAutoProxyDll  pInternetDeInitializeAutoProxyDll;
	pfnInternetGetProxyInfo              pInternetGetProxyInfo;
	/* ************ */

	ret = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings", 0, KEY_QUERY_VALUE, &result);
	ret = RegQueryValueEx(result, "ProxyEnable", 0, 0, (LPBYTE) &enable, &enablesize);
	ret = RegQueryValueEx(result, "ProxyServer", 0, 0, (LPBYTE) url, &size);

	if (ret == ERROR_SUCCESS && enable == 1)
		if (ParseProxyUrl(url, ptr) == 0)
		{
			RegCloseKey(result);
				return 0;
		}

	ZeroMemory(url, 1024);
	size = 1024;
	ret = RegQueryValueEx(result, "AutoConfigURL", 0, 0, (LPBYTE) url, &size);
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

	if( !( pInternetInitializeAutoProxyDll = (pfnInternetInitializeAutoProxyDll)
				GetProcAddress( hModJS, "InternetInitializeAutoProxyDll" ) ) ||
      !( pInternetDeInitializeAutoProxyDll = (pfnInternetDeInitializeAutoProxyDll)
                GetProcAddress( hModJS, "InternetDeInitializeAutoProxyDll" ) ) ||
      !( pInternetGetProxyInfo = (pfnInternetGetProxyInfo)
                GetProcAddress( hModJS, "InternetGetProxyInfo" ) ) )
		return -1;

	if( !( returnVal = pInternetInitializeAutoProxyDll( 0, TempFile, NULL, 
                                                        0, NULL ) ) )
		return -1;

	DeleteFileA( TempFile );

	if(!pInternetGetProxyInfo( (LPSTR) url1,  sizeof( url1 ), 
                              (LPSTR) host, sizeof( host ),
                              &proxy, &dwProxyHostNameLength ) )
		return -1;

	if (strncmp("PROXY ", proxy, 6) == 0)
	{
		if (ParseProxyUrl(proxy + 6, ptr) != 0)
			return -1;
	}
	else
		return -1;
		
	if( !pInternetDeInitializeAutoProxyDll( NULL, 0 ) )
		return -1;

	return 0;
}

#else

int		GetProxyAddr(net_info_t *ptr)
{
	return 0;
}
#endif

int		TcpPortTest(const char *host, const int port)
{
	Socket localsock;
	struct sockaddr_in	addr;

	localsock = socket(PF_INET, SOCK_STREAM, 0);
	if (localsock == -1)
	{
		cerr << "Socket() in TcpPortTest : error = " << getErrno() << endl;
		return -1;
	}

	addr.sin_port = (unsigned short) htons(port);
	addr.sin_addr.s_addr = inet_addr(host);
	addr.sin_family = PF_INET;

	if (connect(localsock, (struct sockaddr *) &addr, sizeof(addr)) == -1)
    {
		cerr << "Connect() in TcpPortTest : error = " << getErrno() << endl;
		closesocket(localsock);
		return 0;
    }

	closesocket(localsock);
	return 1;
}

void	InitStructNetInfo(net_info_t *ptr)
{
	ptr->Audio_Port = 0;
	ptr->SIP_Port = 0;
	ptr->Http_Port = 0;
	ptr->Https_Port = 0;
	ptr->NatType = StunTypeUnknown;
	ptr->Proxy_IP = 0;
	ptr->Proxy_Port = 0;
}

extern "C" {

void	GetNetConf(net_info_t *ptr, const char *StunServer)
{
	initNetwork();
	InitStructNetInfo(ptr);

	if (StunServer) {
		ptr->NatType = (NatType) GetNatType(StunServer, CTRL_PORT);
		ptr->SIP_Port = ptr->NatType > StunTypeUnknown && ptr->NatType < StunTypeBlocked ? 1 : 0;
	}
/*
	ptr->NatType = (NatType) GetNatType(StunServer, AUDIO_PORT);
	ptr->Audio_Port = 0;//ptr->NatType > StunTypeUnknown && ptr->NatType < StunTypeBlocked ? 1 : 0;
	ptr->Http_Port = 0; //TcpPortTest("80.118.132.75", HTTP_PORT);
	ptr->Https_Port = 0; //TcpPortTest("80.118.132.75", HTTPS_PORT);	
*/	
	GetProxyAddr(ptr);
}

}
