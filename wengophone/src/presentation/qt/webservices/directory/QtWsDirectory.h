/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#ifndef OW_QTDIRECTORY_H
#define OW_QTDIRECTORY_H

#include "ui_WsDirectory.h"

#include <presentation/PWsDirectory.h>
#include <control/webservices/directory/CWsDirectory.h>

#include <qtutil/QObjectThreadSafe.h>

#include <QWidget>

class QtWsDirectoryContact;
class CWsDirectory;
class QtWengoPhone;

/**
 * Search buddies qt widget.
 *
 * @author Mathieu Stute
 */
class QtWsDirectory : public QObjectThreadSafe , public PWsDirectory {
	friend class QtWsDirectoryContact;
	Q_OBJECT
public:

	QtWsDirectory(CWsDirectory & cWsDirectory, QObject * parent = 0);

	~QtWsDirectory();

	QWidget * getWidget();
	
	void updatePresentation();

	void clear();

	void show();

private Q_SLOTS:

	void searchButtonClicked();

private:

	void callContact(const QString & sipAddress);

	void addContact(Profile * profile);

	void initThreadSafe();

	void updatePresentationThreadSafe();

	void contactFoundEventHandler(WsDirectory & sender, Profile * profile, bool online);

	void contactFoundEventHandlerThreadSafe(WsDirectory & sender, Profile * profile, bool online);

	CWsDirectory & _cWsDirectory;

	Ui::WsDirectoryForm ui;

	QWidget * _widget;

	QtWengoPhone * _qtWengoPhone;
};

#endif	//OW_QTDIRECTORY_H
