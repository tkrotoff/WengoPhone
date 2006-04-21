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

#include "QtIMContactDetails.h"

#include <imwrapper/IMContact.h>

#include "ui_IMContactDetails.h"

QtIMContactDetails::QtIMContactDetails(IMContact & imContact, QWidget * parent)
: QWidget(parent), _imContact(imContact) {

	_ui = new Ui::IMContactDetails();
	_ui->setupUi(this);

	init();

	connect(_ui->removeButton, SIGNAL(clicked()), SLOT(removeButtonClicked()));
}

void QtIMContactDetails::init() {
	_ui->imContactIdLabel->setText(QString::fromStdString(_imContact.getContactId()));

	QPixmap pixmap;

	if (_imContact.getProtocol() == EnumIMProtocol::IMProtocolMSN) {
		pixmap = QPixmap(":pics/protocol_msn.png");
	} else if (_imContact.getProtocol() == EnumIMProtocol::IMProtocolYahoo) {
		pixmap = QPixmap(":pics/protocol_yahoo.png");
	} else if (_imContact.getProtocol() == EnumIMProtocol::IMProtocolAIMICQ) {
		pixmap = QPixmap(":pics/protocol_aim.png");
	} else if (_imContact.getProtocol() == EnumIMProtocol::IMProtocolJabber) {
		pixmap = QPixmap(":pics/protocol_jabber.png");
	} else if (_imContact.getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE) {
		pixmap = QPixmap(":pics/protocol_wengo.png");		
	}

	_ui->protocolPixmap->setPixmap(pixmap);
}

void QtIMContactDetails::removeButtonClicked() {
	removeButtonClicked(this);
}
