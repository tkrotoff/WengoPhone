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

#ifndef QTEDITCONTACTPROFILE_H
#define QTEDITCONTACTPROFILE_H

#include <imwrapper/EnumIMProtocol.h>

#include <QDialog>

class Contact;
class CWengoPhone;
class QWidget;
class QtIMContactDetails;
class QMenu;

namespace Ui {
	class ContactDetails;
}

// FIXME:Ui, cpp and h files should be moved from login dir to another dir (contact ?)
class QtEditContactProfile : public QDialog
{
	Q_OBJECT

public:
    enum EditMode { ModeEdit, ModeCreate };

	QtEditContactProfile (QtEditContactProfile::EditMode mode,Contact & contact, CWengoPhone & cWengoPhone, QWidget * parent = 0);

    virtual ~QtEditContactProfile();

private Q_SLOTS:

	void saveButtonClicked();

	void cancelButtonClicked();

	void addIMContact(EnumIMProtocol::IMProtocol protocol);

	void removeButtonClicked(QtIMContactDetails * qtIMContactDetails);

	void addIMContactButtonClicked();

	//FIXME: wa have a lot of action because of a strange bug on MacOS X.
	void actionClickedMSN() {
		addIMContact(EnumIMProtocol::IMProtocolMSN);
	}

	void actionClickedYahoo() {
		addIMContact(EnumIMProtocol::IMProtocolYahoo);
	}

	void actionClickedAIMICQ() {
		addIMContact(EnumIMProtocol::IMProtocolAIMICQ);
	}

	void actionClickedJabber() {
		addIMContact(EnumIMProtocol::IMProtocolJabber);
	}

private:

	void readFromConfig();

	void writeToConfig();

	void createContact();

	void addIMContactDetails(QtIMContactDetails * qtIMContactDetails);

	void removeIMContactDetails(QtIMContactDetails * qtIMContactDetails);

	/**
	 * Qt Designer file.
	 */
	Ui::ContactDetails * _ui;

    EditMode _mode;

	Contact & _contact;

	CWengoPhone & _cWengoPhone;

	QMenu * _addIMContactMenu;

};

#endif
