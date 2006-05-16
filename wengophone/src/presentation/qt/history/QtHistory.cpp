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
#include <control/CWengoPhone.h>
#include <model/history/History.h>
#include <model/phonecall/SipAddress.h>

#include <util/Logger.h>

#include <QDate>
#include <QTime>

QtHistory::QtHistory( CHistory & cHistory )
	: QObjectThreadSafe(NULL), _cHistory(cHistory) {

	_cHistory.historyLoadedEvent += boost::bind(&QtHistory::historyLoadedEventHandler, this, _1);
	_cHistory.mementoAddedEvent += boost::bind(&QtHistory::mementoAddedEventHandler, this, _1, _2);
	_cHistory.mementoUpdatedEvent += boost::bind(&QtHistory::mementoUpdatedEventHandler, this, _1, _2);
	_cHistory.mementoRemovedEvent += boost::bind(&QtHistory::mementoRemovedEventHandler, this, _1, _2);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtHistory::initThreadSafe, this));
	postEvent(event);
}

QWidget * QtHistory::getWidget() {
	return _historyWidget;
}

void QtHistory::initThreadSafe() {
	_historyWidget = new QtHistoryWidget();

	connect (_historyWidget, SIGNAL( replayItem(QtHistoryItem *) ),SLOT( replayItem(QtHistoryItem *) ));
	connect (_historyWidget, SIGNAL( removeItem(unsigned int) ),SLOT( removeItem(unsigned int) ));
	connect(_historyWidget, SIGNAL(missedCallsSeen()), SLOT(resetUnseenMissedCalls()));

	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cHistory.getCWengoPhone().getPresentation();
	qtWengoPhone->setHistory(_historyWidget);
}

void QtHistory::historyLoadedEventHandler(CHistory & history) {
	updatePresentation();
}

void QtHistory::updatePresentation () {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtHistory::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtHistory::updatePresentationThreadSafe() {
	_historyWidget->clearHistory();

	HistoryMementoCollection * collection = _cHistory.getHistory().getHistoryMementoCollection();
	for(HistoryMap::iterator it = collection->begin(); it != collection->end(); it++ ) {
		HistoryMemento * memento = (*it).second;

		addHistoryMemento(
			HistoryMemento::stateToString(memento->getState()),
			memento->getDate().toString(),
			memento->getTime().toString(),
			memento->getDuration(),
			memento->getPeer(),
			(*it).first
		);
	}
}

void QtHistory::addHistoryMemento(const std::string & type, const std::string & date,
		const std::string & time, int duration, const std::string & name, unsigned int id) {

	QDate qdate = QDate::fromString(QString::fromStdString(date), "yyyy-MM-dd");
	QTime qtime = QTime::fromString(QString::fromStdString(time));
	QTime qduration;
	if( duration == -1 ) {
		qduration = qduration.addSecs(0);
	} else {
		qduration = qduration.addSecs(duration);
	}

	SipAddress sipAddress(name);
	QString formattedName = QString::fromStdString(sipAddress.getUserName());

	if( type == HistoryMemento::StateIncomingCall ) {
		_historyWidget->addIncomingCallItem(QString::fromStdString(type),
			qdate, qtime, qduration, formattedName, id);
	} else if ( type == HistoryMemento::StateOutgoingCall ) {
		_historyWidget->addOutGoingCallItem(QString::fromStdString(type),
			qdate, qtime, qduration, formattedName, id);
	} else if ( type == HistoryMemento::StateMissedCall ) {
		_historyWidget->addMissedCallItem(QString::fromStdString(type),
			qdate, qtime, qduration, formattedName, id);
	} else if ( type == HistoryMemento::StateRejectedCall) {
		_historyWidget->addRejectedCallItem(QString::fromStdString(type),
			qdate, qtime, qduration, formattedName, id);
	} else if ( type == HistoryMemento::StateOutgoingSMSOK) {
		_historyWidget->addSMSItem(QString::fromStdString(type),
			qdate, qtime, qduration, formattedName, id);
	} else if ( type == HistoryMemento::StateOutgoingSMSNOK) {
		//do not show unsent SMS for now
	} else if ( type == HistoryMemento::StateNone) {
		//nothing to add
	} else if ( type == HistoryMemento::StateAny) {
		//nothing to add
	}
}

void QtHistory::removeHistoryMemento(unsigned int id) {
	_cHistory.removeHistoryMemento(id);
}

void QtHistory::mementoAddedEventHandler(CHistory &, unsigned id) {
	updatePresentation();
}

void QtHistory::mementoUpdatedEventHandler(CHistory &, unsigned id) {
	updatePresentation();
}

void QtHistory::clearAllEntries() {
	_cHistory.clearAllEntries();
}

void QtHistory::clearSmsEntries() {
	_cHistory.clearSmsEntries();
}

void QtHistory::clearChatEntries() {
	_cHistory.clearChatEntries();
}

void QtHistory::clearIncomingCallEntries() {
	_cHistory.clearIncomingCallEntries();
}

void QtHistory::clearOutgoingCallEntries() {
	_cHistory.clearOutgoingCallEntries();
}

void QtHistory::clearMissedCallEntries() {
	_cHistory.clearMissedCallEntries();
}

void QtHistory::clearRejectedCallEntries() {
	_cHistory.clearRejectedCallEntries();
}

void QtHistory::replayItem(QtHistoryItem * item) {
	QString text = "";
	QString phoneNumber = "";
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cHistory.getCWengoPhone().getPresentation();
	switch ( item->getItemType() ) {

		case QtHistoryItem::Sms:
			//retrieve info & configure the Sms widget
			text = QString::fromStdString(_cHistory.getMementoData(item->getId()));
			phoneNumber = QString::fromStdString(_cHistory.getMementoPeer(item->getId()));

			//test existance of Sms (available only if a WengoAccount has been created)
			if( qtWengoPhone->getSms() ) {
				qtWengoPhone->getSms()->setText(text);
				qtWengoPhone->getSms()->setPhoneNumber(phoneNumber);
				qtWengoPhone->getSms()->getWidget()->show();
			}
			break;

		case QtHistoryItem::OutGoingCall:

			if( QMessageBox::question(
				_historyWidget,
				tr("Replay call"),
				tr("Do you want to call %1?").arg(item->text(QtHistoryItem::COLUMN_PEERS)),
				tr("&No"), tr("&Yes"),
				QString(), 0, 1) ) {
					_cHistory.replay(item->getId());
				}
			break;

		case QtHistoryItem::IncomingCall:
			//can't replay
			break;

		case QtHistoryItem::MissedCall:
			//can't replay
			break;

		case QtHistoryItem::Chat:
			break;

		default:
			break;
	}
}

void QtHistory::removeItem(unsigned id) {
	LOG_DEBUG("QtHistory::removeItem");
	_cHistory.removeHistoryMemento(id);
}

void QtHistory::mementoRemovedEventHandler(CHistory &, unsigned id) {
	updatePresentation();
}

void QtHistory::resetUnseenMissedCalls() {
	_cHistory.resetUnseenMissedCalls();
}
