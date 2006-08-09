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

#include "QtChatActionBarWidget.h"

#include <model/contactlist/ContactProfile.h>

#include <presentation/qt/contactlist/QtContactList.h>
#include <presentation/qt/contactlist/QtContactListManager.h>

#include <presentation/qt/chat/QtChatWidget.h>
#include <presentation/qt/QtWengoPhone.h>

#include <qtutil/QtWengoStyleLabel.h>

#include <util/String.h>

#include <QGridLayout>
#include <QString>

#if defined(OS_WINDOWS)
#include <windows.h>
#endif // OS_WINDOWS

QtChatActionBarWidget::QtChatActionBarWidget(QtWengoPhone * qtWengoPhone, QtChatWidget * chatWidget, QWidget * parent) :
QWidget(parent),
_qtWengoPhone(qtWengoPhone),
_chatWidget(chatWidget) {	

	setMaximumSize(QSize(10000, 80));
	setMinimumSize(QSize(16, 80));
	QGridLayout * layout = new QGridLayout(this);
	_callLabel = new QtWengoStyleLabel(this);
	_endLabel = new QtWengoStyleLabel(this);

	_callLabel->setPixmaps(
		QPixmap(":/pics/chat/chat_call_bar_button.png"),
		QPixmap(),
		QPixmap(),
		QPixmap(":/pics/chat/chat_call_bar_button_on.png"),
		QPixmap(),
		QPixmap()
		);
	_callLabel->setMaximumSize(QSize(46, 65));
	_callLabel->setMinimumSize(QSize(46, 46));

	_endLabel->setPixmaps(
		QPixmap(),
		QPixmap(":/pics/profilebar/bar_end.png"),
		QPixmap(":/pics/profilebar/bar_fill.png"),
		QPixmap(),
		QPixmap(":/pics/profilebar/bar_end.png"),
		QPixmap(":/pics/profilebar/bar_fill.png")
		);
	_endLabel->setMinimumSize(QSize(16, 65));

	layout->addWidget(_callLabel, 0, 0);
	layout->addWidget(_endLabel, 0, 1);
	layout->setMargin(0);
	layout->setSpacing(0);

	addLabel(QString("inviteLabel"),
		QPixmap(":/pics/chat/invite.png"),
		QPixmap(":/pics/chat/invite_on.png"),
		QSize(36, 65));

	connect(_labels["inviteLabel"], SIGNAL(clicked()), SLOT(inviteContact()));
	connect(_callLabel, SIGNAL(clicked()), SLOT(callContact()));
}

void QtChatActionBarWidget::addLabel(const QString & identifier, const QPixmap & normalPixmap, const QPixmap & pressedPixmap, const QSize & size, int position){
	QMutexLocker locker(& _mutex);
	if(!_labels.contains(identifier)){
		QGridLayout * glayout = dynamic_cast<QGridLayout *>(layout());
		QtWengoStyleLabel * label = new QtWengoStyleLabel(this);
		label->setPixmaps(normalPixmap, QPixmap(), QPixmap(), pressedPixmap, QPixmap(), QPixmap());
		label->setMinimumSize(size);
		label->setMaximumSize(size);
		_labels.insert(identifier, label);
		if(position < 0 || position > _labelsIndexes.size()){
			position = _labelsIndexes.size();
		}
		_labelsIndexes.insert(position, label);

		// first deplace the end
		glayout->addWidget(_endLabel, 0, glayout->columnCount());
		// then shift the buttons
		for(int i=glayout->columnCount()-3; i>=position+1; i--){
			glayout->addWidget(_labelsIndexes[i], 0, i+1);
		}
		// then add the button
		glayout->addWidget(label, 0, position+1);
	}
}

void QtChatActionBarWidget::removeLabel(const QString & identifier){
	if(_labels.contains(identifier)){
		QGridLayout * glayout = dynamic_cast<QGridLayout *>(layout());
		QtWengoStyleLabel * label = _labels.take(identifier);
		_labelsIndexes.removeAll(label);
		glayout->removeWidget(label);
		delete label;
	}
}

void QtChatActionBarWidget::addSeparator(int position){
	QMutexLocker locker(& _mutex);
	QGridLayout * glayout = dynamic_cast<QGridLayout *>(layout());
	QtWengoStyleLabel * label = new QtWengoStyleLabel(this);
	label->setPixmaps(QPixmap(":/pics/profilebar/bar_separator.png"), QPixmap(), QPixmap(), QPixmap(":/pics/profilebar/bar_separator.png"), QPixmap(), QPixmap());
	label->setMinimumSize(QSize(3,65));
	label->setMaximumSize(QSize(3,65));
	if(position < 0 || position > _labelsIndexes.size()){
		position = _labelsIndexes.size();
	}
	_labelsIndexes.insert(position, label);
	_separators.append(label);

	// first deplace the end
	glayout->addWidget(_endLabel, 0, glayout->columnCount());
	// then shift the buttons
	for(int i=glayout->columnCount()-3; i>=position+1; i--){
		glayout->addWidget(_labelsIndexes[i], 0, i+1);
	}
	// then add the button
	glayout->addWidget(label, 0, position);
}

void QtChatActionBarWidget::removeSeparator(int ord){
	QMutexLocker locker(& _mutex);
	if(ord < 1 || ord >= _separators.size()){
		return;
	}
	QGridLayout * glayout = dynamic_cast<QGridLayout *>(layout());
	QtWengoStyleLabel * label = _separators.takeAt(ord);
	_labelsIndexes.removeAll(label);
	_separators.removeAt(ord);
	glayout->removeWidget(label);
	delete label;
}

void QtChatActionBarWidget::inviteContact() {
	if (_chatWidget) {
		if (_chatWidget->canDoMultiChat()) {
			_chatWidget->inviteContact();
		}
	}
}

void QtChatActionBarWidget::callContact() {
	QString contactId;
	QtContactList * qtContactList;
	ContactProfile contactProfile;
	QtContactListManager * ul ;
	if (_chatWidget) {
		contactId = _chatWidget->getContactId();
		qtContactList = _qtWengoPhone->getContactList();
		contactProfile = qtContactList->getCContactList().getContactProfile(contactId.toStdString());
		ul = QtContactListManager::getInstance();
		ul->startCall(contactId);
		if (_qtWengoPhone->getWidget()->isMinimized()) {
			_qtWengoPhone->getWidget()->showNormal();
		}
#if defined(OS_WINDOWS)
		SetForegroundWindow(_qtWengoPhone->getWidget()->winId());
#endif
	}
}
