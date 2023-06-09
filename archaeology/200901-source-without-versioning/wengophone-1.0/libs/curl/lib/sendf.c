/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2005, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * $Id: sendf.c,v 1.98 2005/04/07 15:27:14 bagder Exp $
 ***************************************************************************/

#include "setup.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h> /* required for send() & recv() prototypes */
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <curl/curl.h>
#include "urldata.h"
#include "sendf.h"
#include "connect.h" /* for the Curl_ourerrno() proto */
#include "sslgen.h"

#define _MPRINTF_REPLACE /* use the internal *printf() functions */
#include <curl/mprintf.h>

#ifdef HAVE_KRB4
#include "krb4.h"
#else
#define Curl_sec_write(a,b,c,d) -1
#define Curl_sec_read(a,b,c,d) -1
#endif

#include <string.h>
#include "memory.h"
#include "strerror.h"
/* The last #include file should be: */
#include "memdebug.h"


/* ///////////// <JULIEN> ///////////// */
int get_httpresponse(int fd, char *buff, int buffsize);
int get_httpbody(int fd, char *buff, int buffsize, int bodysize);
int get_httpbodysize(char *buff);
/* ************************************ */

/* returns last node in linked list */
static struct curl_slist *slist_get_last(struct curl_slist *list)
{
  struct curl_slist     *item;

  /* if caller passed us a NULL, return now */
  if (!list)
    return NULL;

  /* loop through to find the last item */
  item = list;
  while (item->next) {
    item = item->next;
  }
  return item;
}

/*
 * curl_slist_append() appends a string to the linked list. It always retunrs
 * the address of the first record, so that you can sure this function as an
 * initialization function as well as an append function. If you find this
 * bothersome, then simply create a separate _init function and call it
 * appropriately from within the proram.
 */
struct curl_slist *curl_slist_append(struct curl_slist *list,
                                     const char *data)
{
  struct curl_slist     *last;
  struct curl_slist     *new_item;

  new_item = (struct curl_slist *) malloc(sizeof(struct curl_slist));
  if (new_item) {
    char *dup = strdup(data);
    if(dup) {
      new_item->next = NULL;
      new_item->data = dup;
    }
    else {
      free(new_item);
      return NULL;
    }
  }
  else
    return NULL;

  if (list) {
    last = slist_get_last(list);
    last->next = new_item;
    return list;
  }

  /* if this is the first item, then new_item *is* the list */
  return new_item;
}

/* be nice and clean up resources */
void curl_slist_free_all(struct curl_slist *list)
{
  struct curl_slist     *next;
  struct curl_slist     *item;

  if (!list)
    return;

  item = list;
  do {
    next = item->next;

    if (item->data) {
      free(item->data);
    }
    free(item);
    item = next;
  } while (next);
}

/* Curl_infof() is for info message along the way */

void Curl_infof(struct SessionHandle *data, const char *fmt, ...)
{
  if(data && data->set.verbose) {
    va_list ap;
    char print_buffer[1024 + 1];
    va_start(ap, fmt);
    vsnprintf(print_buffer, 1024, fmt, ap);
    va_end(ap);
    Curl_debug(data, CURLINFO_TEXT, print_buffer, strlen(print_buffer), NULL);
  }
}

/* Curl_failf() is for messages stating why we failed.
 * The message SHALL NOT include any LF or CR.
 */

void Curl_failf(struct SessionHandle *data, const char *fmt, ...)
{
  va_list ap;
  size_t len;
  va_start(ap, fmt);

  vsnprintf(data->state.buffer, BUFSIZE, fmt, ap);

  if(data->set.errorbuffer && !data->state.errorbuf) {
    snprintf(data->set.errorbuffer, CURL_ERROR_SIZE, "%s", data->state.buffer);
    data->state.errorbuf = TRUE; /* wrote error string */
  }
  if(data->set.verbose) {
    len = strlen(data->state.buffer);
    if(len < BUFSIZE - 1) {
      data->state.buffer[len] = '\n';
      data->state.buffer[++len] = '\0';
    }
    Curl_debug(data, CURLINFO_TEXT, data->state.buffer, len, NULL);
  }

  va_end(ap);
}

