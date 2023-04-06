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

#include "CallHistoryWidget.h"

#include "MyWidgetFactory.h"

#include "callhistory/CallLog.h"
#include "callhistory/AddCallLogEvent.h"
#include "callhistory/RemoveCallLogEvent.h"
#include "AudioCallManager.h"
#include "SessionWindow.h"
#include "contact/ContactList.h"
#include "contact/Contact.h"
#include "util/EventFilter.h"
#include "sip/SipAddress.h"

#include <qwidget.h>
#include <qlistview.h>
#include <qpoint.h>

#include <iostream>
using namespace std;

const int CallHistoryWidget::LISTVIEW_COLUMN_ICON = 0;
const int CallHistoryWidget::LISTVIEW_COLUMN_DATETIME = 1;
const int CallHistoryWidget::LISTVIEW_COLUMN_FULL_NAME = 2;
const int CallHistoryWidget::LISTVIEW_COLUMN_DURATION = 3;
const int CallHistoryWidget::LISTVIEW_COLUMN_PHONE_NUMBER = 4;
const int CallHistoryWidget::LISTVIEW_COLUMN_SMS_MESSAGE = 5;

CallHistoryWidget::CallHistoryWidget(CallHistory & callHistory, QWidget * parent)
: QObject(parent) {

	_callHistoryWidget = MyWidgetFactory::create("CallHistoryWidgetForm.ui", this, parent);

	//_callHistoryListView (QListView)
	_callHistoryListView = (QListView *) _callHistoryWidget->child("callHistoryListView", "QListView");
	_callHistoryListView->setSorting(LISTVIEW_COLUMN_DATETIME, false);

	_callHistoryListView->setColumnWidthMode(LISTVIEW_COLUMN_PHONE_NUMBER, QListView::Manual);
	_callHistoryListView->hideColumn(LISTVIEW_COLUMN_PHONE_NUMBER);

	_callHistoryListView->setColumnWidthMode(LISTVIEW_COLUMN_SMS_MESSAGE, QListView::Manual);
	_callHistoryListView->hideColumn(LISTVIEW_COLUMN_SMS_MESSAGE);

	connect(_callHistoryListView, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)),
			this, SLOT(callContact(QListViewItem *, const QPoint &, int)));

	//Delete key press filter
	DeleteKeyEventFilter * deleteKeyEventFilter = new DeleteKeyEventFilter(this, SLOT(deletePressed()));
	_callHistoryListView->installEventFilter(deleteKeyEventFilter);

	//For the design pattern observer
	callHistory.addObserver(*this);
}

CallHistoryWidget::~CallHistoryWidget() {
	//TODO why it crashes if uncommented?
	//delete _callHistoryListView;
	//delete _callHistoryWidget;
}

void CallHistoryWidget::update(const Subject & /*subject*/, const Event & event) {
	QString typeEvent = event.getTypeEvent();

	if (typeEvent == "AddCallLogEvent") {
		const AddCallLogEvent & evt = (const AddCallLogEvent &)event;
		const CallLog & callLog = evt.getCallLog();
		addCallLog(callLog);
	}

	else if (typeEvent == "RemoveCallLogEvent") {
		const RemoveCallLogEvent & evt = (const RemoveCallLogEvent &)event;
		const CallLog & callLog = evt.getCallLog();

		QListViewItemIterator it(_callHistoryListView);
		while (it.current()) {
			QListViewItem * item = it.current();

			if (item->text(LISTVIEW_COLUMN_ICON) == callLog.getTypeStringTranslated() &&
				item->text(LISTVIEW_COLUMN_DATETIME) == callLog.getDateTime().toString("yyyy-MM-dd hh:mm") &&
				item->text(LISTVIEW_COLUMN_FULL_NAME) == callLog.getContactName() &&
				item->text(LISTVIEW_COLUMN_DURATION) == callLog.getDuration().toString("hh:mm:ss") &&
				item->text(LISTVIEW_COLUMN_PHONE_NUMBER) == callLog.getPhoneNumber()) {

				_callHistoryListView->takeItem(item);
				break;
			}
			++it;
		}
	}

	else {
		assert(NULL && "Unknown message event");
	}
}

void CallHistoryWidget::addCallLog(const CallLog & callLog) {
	QListViewItem * item = new QListViewItem(
					_callHistoryListView,
					callLog.getTypeStringTranslated(),
					callLog.getDateTime().toString("yyyy-MM-dd hh:mm"),
					callLog.getContactName(),
					callLog.getDuration().toString("hh:mm:ss"),
					callLog.getPhoneNumber(),
					callLog.getSms());
	item->setPixmap(LISTVIEW_COLUMN_ICON, callLog.getTypeIcon());

	_callHistoryListView->insertItem(item);
}

void CallHistoryWidget::removeCallLog(const QString & typeStringTranslated, const QString & dateTime,
	const QString & contactName, const QString & duration, const QString & phoneNumber) {

	CallHistory & callHistory = CallHistory::getInstance();

	for (unsigned int i = 0; i < callHistory.size(); i++) {
		CallLog & callLog = callHistory[i];

		if (typeStringTranslated == callLog.getTypeStringTranslated() &&
			dateTime == callLog.getDateTime().toString("yyyy-MM-dd hh:mm") &&
			contactName == callLog.getContactName() &&
			duration == callLog.getDuration().toString("hh:mm:ss") &&
			phoneNumber == callLog.getPhoneNumber()) {

			callHistory.removeCallLog(callLog);
			break;
		}
	}
}

void CallHistoryWidget::deletePressed() {
	QListViewItem * item = _callHistoryListView->selectedItem();
	if (item == NULL) {
		return;
	}

	removeCallLog(item->text(LISTVIEW_COLUMN_ICON),
			item->text(LISTVIEW_COLUMN_DATETIME),
			item->text(LISTVIEW_COLUMN_FULL_NAME),
			item->text(LISTVIEW_COLUMN_DURATION),
			item->text(LISTVIEW_COLUMN_PHONE_NUMBER));
}

void CallHistoryWidget::callContact(QListViewItem * item, const QPoint &, int column) {
	if (item == NULL || column == -1) {
		return;
	}

	QString phoneNumber = item->text(LISTVIEW_COLUMN_PHONE_NUMBER);
	QString callType = item->text(LISTVIEW_COLUMN_ICON);

	AudioCallManager & audioCallManager= AudioCallManager::getInstance();

	if (callType.contains("sms", false)) {
		//contains("sms, false): case insensitive
		QString msg = item->text(LISTVIEW_COLUMN_SMS_MESSAGE);
		audioCallManager.sendSms(phoneNumber, msg);
	} else {
		audioCallManager.createAudioCall(SipAddress::fromPhoneNumber(phoneNumber));
	}
}
