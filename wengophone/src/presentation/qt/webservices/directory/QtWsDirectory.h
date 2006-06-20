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

#ifndef OWQTWSDIRECTORY_H
#define OWQTWSDIRECTORY_H

#include <presentation/PWsDirectory.h>

#include <control/webservices/directory/CWsDirectory.h>

#include <util/Trackable.h>

#include <qtutil/QObjectThreadSafe.h>

#include <QWidget>

class ContactProfile;
class CWsDirectory;
class QtWsDirectoryContact;
class QtWengoPhone;

class QDialog;
class QWidget;
namespace Ui { class WsDirectory; }

/**
 * Search buddies qt widget.
 *
 * @author Mathieu Stute
 */
class QtWsDirectory : public QObjectThreadSafe, public PWsDirectory, public Trackable {
	Q_OBJECT
	friend class QtWsDirectoryContact;
public:

	QtWsDirectory(CWsDirectory & cWsDirectory);

	virtual ~QtWsDirectory();

	QWidget * getWidget() const;

	void updatePresentation();

	void clear();

	void show();

private Q_SLOTS:

	void searchButtonClicked();

private:

	void callContact(const QString & sipAddress);

	void addContact(ContactProfile * contact);

	void initThreadSafe();

	void updatePresentationThreadSafe();

	void contactFoundEventHandler(WsDirectory & sender, ContactProfile * contact, bool online);

	void contactFoundEventHandlerThreadSafe(WsDirectory & sender, ContactProfile * contact, bool online);

	CWsDirectory & _cWsDirectory;

	Ui::WsDirectory * _ui;

	QDialog * _directoryWindow;

	QtWengoPhone * _qtWengoPhone;
};

#endif	//OWQTWSDIRECTORY_H
