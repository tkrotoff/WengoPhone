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

#ifndef DATABASE_FILETARGET_H
#define DATABASE_FILETARGET_H

#include "Target.h"

#include <exception/FileNotFoundException.h>

class QString;
class QFile;

namespace database {

/**
 * Writes a Contact in the VCard format.
 *
 * @author Tanguy Krotoff
 */
class FileTarget : public Target {
public:

	FileTarget(const QString & filename) throw (FileNotFoundException);

	virtual ~FileTarget();

	virtual void write(const QString & data);

private:

	FileTarget();
	FileTarget(const FileTarget &);
	FileTarget & operator=(const FileTarget &);

	QFile * _file;
};

}

#endif	//DATABASE_FILETARGET_H
