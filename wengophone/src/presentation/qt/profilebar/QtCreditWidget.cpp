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

#include "QtCreditWidget.h"

#include "ui_CreditWidget.h"

#include <presentation/qt/config/QtWengoConfigDialog.h>

#include <control/CWengoPhone.h>

#include <model/webservices/url/WsUrl.h>

#include <QtGui/QtGui>

QtCreditWidget::QtCreditWidget(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObjectThreadSafe(NULL),
	_cWengoPhone(cWengoPhone) {

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtCreditWidget::initThreadSafe, this));
	postEvent(event);
}

QtCreditWidget::~QtCreditWidget() {
	delete _ui;
}

void QtCreditWidget::initThreadSafe() {
	_widget = new QWidget();

	_ui = new Ui::CreditWidget();
	_ui->setupUi(_widget);

	//callForwardButton
	connect(_ui->callForwardButton, SIGNAL(clicked()), SLOT(callforwardModeClicked()));

	//buyCreditsButton
	connect(_ui->buyCreditsButton, SIGNAL(clicked()), SLOT(buyCreditsClicked()));
}

QWidget * QtCreditWidget::getWidget() const {
	return _widget;
}

void QtCreditWidget::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtCreditWidget::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtCreditWidget::updatePresentationThreadSafe() {
	if (!_callForwardMode.isEmpty()) {
		_ui->callForwardButton->setText(_callForwardMode);
	}

	if (!_landlineNumber.isEmpty()) {
		_ui->landlineNumberLabel->setText(_landlineNumber);
	}
}

void QtCreditWidget::setLandlineNumber(const QString & number) {
	_landlineNumber = number;
	updatePresentation();
}

void QtCreditWidget::setCallForwardMode(const QString & callForwardMode) {
	_callForwardMode = callForwardMode;
	updatePresentation();
}

void QtCreditWidget::buyCreditsClicked() {
	WsUrl::showWengoBuyWengos();
}

void QtCreditWidget::callforwardModeClicked() {
	QtWengoConfigDialog dialog(_cWengoPhone, _widget);
	dialog.showCallForwardPage();
	dialog.show();
}

void QtCreditWidget::slotUpdatedTranslation() {
	_ui->retranslateUi(_widget);
	updatePresentationThreadSafe();
}
