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

#include "QtHistory.h"

#include "QtHistoryWidget.h"
#include "QtHistoryItem.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/webservices/sms/QtSms.h>

#include <control/history/CHistory.h>
#include <control/CWengoPhone.h>

#include <model/history/History.h>
#include <model/phonecall/SipAddress.h>

#include <util/Logger.h>
#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtHistory::QtHistory(CHistory & cHistory)
	: QObject(NULL),
	_cHistory(cHistory) {

	_mutex = new QMutex(QMutex::Recursive);

	_stateFilter = HistoryMemento::Any;
	_historyWidget = new QtHistoryWidget(NULL);

	SAFE_CONNECT(_historyWidget, SIGNAL(replayItem(QtHistoryItem *)), SLOT(replayItem(QtHistoryItem *)));
	SAFE_CONNECT(_historyWidget, SIGNAL(removeItem(unsigned)), SLOT(removeItem(unsigned)));
	SAFE_CONNECT(_historyWidget, SIGNAL(missedCallsSeen()), SLOT(resetUnseenMissedCalls()));
	SAFE_CONNECT(_historyWidget, SIGNAL(showOnlyItemOfType(int)), SLOT(showOnlyItemOfTypeSlot(int)));

	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cHistory.getCWengoPhone().getPresentation();
	qtWengoPhone->setQtHistoryWidget(_historyWidget);
}

QtHistory::~QtHistory() {
	delete _mutex;
}

QWidget * QtHistory::getWidget() const {
	return _historyWidget->getWidget();
}

void QtHistory::historyLoadedEvent() {
	updatePresentation();
}

void QtHistory::showOnlyItemOfTypeSlot(int state) {
	_stateFilter = (HistoryMemento::State)state;
	updatePresentation();
}

void QtHistory::updatePresentation() {
	QMutexLocker locker(_mutex);

	_historyWidget->clearHistory();

	HistoryMementoCollection * collection = _cHistory.getHistory().getHistoryMementoCollection();
	for (HistoryMap::iterator it = collection->begin(); it != collection->end(); it++) {
		HistoryMemento * memento = (*it).second;

		if ((memento->getState() == _stateFilter) || (_stateFilter == HistoryMemento::Any)) {
			addHistoryMemento(
				memento->getState(),
				memento->getDate().toString(),
				memento->getTime().toString(),
				memento->getDuration(),
				memento->getPeer(),
				(*it).first
			);
		}
	}
}

void QtHistory::addHistoryMemento(HistoryMemento::State state, const std::string & date,
		const std::string & time, int duration, const std::string & name, unsigned id) {

	QDate qdate = QDate::fromString(QString::fromStdString(date), "yyyy-MM-dd");
	QTime qtime = QTime::fromString(QString::fromStdString(time));
	QTime qduration;
	if (duration == -1) {
		qduration = qduration.addSecs(0);
	} else {
		qduration = qduration.addSecs(duration);
	}

	SipAddress sipAddress(name);
	QString formattedName = QString::fromStdString(sipAddress.getUserName());

	switch(state) {
	case HistoryMemento::IncomingCall:
		_historyWidget->addIncomingCallItem(tr("Incoming call"),
				qdate, qtime, qduration, formattedName, id);
		break;
	case HistoryMemento::OutgoingCall:
		_historyWidget->addOutGoingCallItem(tr("Outgoing call"),
				qdate, qtime, qduration, formattedName, id);
		break;
	case HistoryMemento::MissedCall:
		_historyWidget->addMissedCallItem(tr("Missed call"),
				qdate, qtime, qduration, formattedName, id);
		break;
	case HistoryMemento::RejectedCall:
		_historyWidget->addRejectedCallItem(tr("Rejected call"),
				qdate, qtime, qduration, formattedName, id);
		break;
	case HistoryMemento::OutgoingSmsOk:
		_historyWidget->addSMSItem(tr("Outgoing SMS"),
				qdate, qtime, qduration, formattedName, id);
		break;
	case HistoryMemento::OutgoingSmsNok:
		break;
	case HistoryMemento::ChatSession:
		break;
	case HistoryMemento::None:
		break;
	case HistoryMemento::Any:
		break;
	default:
		LOG_FATAL("unknown HistoryMemento::State=" + String::fromNumber(state));
	}
}

void QtHistory::removeHistoryMemento(unsigned id) {
	_cHistory.removeHistoryMemento(id);
}

void QtHistory::mementoAddedEvent(unsigned id) {
	updatePresentation();
}

void QtHistory::mementoUpdatedEvent(unsigned id) {
	updatePresentation();
}

void QtHistory::mementoRemovedEvent(unsigned id) {
	updatePresentation();
}

void QtHistory::unseenMissedCallsChangedEvent(int count) {
}

void QtHistory::resetUnseenMissedCalls() {
	_cHistory.resetUnseenMissedCalls();
}

void QtHistory::clearAllEntries() {
	_cHistory.clear(HistoryMemento::Any);
}

void QtHistory::clearSmsEntries() {
	_cHistory.clear(HistoryMemento::OutgoingSmsOk);
}

void QtHistory::clearChatEntries() {
	_cHistory.clear(HistoryMemento::ChatSession);
}

void QtHistory::clearIncomingCallEntries() {
	_cHistory.clear(HistoryMemento::IncomingCall);
}

void QtHistory::clearOutgoingCallEntries() {
	_cHistory.clear(HistoryMemento::OutgoingCall);
}

void QtHistory::clearMissedCallEntries() {
	_cHistory.clear(HistoryMemento::MissedCall);
}

void QtHistory::clearRejectedCallEntries() {
	_cHistory.clear(HistoryMemento::RejectedCall);
}

void QtHistory::replayItem(QtHistoryItem * item) {
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cHistory.getCWengoPhone().getPresentation();

	QMessageBox mb(tr("WengoPhone - Call History"),
		tr("Do you want to call %1?").arg(item->text(QtHistoryItem::COLUMN_PEERS)),
		QMessageBox::Question,
		QMessageBox::Yes | QMessageBox::Default,
		QMessageBox::No | QMessageBox::Escape,
		QMessageBox::NoButton, _historyWidget->getWidget());

	switch (item->getItemType()) {
	case QtHistoryItem::Sms: {
		//Retrieve info & configure the Sms widget
		QString text = QString::fromUtf8(
			_cHistory.getMementoData(item->getId()).c_str(),
			_cHistory.getMementoData(item->getId()).size());
		QString phoneNumber = QString::fromStdString(_cHistory.getMementoPeer(item->getId()));

		//Test existance of Sms (available only if a WengoAccount has been created)
		QtSms * sms = qtWengoPhone->getQtSms();
		if (sms) {
			sms->setText(text);
			sms->setPhoneNumber(phoneNumber);
			sms->getWidget()->show();
		}
		break;
	}

	case QtHistoryItem::OutGoingCall:
	case QtHistoryItem::IncomingCall:
	case QtHistoryItem::MissedCall:
	case QtHistoryItem::RejectedCall:
		if (mb.exec() == QMessageBox::Yes) {
			_cHistory.replay(item->getId());
		}
		break;

	case QtHistoryItem::Chat:
		break;

	default:
		break;
	}
}

void QtHistory::removeItem(unsigned id) {
	LOG_DEBUG("item removed=" + String::fromNumber(id));
	_cHistory.removeHistoryMemento(id);
}
