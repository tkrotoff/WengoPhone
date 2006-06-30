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

#include "ui_SimpleAddIMContact.h"

#include <imwrapper/EnumIMProtocol.h>

#include <QObject>
#include <QDialog>

#include <set>

class CUserProfile;
class ContactProfile;
class QWidget;
class IMAccount;

/**
 * Simplified window for adding an IM contact to a given contact.
 *
 * @author Mr K.
 */
class QtSimpleAddIMContact : public QDialog
{
	Q_OBJECT
public:

	typedef std::vector< std::pair<std::string, std::string> > ContactGroupVector;

	QtSimpleAddIMContact(CUserProfile & cUserProfile,
		ContactProfile & contactProfile,
		QWidget * parent = NULL);

	virtual ~QtSimpleAddIMContact();

private Q_SLOTS:

	void saveContact();

	void advanced();

	void currentIndexChanged (const QString & text);

private:

	bool hasAccount(EnumIMProtocol::IMProtocol imProtocol) const;

	Ui::SimpleAddIMAccount _ui;

	CUserProfile & _cUserProfile;

	ContactProfile & _contactProfile;
};

#endif //OWQTSIMPLEADDIMCONTACT_H
