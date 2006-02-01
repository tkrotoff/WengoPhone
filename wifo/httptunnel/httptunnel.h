#ifndef	_HTTPTUNNEL_H_
#define	_HTTPTUNNEL_H_

#if defined(WIN32)
	#include <windows.h>
	//#pragma comment(lib, "ws2_32.lib")
	#define FCNTL_BLOCK(fd)		{u_long arg1 = 0;\
								ioctlsocket(fd, FIONBIO, &arg1);}
	#define FCNTL_NOBLOCK(fd)	{u_long arg2 = 1;\
								ioctlsocket(fd, FIONBIO, &arg2);}

	#define ERR_SOCK(err)	(err == WSAENETDOWN ||\
							 err == WSAEHOSTUNREACH ||\
							 err == WSAENETRESET ||\
							 err == WSAENOTCONN ||\
							 err == WSAESHUTDOWN ||\
							 err == WSAECONNABORTED ||\
							 err == WSAECONNRESET ||\
							 err == WSAETIMEDOUT)
#else
	#include <unistd.h>
	#include <sys/socket.h>
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <errno.h>
	#define FCNTL_BLOCK(fd)		{int flags1 = fcntl(fd, F_GETFL, 0);\
								fcntl(fd, F_SETFL, flags1 & ~O_NONBLOCK);}

	#define FCNTL_NOBLOCK(fd)	{int flags2 = fcntl(fd, F_GETFL, 0);\
								fcntl(fd, F_SETFL, flags2 | O_NONBLOCK);}


	#define WSAEWOULDBLOCK 	EAGAIN
	#define ERR_SOCK(err)	(ECONNREFUSED || ENOTCONN)

#endif

//#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char	*httpServerIP;
extern int	httpServerPort;
extern char	*proxyServerIP;
extern int	proxyServerPort;

extern int	UseProxy;

#define	HTTP_TUNNEL_FIXE_MODE	1
#define HTTP_TUNNEL_VAR_MODE	0

#define MAX_SIZE	10000
#define HTTP_TUNNEL_ERR_UNKNOW	-1
#define HTTP_TUNNEL_ERR_DECONN	-2
#define HTTP_TUNNEL_ERR_BUF2SMA	-3

typedef struct	http_sock
{
	int	fd;
	int	mode;
	int	send_size;
	int	recv_size;
	void 	*curl;
}				http_sock_t;

typedef struct	http_distant_host
{
	const char *host;
	int	port;
}				http_distant_host_t;



//<MINHPQ>
typedef enum {
	SOCK_MODE_UDP_NO_TUNNEL,
	SOCK_MODE_HTTP_TUNNEL
} tunnel_sock_mode_t;
//</MINHPQ>


int http_tunnel_get_socket(void *h_tunnel);
int http_tunnel_recv(void *h_tunnel, void *buffer, int size);
int	http_tunnel_send(void *h_tunnel, const void *buffer, int size);
void* http_tunnel_open(const char *host, int port, int mode, int *httpcode);
int http_tunnel_close(void *h_tunnel);
void* http_tunnel_open_with_existing_sock(const char *host, int port, int mode, int sock);
void http_tunnel_init_host(const char *hostname, int port, NETLIB_BOOLEAN ssl);
void http_tunnel_init_proxy(const char *hostname, int port, const char *username, const char *password);
#ifdef __cplusplus
}
#endif

#endif
