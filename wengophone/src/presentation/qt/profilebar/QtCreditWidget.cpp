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

#include <control/CWengoPhone.h>
#include <presentation/qt/config/QtWengoConfigDialog.h>

#include <qtutil/MouseEventFilter.h>

QtCreditWidget::QtCreditWidget(CWengoPhone & cWengoPhone, QWidget * parent , Qt::WFlags f )
	: QObjectThreadSafe(NULL), _cWengoPhone(cWengoPhone) {

	_ui = NULL;
	_callForwardMode = tr("unauthorized");
	_pstnNumber = tr("no number");
	
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtCreditWidget::initThreadSafe, this));
	postEvent(event);
}

void QtCreditWidget::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtCreditWidget::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtCreditWidget::initThreadSafe() {
	_widget = new QWidget();
	_ui = new Ui::CreditWidget();
	_ui->setupUi(_widget);

	_ui->pstnNumberLabel->setText(tr("no number"));
	_ui->pstnNumberLabel->setToolTip(tr("Your Wengo number"));
	_ui->callForwardLabel->setText(tr("unauthorized"));
	_ui->callForwardLabel->setToolTip(tr("You need to buy wengo's credits in order to use the call forward"));
	_callForwardMouseFilter = new MousePressEventFilter(
			this, SLOT(callforwardModeClicked()), Qt::LeftButton);
	_ui->callForwardLabel->installEventFilter(_callForwardMouseFilter);

	MousePressEventFilter * mouseFilter = new MousePressEventFilter(
		this, SLOT(buyOutClicked()), Qt::LeftButton);
	_ui->buyCreditsLabel->installEventFilter(mouseFilter);
	_ui->buyCreditsLabel->setToolTip(tr("Click here to buy Wengo's credits"));
}

void QtCreditWidget::updatePresentationThreadSafe() {

	_ui->callForwardLabel->setText(_callForwardMode);
	_ui->pstnNumberLabel->setText(_pstnNumber);

	if( _callForwardMode != tr("unauthorized") ) {

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
}

QWidget * QtCreditWidget::getWidget() {
	return _widget;
}

void QtCreditWidget::setPstnNumber(const QString & number) {
	_pstnNumber = number;
	updatePresentation();
}

void QtCreditWidget::setCallForwardMode(const QString & mode) {
	_callForwardMode = mode;
	updatePresentation();
}

void QtCreditWidget::buyOutClicked() {
	_cWengoPhone.showWengoBuyWengos();
}

void QtCreditWidget::callforwardModeClicked() {
	QtWengoConfigDialog dialog(_cWengoPhone, _widget);
	dialog.showCallForwardPage();
	dialog.show();
}

void QtCreditWidget::slotUpdatedTranslation() {
  if (_ui)
    _ui->retranslateUi(_widget);
}
