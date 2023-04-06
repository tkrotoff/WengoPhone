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

#ifndef _ENVDISCOVERY_
#define _ENVDISCOVERY_

typedef		struct net_info_s
{
	int		NatType;
	int		SIP_Port;
	int		Audio_Port;
	int		Http_Port;
	int		Https_Port;
	char		*Proxy_IP;
	int		Proxy_Port;
}			net_info_t;

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

#ifdef __cplusplus
extern "C" {
#endif

void	GetNetConf(net_info_t *ptr, const char *StunServer);

#ifdef __cplusplus
}
#endif


#endif
