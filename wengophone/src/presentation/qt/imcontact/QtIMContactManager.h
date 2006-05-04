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

#ifndef QTIMCONTACTMANAGER_H
#define QTIMCONTACTMANAGER_H

#include <util/NonCopyable.h>

#include <QObject>

class Contact;

class QWidget;
class QTreeWidget;
class QTreeWidgetItem;
namespace Ui { class IMContactManager; }

/**
 * Window that handles IM accounts of a contact (IMContact).
 *
 * Very similar to QtIMAccountManager but for IM contacts.
 *
 * @see QtIMAccountManager
 * @see Contact
 * @see IMContact
 * @author Tanguy Krotoff
 */
class QtIMContactManager : public QObject, NonCopyable {
	Q_OBJECT
public:

	QtIMContactManager(Contact & contact, QWidget * parent);

	~QtIMContactManager();

	QWidget * getWidget() const {
		return _imContactManagerWidget;
	}

private Q_SLOTS:

	void addIMContact();

	void deleteIMContact();

private:

	void loadIMContacts();

	Ui::IMContactManager * _ui;

	QWidget * _imContactManagerWidget;

	Contact & _contact;
};

#endif //QTIMCONTACTMANAGER_H
