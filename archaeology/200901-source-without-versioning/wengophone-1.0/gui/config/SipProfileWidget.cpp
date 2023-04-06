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

#include "SipProfileWidget.h"

#include "MyWidgetFactory.h"
#include "Authentication.h"
#include "connect/Connect.h"

#include <observer/Event.h>

#include <qwidgetfactory.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include <cassert>

SipProfileWidget::SipProfileWidget(QWidget * parent)
: QObject(parent), _auth(Authentication::getInstance()) {

	_auth.addObserver(*this);

	_sipProfileWidget = MyWidgetFactory::create("SipProfileWidgetForm.ui", this, parent);

	_doRegisterButton = (QPushButton *) _sipProfileWidget->child("registerButton");
	connect(_doRegisterButton, SIGNAL(clicked()),
		this, SLOT(doRegister()));

	_unregisterButton = (QPushButton *) _sipProfileWidget->child("unregisterButton");
	connect(_unregisterButton, SIGNAL(clicked()),
		this, SLOT(unregister()));

	_usernameEdit = (QLineEdit *) _sipProfileWidget->child("usernameEdit");
	_userIdEdit = (QLineEdit *) _sipProfileWidget->child("userIdEdit");
	_passwordEdit = (QLineEdit *) _sipProfileWidget->child("passwordEdit");
	_realmEdit = (QLineEdit *) _sipProfileWidget->child("realmEdit");
	_serverEdit = (QLineEdit *) _sipProfileWidget->child("serverEdit");
	_proxyEdit = (QLineEdit *) _sipProfileWidget->child("proxyEdit");
	_portEdit = (QLineEdit *) _sipProfileWidget->child("portEdit");
	_autoCheckBox = (QCheckBox *) _sipProfileWidget->child("autoCheckBox");
	_forceCheckBox = (QCheckBox *) _sipProfileWidget->child("forceCheckBox");

	update(_auth, Event());
}

/**
 * @brief execute modifications made on the widget
 * modify database
 */
void SipProfileWidget::doRegister() {  
	save();
	Connect::getInstance().disconnect();
	Connect::getInstance().connect();
	//_auth.registering(true);
}

/**
 * @brief save changes and init stack SIP without registering
 */
void SipProfileWidget::unregister() {
	save();
	_auth.setAutoRegister(false);
	Connect::getInstance().disconnect();
	//_auth.registering(false);
}

/**
 * @param event occuring
 * @brief apply changes on the window and make authentication
 * inherits from class Observer
 */
void SipProfileWidget::update(const Subject &, const Event &) {
	_usernameEdit->setText(_auth.getUsername());
	_userIdEdit->setText(_auth.getUserId());
	_passwordEdit->setText(_auth.getPassword());
	_realmEdit->setText(_auth.getRealm());  
	_serverEdit->setText(_auth.getServer());
	_proxyEdit->setText(_auth.getProxy());
	_portEdit->setText(_auth.getPort());
	_forceCheckBox->setChecked(_auth.isForceRegister());
	_autoCheckBox->setChecked(_auth.isAutoRegister());
}

/**
 * @brief save changes
 */
void SipProfileWidget::save() {
	_auth.changeSettings(_usernameEdit->text(),
			_userIdEdit->text(),
			_passwordEdit->text(),
			_realmEdit->text(),
			_serverEdit->text(),
			_proxyEdit->text(),
			_portEdit->text(),
			_forceCheckBox->isChecked(),
			_autoCheckBox->isChecked());
}
