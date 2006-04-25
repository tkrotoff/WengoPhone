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

#include <sys/stat.h> 
#include "curl/curl.h"
#include "cutil/global.h"

#define WENGO_FTP		"ftp.wengo.fr"
#define WENGO_USER		"wengo"
#define WENGO_PASSWD	"wengo"

#ifdef OS_WIN32
#define snprintf	_snprintf
#define stat		_stat
#endif

char *get_filename(const char *full)
{
	char *filename;
	char *begin = (char *) full;

	if (!full)
		return NULL;

	filename = begin + strlen(begin) - 1;

	while (filename != begin && *filename != '\\' && *filename != '/')
		filename--;

	if (filename == begin)
		return begin;
	else
		return ++filename;
}

int ftp_upload(const char * host, const char * path, const char *fullfilename, 
			   int (*progress_cb)(void *, double, double, double, double))
{
	CURL *handle;
	char url_buff[1024];
	char auth_passwd[1024];
	const char *filename;
	FILE *lfile;
	struct _stat buf;
	int res;

	if (!fullfilename)
		return -1;

	stat(fullfilename, &buf);
	filename = get_filename(fullfilename);

	if (host && *host)
		snprintf(url_buff, sizeof(url_buff), "ftp://%s/%s/%s", host, path, filename);
	else
		snprintf(url_buff, sizeof(url_buff), "ftp://%s/%s", WENGO_FTP, filename);

	snprintf(auth_passwd, sizeof(auth_passwd), "%s:%s", WENGO_USER, WENGO_PASSWD);

	curl_global_init(CURL_GLOBAL_ALL);

	lfile = fopen(fullfilename, "rb");
 
	handle = curl_easy_init();

	curl_easy_setopt(handle, CURLOPT_URL, url_buff);
	curl_easy_setopt(handle, CURLOPT_USERPWD, auth_passwd);
	curl_easy_setopt(handle, CURLOPT_READDATA, lfile);
	curl_easy_setopt(handle, CURLOPT_INFILESIZE, buf.st_size); 
	curl_easy_setopt(handle, CURLOPT_UPLOAD, TRUE);

	if (progress_cb != NULL)
	{
		curl_easy_setopt(handle, CURLOPT_NOPROGRESS, FALSE);
		curl_easy_setopt(handle, CURLOPT_PROGRESSFUNCTION, progress_cb);
	}

	res = curl_easy_perform(handle);

	curl_easy_cleanup(handle); 

	fclose(lfile);

	curl_global_cleanup();
	
	return res;
}