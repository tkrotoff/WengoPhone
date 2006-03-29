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

#include <presentation/qt/QtWengoPhone.h>
#include <control/CWengoPhone.h>
#include <model/history/History.h>

#include <util/Logger.h>

#include <QDate>
#include <QTime>

QtHistory::QtHistory( CHistory & cHistory ) : _cHistory(cHistory) {
	_cHistory.historyLoadedEvent += boost::bind(&QtHistory::historyLoadedEventHandler, this, _1);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtHistory::initThreadSafe, this));
	postEvent(event);
}

QWidget * QtHistory::getWidget() {
	return _historyWidget;
}

void QtHistory::initThreadSafe() {
	_historyWidget = new QtHistoryWidget();
	
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
	//TODO: clear the widget
	
	HistoryMementoCollection * collection = _cHistory.getHistory().getHistoryMementoCollection();
	for(HistoryMap::iterator it = collection->begin(); it != collection->end(); it++ ) {
		HistoryMemento * memento = (*it).second;
		
		addHistoryMemento(
			HistoryMemento::stateToString(memento->getState()),
			memento->getDate().toString(),
			memento->getTime().toString(),
			memento->getPeer(),
			(*it).first
		);
	}
}

void QtHistory::addHistoryMemento(std::string type,
	std::string date, std::string time, std::string name, unsigned id) {
	QDate qDate = QDate::fromString(QString::fromStdString(date));
	QTime qTime = QTime::fromString(QString::fromStdString(time));
	
	if( type == HistoryMemento::StateIncomingCall ) {
		_historyWidget->addIncomingCallItem(QString::fromStdString(type),
			qDate, QString::fromStdString(name), id);
	} else if ( type == HistoryMemento::StateOutgoingCall ) {
		_historyWidget->addOutGoingCallItem(QString::fromStdString(type),
			qDate, QString::fromStdString(name), id);
	} else if ( type == HistoryMemento::StateMissedCall ) {
		//_historyWidget->addSMSItem(QString::fromStdString(type), qDate, QString::fromStdString(name), id);
	} else if ( type == HistoryMemento::StateRejectedCall) {
		//_historyWidget->addSMSItem(QString::fromStdString(type), qDate, QString::fromStdString(name), id);
	} else if ( type == HistoryMemento::StateOutgoingSMSOK) {
		_historyWidget->addSMSItem(QString::fromStdString(type), qDate, QString::fromStdString(name), id);
	} else if ( type == HistoryMemento::StateOutgoingSMSNOK) {
		//do not show unsent SMS for now
	} else if ( type == HistoryMemento::StateNone) {
		//nothing to add
	} else if ( type == HistoryMemento::StateAny) {
		//nothing to add
	}
}

void QtHistory::clear() {
	_cHistory.clear();
}

void QtHistory::removeHistoryMemento(int id) {
	_cHistory.removeHistoryMemento(id);
}
