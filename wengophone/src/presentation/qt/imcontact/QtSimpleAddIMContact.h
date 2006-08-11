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

#ifndef OWQTSIMPLEADDIMCONTACT_H
#define OWQTSIMPLEADDIMCONTACT_H

#include <imwrapper/EnumIMProtocol.h>

#include <QtCore/QObject>

#include <vector>

class CUserProfile;
class ContactProfile;

class QDialog;
class QWidget;
namespace Ui { class SimpleAddIMAccount; }

/**
 * Simplified window for adding an IM contact to a given contact.
 */
class QtSimpleAddIMContact : public QObject {
	Q_OBJECT
public:

	QtSimpleAddIMContact(CUserProfile & cUserProfile, ContactProfile & contactProfile, QWidget * parent);

	~QtSimpleAddIMContact();

	int show();

private Q_SLOTS:

	void saveContact();

	void advanced();

	void currentIndexChanged(const QString & text);

private:

	bool hasAccount(EnumIMProtocol::IMProtocol imProtocol) const;

	typedef std::vector< std::pair<std::string, std::string> > ContactGroupVector;

	QDialog * _addIMAccountDialog;

	Ui::SimpleAddIMAccount * _ui;

	CUserProfile & _cUserProfile;

	ContactProfile & _contactProfile;
};

#endif	//OWQTSIMPLEADDIMCONTACT_H
