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


#include "FileSource.h"
using namespace database;
#include "exception/FileNotFoundException.h"

#include <qstring.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>

FileSource::FileSource(const QString & filename) throw (FileNotFoundException) {
	static const QString slash("/");
	static const QString antislash("\\");

	QString tmpFilename = QDir::convertSeparators(filename);
	//For windows 98 otherwise it does not work
	//Example: C:/\wengo\\...

	tmpFilename.replace(slash + antislash, antislash);
	tmpFilename.replace(slash + slash, slash);
	tmpFilename.replace(antislash + antislash, antislash);

	QString errorMessage(QString("Couldn't open file: ") + tmpFilename);
	_file = new QFile(tmpFilename);
	if (!_file->open(IO_ReadOnly)) {
		throw FileNotFoundException(errorMessage);
	}
}

FileSource::~FileSource() {
	delete _file;
}

QString FileSource::read() const {
	QString data;

	QTextStream stream(_file);
	stream.setEncoding(QTextStream::UnicodeUTF8);
	data = stream.read();
	_file->close();

	return data;
}
