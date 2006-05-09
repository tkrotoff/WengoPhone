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

#include <model/profile/UserProfile.h>
#include <model/phoneline/IPhoneLine.h>
#include <control/CWengoPhone.h>

QtEventWidget::QtEventWidget(CWengoPhone & cWengoPhone, UserProfile & userProfile, QWidget * parent, Qt::WFlags f)
	: QWidget ( parent, f ), _userProfile(userProfile), _cWengoPhone(cWengoPhone) {

	QGridLayout * gridLayout = new QGridLayout(this);

	_missedCallLabel = new QtClickableLabel(this);
	_missedCallLabel->setText(tr("Missed calls"));

	_voiceMailLabel = new QtClickableLabel(this);
	_voiceMailLabel->setText(tr("New messages"));
	connect(_voiceMailLabel, SIGNAL(clicked()), SLOT(missedCallClicked()));

	gridLayout->addWidget(_missedCallLabel, 0, 0);
	gridLayout->addWidget(_voiceMailLabel, 0, 1);

}

void QtEventWidget::setVoiceMail(int count) {
	if( count ) {
		_voiceMailLabel->setText(QString("%1").arg(count) + " " + tr("new Voice Mail"));
	} else {
		_voiceMailLabel->setText(tr("No new Voice Mail"));
	}
}

void QtEventWidget::missedCallClicked() {
	if( QMessageBox::question(
		this,
		tr("Call message box"),
		tr("Do you want to call 123?"),
		tr("&No"), tr("&Yes"),
		QString(), 0, 1) ) {

		_userProfile.getActivePhoneLine()->makeCall("123", false);
	}
}
