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

#ifndef OWFTPUPLOAD_H
#define OWFTPUPLOAD_H

#include <ftpupload/ftpuploaddll.h>

#include <QtCore/QThread>

#include <string>

/**
 * FtpUpload uploads a file to a FTP server.
 *
 * @author Julien Bossart
 * @author Mathieu Stute
 * @author Tanguy Krotoff
 */
class FTPUPLOAD_API FtpUpload : public QThread {
	Q_OBJECT
public:

	enum Status {
		Ok,
		Error,
		None,
	};

	/**
	 * Constructor.
	 *
	 * @param hostname hostname of the ftp server.
	 * @param login login used for connexion.
	 * @param password password used for connexion.
	 * @param filename file to upload.
	 */
	FtpUpload(const std::string & hostname, const std::string & login, const std::string & password,
		const std::string & filename);

	const std::string getHostname() const;

	const std::string getLogin() const;

	const std::string getPassword() const;

	void setFilename(const std::string & filename);
	const std::string getFilename() const;

	/**
	 * Internal use only.
	 */
	void setProgress(double ultotal, double ulnow);

Q_SIGNALS:

	/**
	 * FTP Status event.
	 *
	 * @param sender this class
	 * @param status FTP status code
	 */
	void statusSignal(Status status);

	/**
	 * FTP upload progression event.
	 *
	 * @param sender this class
	 * @param bytesDone specifies how many bytes have been transfered
	 * @param bytesTotal total size of the HTTP transfer, if 0 then the total number of bytes is not known
	 */
	void progressionSignal(double bytesDone, double bytesTotal);

private:

	/**
	`* From class QThread.
	 */
	virtual void run();

	std::string _hostname;

	std::string _login;

	std::string _password;

	std::string _filename;
};

#endif	//OWFTPUPLOAD_H
