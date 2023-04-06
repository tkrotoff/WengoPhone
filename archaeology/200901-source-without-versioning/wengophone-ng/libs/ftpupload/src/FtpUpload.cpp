/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <ftpupload/FtpUpload.h>

#include <cutil/global.h>
#include <util/File.h>

#include <curl/curl.h>

#include <QtCore/QMetaType>

#include <sys/stat.h>

#ifdef OS_WINDOWS
	#define snprintf _snprintf
	#define stat _stat
#else
	#include <cstring>
#endif

static bool qtMetaTypeDeclared = false;
static void declareQtMetaType() {
	if (!qtMetaTypeDeclared) {
		qtMetaTypeDeclared = true;

		qRegisterMetaType<FtpUpload::Status>("FtpUpload::Status");
	}
}

int curlFTPProgress(void * instance, double dltotal, double dlnow, double ultotal, double ulnow) {
	if (instance) {
		FtpUpload * ftpUpload = (FtpUpload*) instance;
		ftpUpload->setProgress(ulnow, ultotal);
	}
	return 0;
}

int ftp_upload(const char * hostname, const char * login, const char * password,
	const char * filename, void * ftpUploadInstance) {

	if (!filename) {
		return -1;
	}

	struct stat buf;
	stat(filename, &buf);
	File file(filename);
	const char * short_filename = file.getFilename().c_str();
	const char * path = file.getPath().c_str();

	char url_buff[1024];
	memset(url_buff, 0, sizeof(url_buff));
	snprintf(url_buff, sizeof(url_buff), "ftp://%s", hostname);

	char tmp[1024];
	memset(tmp, 0, sizeof(tmp));

	if (path && *path) {
		snprintf(tmp, sizeof(tmp), "%s/%s", url_buff, path);
	}

	snprintf(url_buff, sizeof(url_buff), "%s/%s", tmp, short_filename);

	char auth_passwd[1024];
	memset(auth_passwd, 0, sizeof(auth_passwd));
	snprintf(auth_passwd, sizeof(auth_passwd), "%s:%s", login, password);

	curl_global_init(CURL_GLOBAL_ALL);

	FILE * lfile = fopen(filename, "rb");

	CURL * handle = curl_easy_init();

	curl_easy_setopt(handle, CURLOPT_URL, url_buff);
	curl_easy_setopt(handle, CURLOPT_USERPWD, auth_passwd);
	curl_easy_setopt(handle, CURLOPT_READDATA, lfile);
	curl_easy_setopt(handle, CURLOPT_INFILESIZE, buf.st_size);
	curl_easy_setopt(handle, CURLOPT_UPLOAD, 1);
	curl_easy_setopt(handle, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(handle, CURLOPT_FTP_USE_EPSV, 0);
	curl_easy_setopt(handle, CURLOPT_FTP_SKIP_PASV_IP, 1);

	curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(handle, CURLOPT_PROGRESSFUNCTION, curlFTPProgress);
	curl_easy_setopt(handle, CURLOPT_PROGRESSDATA, ftpUploadInstance);

	int res = curl_easy_perform(handle);

	curl_easy_cleanup(handle);

	fclose(lfile);

	curl_global_cleanup();

	return res;
}


FtpUpload::FtpUpload(const std::string & hostname, const std::string & login, const std::string & password,
	const std::string & filename)
	: _hostname(hostname),
	_login(login),
	_password(password),
	_filename(filename) {

	declareQtMetaType();
}

const std::string FtpUpload::getHostname() const {
	return _hostname;
}

const std::string FtpUpload::getLogin() const {
	return _login;
}

const std::string FtpUpload::getPassword() const {
	return _password;
}

void FtpUpload::setFilename(const std::string & filename) {
	_filename = filename;
}

const std::string FtpUpload::getFilename() const {
	return _filename;
}

void FtpUpload::setProgress(double ultotal, double ulnow) {
	progressionSignal(ulnow, ultotal);
}

void FtpUpload::run() {
	int res = ftp_upload(_hostname.c_str(), _login.c_str(), _password.c_str(),
		_filename.c_str(), this);

	if (res == 0) {
		statusSignal(Ok);
	} else {
		statusSignal(Error);
	}
}
