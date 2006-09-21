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

#ifndef QTHISTORY_H
#define QTHISTORY_H

#include <presentation/PHistory.h>

#include <model/history/HistoryMemento.h>

#include <QtCore/QObject>

class CHistory;
class QtHistoryWidget;
class QtHistoryItem;

/**
 * Qt Presentation component for history.
 *
 * @author Mathieu Stute
 */
class QtHistory : public QObject, public PHistory {
	Q_OBJECT
public:

	QtHistory(CHistory & cHistory);

	~QtHistory();

	void updatePresentation();

	void addHistoryMemento(HistoryMemento::State, const std::string & date,
		const std::string & time, int duration, const std::string & name, unsigned id);

	void clearAllEntries();

	void removeHistoryMemento(unsigned id);

	void historyLoadedEvent();

	void mementoAddedEvent(unsigned id);

	void mementoUpdatedEvent(unsigned id);

	void mementoRemovedEvent(unsigned id);

	void unseenMissedCallsChangedEvent(int count);

	void clearSmsEntries();

	void clearChatEntries();

	void clearIncomingCallEntries();

	void clearOutgoingCallEntries();

	void clearMissedCallEntries();

	void clearRejectedCallEntries();

	QWidget * getWidget() const;

public Q_SLOTS:

	void replayItem(QtHistoryItem * item);

	void removeItem(unsigned id);

private Q_SLOTS:

	void resetUnseenMissedCalls();

	void showOnlyItemOfTypeSlot(int state);

private:

	QtHistoryWidget * _historyWidget;

	HistoryMemento::State _stateFilter;

	CHistory & _cHistory;
};

#endif	//QTHISTORY_H
