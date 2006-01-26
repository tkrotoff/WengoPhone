#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include <curl/curl.h>
#include "httptunnel.h"

#include <string.h>

#ifndef WIN32
inline int Sleep(int a) {return usleep(a * 1000);}
inline int closesocket(int sock) {return close(sock);}
inline int WSAGetLastError() {return errno;}
inline int GetLastError() {return errno;}
#define SOCKET int
#endif

#ifdef __cplusplus
extern "C" {
#endif

char	*httpServerIP;
int		httpServerPort;
char	*proxyServerIP;
int		proxyServerPort;
long	proxyAuthType;

static char	*proxyUsername;
static char	*proxyPassword;

int	UseProxy = 0;
int UseSSL = 0;

int get_ip_addr(char * outbuf, int size, const char * hostname)
{
	struct addrinfo aiHints;
	struct addrinfo *res;

	if (size < 16) {
		return -1;
	}
	// *** Get IP of host ***
	memset(&aiHints, 0, sizeof(aiHints));
	aiHints.ai_family = AF_INET;
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;
	/* //// resolution DNS server SIP //// */

	if (getaddrinfo(hostname,NULL,&aiHints,&res) != 0) {
		return -1;
	}
	if (!res) {
		return -1;
	}

	strncpy(outbuf, inet_ntoa( ((struct sockaddr_in *)res->ai_addr)->sin_addr), size); 
	freeaddrinfo(res);
	return 0;
}


void http_tunnel_init_host(const char *hostname, int port)
{
	char hostIP[20];

	get_ip_addr(hostIP, sizeof(hostIP), hostname);
	httpServerIP = strdup(hostIP);
	httpServerPort = port;
}

void http_tunnel_init_proxy(const char *hostname, int port, const char *username, const char *password)
{
	char hostIP[20];

	proxyServerIP = 0;
	proxyServerPort = 0;
	proxyAuthType = 0;
	UseProxy = 0;

	if (hostname && (strlen(hostname) > 0)) 
	{
		if (get_ip_addr(hostIP, sizeof(hostIP), hostname) < 0) 
		{
			return;
		}
		proxyServerIP = strdup(hostIP);
		proxyServerPort = port;
		UseProxy = 1;
	}

	if (username)
		proxyUsername = strdup(username);

	if (password)
		proxyPassword = strdup(password);
}

int http_tunnel_get_socket(void *h_tunnel)
{
	struct http_sock *hs;

	if (!h_tunnel)
		return -1;
	hs = (struct http_sock *) h_tunnel;
	return hs->fd;
}

static SOCKET easy_get_sock(CURL *curl)
{
  SOCKET   fd = (SOCKET) -1;

  curl_easy_getinfo(curl, CURLINFO_HTTP_SOCKET, &fd);

  return fd;
}

int	get_httpresponse2(http_sock_t *hs, char *buff, int buffsize, int ssl)
{
	struct timeval	timeout;
	fd_set	rfds;
	int ret;
	int nbytes = 0;

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	while (1)
	{

		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_SET(hs->fd, &rfds);

		ret = select(hs->fd + 1, &rfds, 0, 0, &timeout);
		
		if (ret <= 0)
			return -1;

		if (FD_ISSET(hs->fd, &rfds))
		{
			ret = recv(hs->fd, buff + nbytes, 1, 0);
			
			if (ret < 0)
				return -1;
			else if (ret == 0)
				return nbytes;
			else
				nbytes += ret;

			if (nbytes == buffsize)
				return nbytes;
			
			if (nbytes > 3 && strncmp("\r\n\r\n", buff + nbytes - 4, 4) == 0)
				break;
		}
	}

	return nbytes;
}

void get_proxy_auth_type()
{
		CURL *curl_tmp;
		char buff[2048];
		int ret;

		ret = 0;
		curl_tmp = curl_easy_init();

		sprintf(buff, "http://%s:%d", httpServerIP, httpServerPort);
		curl_easy_setopt(curl_tmp, CURLOPT_URL, strdup(buff));

		sprintf(buff, "%s:%d", proxyServerIP, proxyServerPort);
		curl_easy_setopt(curl_tmp, CURLOPT_PROXY, strdup(buff));

		curl_easy_setopt(curl_tmp, CURLOPT_HTTPPROXYTUNNEL, 1);
		ret = curl_easy_perform(curl_tmp);

		curl_easy_getinfo(curl_tmp, CURLINFO_PROXYAUTH_AVAIL, &proxyAuthType);
		
		// free the Curl tmp handle
		curl_easy_cleanup(curl_tmp);
}

void *http_tunnel_open(const char *host, int port, int mode)
{
	struct sockaddr_in	addr;
	char query[512];
	char buff[2048];
	char ipaddr[20];
	int nbytes, ret;
	http_sock_t *hs;
	//AuthParam_t ap;
	struct curl_slist *slist=NULL;
	SOCKET sock;

	// SETSOCKOPT VALUE
	int buffsize = 256000;

	hs = (http_sock_t *) malloc(sizeof(http_sock_t));
	if (!hs) 
		return NULL;

	memset(hs, 0, sizeof(http_sock_t));
	memset(ipaddr, 0, sizeof(ipaddr));

	// *** Get IP of host ***
	get_ip_addr(ipaddr, sizeof(ipaddr), host);		

	hs->mode = mode;
	hs->send_size = 0;
	hs->recv_size = 0;


	if (UseProxy == 1)
	{
		if (proxyAuthType == 0)
			get_proxy_auth_type();

		ret = 0;
		hs->curl = curl_easy_init();

		if(hs->curl) 
		{
			curl_easy_setopt(hs->curl, CURLOPT_VERBOSE, 1);

			/* ********* SSL ********* */
			if (UseSSL)
			{
				curl_easy_setopt(hs->curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_SSLv3);
				curl_easy_setopt(hs->curl, CURLOPT_SSL_VERIFYPEER, 0);
				curl_easy_setopt(hs->curl, CURLOPT_SSL_VERIFYHOST, 0);
				sprintf(buff, "https://%s:%d", httpServerIP, httpServerPort);
			}
			/* *********************** */
			else
			{
				sprintf(buff, "http://%s:%d", httpServerIP, httpServerPort);
			}

			curl_easy_setopt(hs->curl, CURLOPT_URL, strdup(buff));

			sprintf(buff, "%s:%d", proxyServerIP, proxyServerPort);
			curl_easy_setopt(hs->curl, CURLOPT_PROXY, strdup(buff));

			if (proxyAuthType)
			{
				sprintf(buff, "%s:%s", proxyUsername, proxyPassword);
				curl_easy_setopt(hs->curl, CURLOPT_PROXYUSERPWD, strdup(buff));
				
				if ((proxyAuthType & CURLAUTH_BASIC) == CURLAUTH_BASIC)
					curl_easy_setopt(hs->curl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC);
				else if ((proxyAuthType & CURLAUTH_DIGEST) == CURLAUTH_DIGEST)
					curl_easy_setopt(hs->curl, CURLOPT_PROXYAUTH, CURLAUTH_DIGEST);
				else if ((proxyAuthType & CURLAUTH_NTLM) == CURLAUTH_NTLM)
					curl_easy_setopt(hs->curl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM);
			}

			curl_easy_setopt(hs->curl, CURLOPT_HTTPPROXYTUNNEL, 1);

			sprintf(buff, "UdpHost: %s:%d", ipaddr, port);
			slist = curl_slist_append(slist, strdup(buff));
			
			slist = curl_slist_append(slist, "Connection: Keep-Alive");  
			slist = curl_slist_append(slist, "Pragma: no-cache");
			slist = curl_slist_append(slist, "Cache-Control: no-cache");
			curl_easy_setopt(hs->curl, CURLOPT_HTTPHEADER, slist);

			// curl_easy_use_mysock(hs->fd);
			ret = curl_easy_perform(hs->curl);
			if (ret != 0)
			{
				curl_easy_cleanup(hs->curl);
				free(hs);
				return NULL;
			}

			curl_slist_free_all(slist);

			hs->fd = easy_get_sock(hs->curl);
		}
		else
		{
			free(hs);
			return NULL;
		}

		setsockopt(hs->fd, SOL_SOCKET, SO_RCVBUF, (const char *) &buffsize, sizeof(buffsize));
		setsockopt(hs->fd, SOL_SOCKET, SO_SNDBUF, (const char *) &buffsize, sizeof(buffsize));

		return hs;
	}
	else
	{

		sock = socket(PF_INET, SOCK_STREAM, 0);
		if (sock == -1)
		  {
		    free(hs);
		    return NULL;
		  }

		hs->fd = sock;
		addr.sin_port = (unsigned short) htons(httpServerPort);
		addr.sin_addr.s_addr = inet_addr(httpServerIP);
		addr.sin_family = PF_INET;

		if (connect(hs->fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
		{	
			//ret = GetLastError();
			perror("connect");
			closesocket(hs->fd);
			free(hs);
			return NULL;
		}

		if (mode == HTTP_TUNNEL_FIXE_MODE)
			sprintf(query, "GET / HTTP/1.1\r\nUdpHost: %s:%d\r\nMode: 1\r\n\r\n", ipaddr, port);
		else
			sprintf(query, "GET / HTTP/1.1\r\nUdpHost: %s:%d\r\n\r\n", ipaddr, port);

		nbytes = send(hs->fd, query, (int) strlen(query), 0);

		if (nbytes < 0)
		{
			closesocket(hs->fd);
			free(hs);
			return NULL;
		}

		nbytes = get_httpresponse2(hs, buff, 511, UseSSL);

		if (nbytes > 0)
			buff[nbytes] = 0;
		else
		{
			closesocket(hs->fd);
			free(hs);
			return NULL;
		}

		if (strncmp(buff, "HTTP/1.0 200 OK\r\n", 17) == 0 || strncmp(buff, "HTTP/1.1 200 OK\r\n", 17) == 0)
		{
			setsockopt(hs->fd, SOL_SOCKET, SO_RCVBUF, (const char *) &buffsize, sizeof(buffsize));
			setsockopt(hs->fd, SOL_SOCKET, SO_SNDBUF, (const char *) &buffsize, sizeof(buffsize));
			return hs;
		}
		else
		{
			closesocket(hs->fd);
		    free(hs);
		    return NULL;
		}
	}

	free(hs);
	return NULL;
}



int http_tunnel_close(void *h_tunnel)
{
	http_sock_t *hs;

	if (!h_tunnel)
		return -1;
	hs = (http_sock_t *) h_tunnel;
//	closesocket(hs->fd);
	if (hs->curl) {
		curl_easy_cleanup((CURL *)hs->curl);
	}
	else {
		closesocket(hs->fd);
	}
	free(hs);
	return 0;
}

int	http_tunnel_send(void *h_tunnel, const void *buffer, int size)
{
	int		send_bytes = 0;
	int		total = 0;
	char	ptr[10000];
	void	*ptr2 = 0;
	int		size2send = 0;
	int		err = 0;
	int		size_unknown = 0;
	fd_set	ofds;
	struct http_sock *hs;

	hs = (http_sock_t *) h_tunnel;

	if (!h_tunnel)
	  return -1;

	if (size > MAX_SIZE)
	{
		return HTTP_TUNNEL_ERR_DECONN;
	}

	ptr2 = (void *)buffer;
	size2send = hs->send_size;

	FD_ZERO(&ofds);
	FD_SET(hs->fd, &ofds);

	do
	{
		if (FD_ISSET(hs->fd, &ofds))
		{
			if (hs->send_size == 0)
			{	
				size_unknown = 1;
				hs->send_size = size;
				//ptr = malloc(size + sizeof(size));
				memcpy(ptr, &size, sizeof(size));
				memcpy(ptr + sizeof(size), buffer, size);
				ptr2 = ptr;
				size2send = size + sizeof(size);
			}

			send_bytes = send(hs->fd, (char *) ptr2, size2send, 0);

			if (send_bytes < 0)
			{
				err = WSAGetLastError();
				if (ERR_SOCK(err))
				{
					return HTTP_TUNNEL_ERR_DECONN;
				}
				else
					if (err != WSAEWOULDBLOCK)
						return HTTP_TUNNEL_ERR_UNKNOW;
			}

			if (send_bytes > 0)
				total += send_bytes;
			else
				send_bytes = 0;
			
			if (send_bytes < size2send) 
			{
				size2send -= send_bytes;
			}
			else
			{
				if (size_unknown == 1)
					total -= sizeof(int);
				break;
			}
		}

		FD_ZERO(&ofds);
		FD_SET(hs->fd, &ofds);

		err = select(hs->fd + 1, 0, &ofds, 0, 0);

		if (err <= 0)
		{
			err = WSAGetLastError();
			return -1;
		}

	}while (1);

	if (hs->mode == HTTP_TUNNEL_VAR_MODE)
		hs->send_size = 0;

	return total;
}

int http_tunnel_recv(void *h_tunnel, void *buffer, int size)
{
	int total = 0;
	int received = 0;
	int size_tmp = 0;
	int limit = 0;
	int	err = 0;
	http_sock_t *hs;

	fd_set	rfds;
	struct timeval	to;

	if (!h_tunnel)
	  return -1;

	hs = (http_sock_t *) h_tunnel;

/*
	if (nonblock)
		FCNTL_NOBLOCK(hs->fd);*/

	if (hs->recv_size == 0)
	{
		for (total = 0; total < 4; total += received)
		{
			/* *** INIT TIMEOUT *** */
			to.tv_sec = 4;
			to.tv_usec = 0;
			/* ******************** */
			
			FD_ZERO(&rfds);
			FD_SET(hs->fd, &rfds);
			err = select(hs->fd + 1, &rfds, 0, 0, &to);

			if (err && FD_ISSET(hs->fd, &rfds))
			{
				received = recv(hs->fd, ((char *) &(hs->recv_size)) + total, 4 - total, 0);

				if (received <= 0)
				{
					err = WSAGetLastError();

					if (ERR_SOCK(err))
					{
						return HTTP_TUNNEL_ERR_DECONN;
					}
					else if (err != WSAEWOULDBLOCK)
					{
						return HTTP_TUNNEL_ERR_UNKNOW;
					}
					else
					{
						received = 0;
					}
				}
			}
			else
				return HTTP_TUNNEL_ERR_UNKNOW;
		}
	}

	if (hs->recv_size > MAX_SIZE)
		return HTTP_TUNNEL_ERR_DECONN;
	else if (hs->recv_size > size)
		return HTTP_TUNNEL_ERR_BUF2SMA;
	else if (hs->recv_size == 0)
		return 0;


	total = 0;
	limit = (size < hs->recv_size) ? size : hs->recv_size;
	
	for (size_tmp = limit; total < limit; size_tmp = limit - total)
	{
		/* *** INIT TIMEOUT *** */
		to.tv_sec = 4;
		to.tv_usec = 0;
		/* ******************** */
			
		FD_ZERO(&rfds);
		FD_SET(hs->fd, &rfds);
		err = select(hs->fd + 1, &rfds, 0, 0, &to);

		received = 0;

		if (err && FD_ISSET(hs->fd, &rfds))
		{
				received = recv(hs->fd, (char *) buffer + total, size_tmp, 0);

				if (received <= 0)
				{
					err = WSAGetLastError();

					if (ERR_SOCK(err))
					{
						return HTTP_TUNNEL_ERR_DECONN;
					}
					else if (err != WSAEWOULDBLOCK)
					{
						return HTTP_TUNNEL_ERR_UNKNOW;
					}
					else
					{
						received = 0;
					}
				}
		}

		if (received > 0)
			total += received;
	}

	if (hs->mode == HTTP_TUNNEL_VAR_MODE)
		hs->recv_size = 0;
/*
	if (nonblock)
		FCNTL_BLOCK(hs->fd);
*/

	return total;
}
#ifdef __cplusplus
}
#endif
