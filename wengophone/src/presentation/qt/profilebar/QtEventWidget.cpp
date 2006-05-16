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

#include "QtEventWidget.h"

#include <QtGui>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>

#include <model/profile/UserProfile.h>
#include <model/phoneline/IPhoneLine.h>

#include <presentation/PWengoPhone.h>

#include <qtutil/MouseEventFilter.h>

QtEventWidget::QtEventWidget(CWengoPhone & cWengoPhone, CUserProfile & cUserProfile,
	QWidget * parent, Qt::WFlags f)
	: QObjectThreadSafe(NULL), _cUserProfile(cUserProfile), _cWengoPhone(cWengoPhone) {

	_voiceMailCount = 0;
	_missedCallCount = 0;
	_widget = new QWidget();

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtEventWidget::initThreadSafe, this));
	postEvent(event);
}

void QtEventWidget::initThreadSafe() {

	_ui = new Ui::EventWidget();
	_ui->setupUi(_widget);

	MousePressEventFilter * mouseFilter = new MousePressEventFilter(
		this, SLOT(voiceMailClicked()), Qt::LeftButton);
	_ui->voiceMailIconLabel->installEventFilter(mouseFilter);
	_ui->voiceMailIconLabel->setToolTip(tr("Click here to call your voice mail"));

	MousePressEventFilter * mouseFilter2 = new MousePressEventFilter(
		this, SLOT(missedCallClicked()), Qt::LeftButton);
	_ui->missedCallIconLabel->installEventFilter(mouseFilter2);
	_ui->missedCallIconLabel->setToolTip(tr("Click here to see missed call(s)"));

	updatePresentation();
}

void QtEventWidget::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtEventWidget::updatePresentationThreadSafe, this));
	postEvent(event);
}

QWidget * QtEventWidget::getWidget() {
	return _widget;
}

void QtEventWidget::updatePresentationThreadSafe() {
	
	_ui->voiceMailCountLabel->setText(QString("%1").arg(_voiceMailCount));
	_ui->missedCallCountLabel->setText(QString("%1").arg(_missedCallCount));
}

void QtEventWidget::setVoiceMail(int count) {
	_voiceMailCount = count;
	updatePresentation();
}

void QtEventWidget::setMissedCall(int count) {
	_missedCallCount = count;
	updatePresentation();
}

void QtEventWidget::voiceMailClicked() {
	if( QMessageBox::question(
		_widget,
		tr("Call message box"),
		tr("Do you want to call your voice mail?"),
		tr("&No"), tr("&Yes"),
		QString(), 0, 1) ) {

		_cUserProfile.getUserProfile().getActivePhoneLine()->makeCall("123", false);
	}
}

void QtEventWidget::missedCallClicked() {
	//TODO: show the history tab
	_cWengoPhone.getPresentation()->showHistory();
}
