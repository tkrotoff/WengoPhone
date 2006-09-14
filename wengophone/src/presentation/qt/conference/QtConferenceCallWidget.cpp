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

#include "QtConferenceCallWidget.h"

#include "ui_ConferenceCallWidget.h"

#include <model/phoneline/IPhoneLine.h>
#include <model/phonecall/ConferenceCall.h>

#include <util/SafeDelete.h>
#include <qtutil/WidgetBackgroundImage.h>
#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtConferenceCallWidget::QtConferenceCallWidget(QWidget * parent, IPhoneLine * phoneLine)
	: QDialog(parent), _phoneLine(phoneLine) {

	_ui = new Ui::ConferenceCallWidget();
	_ui->setupUi(this);

	WidgetBackgroundImage::setBackgroundImage(_ui->headerLabel, ":pics/headers/conference.png", true);

	SAFE_CONNECT(_ui->startButton, SIGNAL(clicked()), SLOT(startClicked()));
	SAFE_CONNECT(_ui->cancelButton, SIGNAL(clicked()), SLOT(reject()));
}

QtConferenceCallWidget::~QtConferenceCallWidget() {
	OWSAFE_DELETE(_ui);
}

void QtConferenceCallWidget::startClicked() {
	ConferenceCall * confCall = new ConferenceCall(*_phoneLine);
	confCall->addPhoneNumber(_ui->phoneNumber1LineEdit->text().toStdString());
	confCall->addPhoneNumber(_ui->phoneNumber2LineEdit->text().toStdString());
	accept();
}