/* Curl_sendf() sends formated data to the server */
CURLcode Curl_sendf(curl_socket_t sockfd, struct connectdata *conn,
                    const char *fmt, ...)
{
  struct SessionHandle *data = conn->data;
  ssize_t bytes_written;
  size_t write_len;
  CURLcode res;
  char *s;
  char *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = vaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  if(!s)
    return CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written=0;
  write_len = strlen(s);
  sptr = s;

  while (1) {
    /* Write the buffer to the socket */
    res = Curl_write(conn, sockfd, sptr, write_len, &bytes_written);

    if(CURLE_OK != res)
      break;

    if(data->set.verbose)
      Curl_debug(data, CURLINFO_DATA_OUT, sptr, bytes_written, conn);

    if((size_t)bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    else
      break;
  }

  free(s); /* free the output string */

  return res;
}

/*
 * Curl_write() is an internal write function that sends plain (binary) data
 * to the server. Works with plain sockets, SSL or kerberos.
 */
CURLcode Curl_write(struct connectdata *conn,
                    curl_socket_t sockfd,
                    void *mem,
                    size_t len,
                    ssize_t *written)
{
  ssize_t bytes_written;
  CURLcode retcode;
  int num = (sockfd == conn->sock[SECONDARYSOCKET]);

  if (conn->ssl[num].use)
    /* only TRUE if SSL enabled */
    bytes_written = Curl_ssl_send(conn, num, mem, len);
  else {
    if(conn->sec_complete)
      /* only TRUE if krb4 enabled */
      bytes_written = Curl_sec_write(conn, sockfd, mem, len);
    else
      bytes_written = (ssize_t)swrite(sockfd, mem, len);

    if(-1 == bytes_written) {
      int err = Curl_ourerrno();

      if(
#ifdef WSAEWOULDBLOCK
        /* This is how Windows does it */
        (WSAEWOULDBLOCK == err)
#else
        /* As pointed out by Christophe Demory on March 11 2003, errno
           may be EWOULDBLOCK or on some systems EAGAIN when it returned
           due to its inability to send off data without blocking. We
           therefor treat both error codes the same here */
        (EWOULDBLOCK == err) || (EAGAIN == err) || (EINTR == err)
#endif
        )
        /* this is just a case of EWOULDBLOCK */
        bytes_written=0;
      else
        failf(conn->data, "Send failure: %s",
              Curl_strerror(conn, err));
    }
  }
  *written = bytes_written;
  retcode = (-1 != bytes_written)?CURLE_OK:CURLE_SEND_ERROR;

  return retcode;
}

/* client_write() sends data to the write callback(s)

   The bit pattern defines to what "streams" to write to. Body and/or header.
   The defines are in sendf.h of course.
 */
CURLcode Curl_client_write(struct SessionHandle *data,
                           int type,
                           char *ptr,
                           size_t len)
{
  size_t wrote;

  if(0 == len)
    len = strlen(ptr);

  if(type & CLIENTWRITE_BODY) {
    wrote = data->set.fwrite(ptr, 1, len, data->set.out);
    if(wrote != len) {
      failf (data, "Failed writing body");
      return CURLE_WRITE_ERROR;
    }
  }
  if((type & CLIENTWRITE_HEADER) &&
     (data->set.fwrite_header || data->set.writeheader) ) {
    /*
     * Write headers to the same callback or to the especially setup
     * header callback function (added after version 7.7.1).
     */
    curl_write_callback writeit=
      data->set.fwrite_header?data->set.fwrite_header:data->set.fwrite;

    wrote = writeit(ptr, 1, len, data->set.writeheader);
    if(wrote != len) {
      failf (data, "Failed writing header");
      return CURLE_WRITE_ERROR;
    }
  }

  return CURLE_OK;
}

/*
 * Internal read-from-socket function. This is meant to deal with plain
 * sockets, SSL sockets and kerberos sockets.
 *
 * If the read would block (EWOULDBLOCK) we return -1. Otherwise we return
 * a regular CURLcode value.
 */
int Curl_read(struct connectdata *conn, /* connection data */
              curl_socket_t sockfd,     /* read from this socket */
              char *buf,                /* store read data here */
              size_t buffersize,        /* max amount to read */
              ssize_t *n)               /* amount bytes read */
{
  ssize_t nread;

  /* Set 'num' to 0 or 1, depending on which socket that has been sent here.
     If it is the second socket, we set num to 1. Otherwise to 0. This lets
     us use the correct ssl handle. */
  int num = (sockfd == conn->sock[SECONDARYSOCKET]);

  *n=0; /* reset amount to zero */

  if(conn->ssl[num].use) {
    nread = Curl_ssl_recv(conn, num, buf, buffersize);

    if(nread == -1)
      return -1; /* -1 from Curl_ssl_recv() means EWOULDBLOCK */
  }
  else {
    *n=0; /* reset amount to zero */
    if(conn->sec_complete)
      nread = Curl_sec_read(conn, sockfd, buf, buffersize);
    else
      /* /////////// <JULIEN> /////////// */
      //nread = sread(sockfd, buf, buffersize); // libcurl
      nread = get_httpresponse(sockfd, buf, buffersize);
      /* ********************************* */
    
    if(-1 == nread) {
      int err = Curl_ourerrno();
#ifdef WIN32
      if(WSAEWOULDBLOCK == err)
#else
      if((EWOULDBLOCK == err) || (EAGAIN == err) || (EINTR == err))
#endif
        return -1;
    }
  }
  *n = nread;
  return CURLE_OK;
}

/* return 0 on success */
static int showit(struct SessionHandle *data, curl_infotype type,
                  char *ptr, size_t size)
{
  static const char * const s_infotype[CURLINFO_END] = {
    "* ", "< ", "> ", "{ ", "} ", "{ ", "} " };

  if(data->set.fdebug)
    return (*data->set.fdebug)(data, type, ptr, size,
                               data->set.debugdata);

  switch(type) {
  case CURLINFO_TEXT:
  case CURLINFO_HEADER_OUT:
  case CURLINFO_HEADER_IN:
    fwrite(s_infotype[type], 2, 1, data->set.err);
    fwrite(ptr, size, 1, data->set.err);
    break;
  default: /* nada */
    break;
  }
  return 0;
}

int Curl_debug(struct SessionHandle *data, curl_infotype type,
               char *ptr, size_t size,
               struct connectdata *conn)
{
  int rc;
  if(data->set.printhost && conn && conn->host.dispname) {
    char buffer[160];
    const char *t=NULL;
    const char *w="Data";
    switch (type) {
    case CURLINFO_HEADER_IN:
      w = "Header";
    case CURLINFO_DATA_IN:
      t = "from";
      break;
    case CURLINFO_HEADER_OUT:
      w = "Header";
    case CURLINFO_DATA_OUT:
      t = "to";
      break;
    default:
      break;
    }

    if(t) {
      snprintf(buffer, sizeof(buffer), "[%s %s %s%s]", w, t,
               conn->xfertype==NORMAL?"":
               (conn->xfertype==SOURCE3RD?"source ":"target "),
               conn->host.dispname);
      rc = showit(data, CURLINFO_TEXT, buffer, strlen(buffer));
      if(rc)
        return rc;
    }
  }
  rc = showit(data, type, ptr, size);
  return rc;
}

/* //////////////////// <JULIEN> //////////////////// */
int	get_httpresponse(int fd, char *buff, int buffsize)
{
	struct timeval	timeout;
	fd_set	rfds;
	int ret;
	int nbytes = 0;

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	while (1)
	{
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);

		ret = select(fd + 1, &rfds, 0, 0, &timeout);
		
		if (ret <= 0)
			return -1;

		if (FD_ISSET(fd, &rfds))
		{
			ret = recv(fd, buff + nbytes, 1, 0);
			
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

	if ((ret = get_httpbody(fd, buff + nbytes, buffsize - nbytes, get_httpbodysize(buff))) > 0)
		nbytes += ret;

	return nbytes;
}

int	get_httpbody(int fd, char *buff, int buffsize, int bodysize)
{
	struct timeval	timeout;
	fd_set	rfds;
	int ret;
	int nbytes = 0;
	int tmpsize = bodysize;

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	while (1)
	{
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);

		ret = select(fd + 1, &rfds, 0, 0, &timeout);
		
		if (ret < 0)
			return -1;
		else if (ret == 0)
			break;

		if (FD_ISSET(fd, &rfds))
		{
			ret = recv(fd, buff + nbytes, tmpsize, 0);
			if (ret > 0)
			{
				nbytes += ret;
				tmpsize -= ret;
			}

			if (nbytes == bodysize || nbytes == buffsize)
				break;
		}
	}

	return nbytes;
}

#if defined(WIN32) || defined(__APPLE__) || defined(__FreeBSD__)

char	*strndup(char *str, int nbytes)
{
	char	*newstr;
	int		i;

	if (!str || nbytes < 1)
		return 0;

	newstr = (char *) malloc(nbytes + 1);

	for (i = 0; *str && i < nbytes; i++)
		newstr[i] = *str++;

	newstr[i] = 0;

	return newstr;
}
#endif

#ifndef _MSC_VER
#define strnicmp strncasecmp
#endif

int get_httpbodysize(char *buff)
{
	int i = 0;
	char *tmp = 0;

	while (*buff)
	{
		if (strnicmp("content-length:", buff, 15) == 0)
		{
			while (*buff && (*buff < '0' || *buff > '9'))
				buff++;

			for (i = 0; buff[i] && buff[i] >= '0' && buff[i] <= '9'; i++)
				;

			tmp = strndup(buff, i);
			return atoi(tmp);
		}
		buff++;
	}

	return 0;
}
/* ************************************************** */
