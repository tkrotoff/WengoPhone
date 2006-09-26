#ifndef	_HTTPTUNNEL_H_
#define	_HTTPTUNNEL_H_

/* ***** SSL ***** */
#include <openssl/ssl.h>
#include <openssl/pem.h>
#include <openssl/err.h>
/* *************** */

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
	int		fd;
	int		mode;
	int		send_size;
	int		recv_size;
	void	*curl;
	SSL		*s_ssl;
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
void* http_tunnel_open(const char *host, int port, int mode, int *http_code, int timeout);
int http_tunnel_close(void *h_tunnel);
void http_tunnel_init_host(const char *hostname, int port, int ssl);
void http_tunnel_init_proxy(const char *hostname, int port, const char *username, const char *password);
void http_tunnel_clean_up();
#ifdef __cplusplus
}
#endif

#endif
