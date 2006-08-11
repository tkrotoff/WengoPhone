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

#include <model/webservices/url/WsUrl.h>
#include <control/CWengoPhone.h>
#include <presentation/qt/config/QtWengoConfigDialog.h>

#include <qtutil/MouseEventFilter.h>

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

	MousePressEventFilter * callForwardMouseFilter = new MousePressEventFilter(
			this, SLOT(callforwardModeClicked()), Qt::LeftButton);
	_ui->callForwardLabel->installEventFilter(callForwardMouseFilter);

	MousePressEventFilter * mouseFilter = new MousePressEventFilter(
		this, SLOT(buyOutClicked()), Qt::LeftButton);
	_ui->buyCreditsLabel->installEventFilter(mouseFilter);
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
		_ui->callForwardLabel->setText(_callForwardMode);

		QPalette palette = _ui->callForwardLabel->palette();
		palette.setColor(QPalette::WindowText, Qt::blue);
		_ui->callForwardLabel->setPalette(palette);

		QFont font = _ui->callForwardLabel->font();
		font.setUnderline(true);
		_ui->callForwardLabel->setFont(font);

		QCursor cursor = _ui->callForwardLabel->cursor();
		cursor.setShape(Qt::PointingHandCursor);
		_ui->callForwardLabel->setCursor(cursor);

		_ui->callForwardLabel->setToolTip(tr("Click here to change your call forward settings"));
	}

	if (!_pstnNumber.isEmpty()) {
		_ui->pstnNumberLabel->setText(_pstnNumber);
	}
}

void QtCreditWidget::setPstnNumber(const QString & number) {
	_pstnNumber = number;
	updatePresentation();
}

void QtCreditWidget::setCallForwardMode(const QString & callForwardMode) {
	_callForwardMode = callForwardMode;
	updatePresentation();
}

void QtCreditWidget::buyOutClicked() {
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
