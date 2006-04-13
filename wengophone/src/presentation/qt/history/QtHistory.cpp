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

#include <util/Logger.h>

#include <QDate>
#include <QTime>

QtHistory::QtHistory( CHistory & cHistory ) : _cHistory(cHistory) {
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
	connect (_historyWidget, SIGNAL( removeItem(int) ),SLOT( removeItem(int) ));
	
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

void QtHistory::addHistoryMemento(std::string type,	std::string date, 
		std::string time, int duration, std::string name, unsigned id) {
	QDate qdate = QDate::fromString(QString::fromStdString(date), "yyyy-MM-dd");
	QTime qtime = QTime::fromString(QString::fromStdString(time));
	QTime qduration;
	if( duration == -1 ) {
		qduration = qduration.addSecs(0);
	} else {
		qduration = qduration.addSecs(duration);
	}
	
	if( type == HistoryMemento::StateIncomingCall ) {
		_historyWidget->addIncomingCallItem(QString::fromStdString(type),
			qdate, qtime, qduration, QString::fromStdString(name), id);
	} else if ( type == HistoryMemento::StateOutgoingCall ) {
		_historyWidget->addOutGoingCallItem(QString::fromStdString(type),
			qdate, qtime, qduration, QString::fromStdString(name), id);
	} else if ( type == HistoryMemento::StateMissedCall ) {
		_historyWidget->addMissedCallItem(QString::fromStdString(type),
			qdate, qtime, qduration, QString::fromStdString(name), id);
	} else if ( type == HistoryMemento::StateRejectedCall) {
		_historyWidget->addRejectedCallItem(QString::fromStdString(type),
			qdate, qtime, qduration, QString::fromStdString(name), id);
	} else if ( type == HistoryMemento::StateOutgoingSMSOK) {
		_historyWidget->addSMSItem(QString::fromStdString(type),
			qdate, qtime, qduration, QString::fromStdString(name), id);
	} else if ( type == HistoryMemento::StateOutgoingSMSNOK) {
		//do not show unsent SMS for now
	} else if ( type == HistoryMemento::StateNone) {
		//nothing to add
	} else if ( type == HistoryMemento::StateAny) {
		//nothing to add
	}
}

void QtHistory::removeHistoryMemento(int id) {
	_cHistory.removeHistoryMemento(id);
}

void QtHistory::mementoAddedEventHandler(CHistory &, int id) {
		
	HistoryMemento * memento = _cHistory.getHistory().getMemento(id);
	if( memento ) {
	
		addHistoryMemento(
			HistoryMemento::stateToString(memento->getState()),
			memento->getDate().toString(),
			memento->getTime().toString(),
			memento->getDuration(),
			memento->getPeer(),
			id
		);
	}
}

void QtHistory::mementoUpdatedEventHandler(CHistory &, int id) {
	HistoryMemento * memento = _cHistory.getHistory().getMemento(id);
	if( memento ) {
		QString type = QString::fromStdString(HistoryMemento::stateToString(memento->getState()));
		std::string date = memento->getDate().toString();
		std::string time = memento->getTime().toString();
		QDate qdate = QDate::fromString(QString::fromStdString(date), "yyyy-MM-dd");
		QTime qtime = QTime::fromString(QString::fromStdString(time));
		QString peer = QString::fromStdString(memento->getPeer());
		QTime qduration = QTime();
		qduration = qduration.addSecs(memento->getDuration());
		
		_historyWidget->editItem(type, qdate, qtime, qduration, peer, id);
	}
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

void QtHistory::replayItem ( QtHistoryItem * item ) {
	QString text = "";
	QString phoneNumber = "";
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cHistory.getCWengoPhone().getPresentation();
	switch ( item->getItemType() ) {
	
		case QtHistoryItem::Sms:
			//retrive info & configure the Sms widget
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
			_cHistory.replay(item->getId());
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

void QtHistory::removeItem(int id) {
	LOG_DEBUG("QtHistory::removeItem");
	_cHistory.removeHistoryMemento(id);
}

void QtHistory::mementoRemovedEventHandler(CHistory &, int id) {
	updatePresentation();
}
