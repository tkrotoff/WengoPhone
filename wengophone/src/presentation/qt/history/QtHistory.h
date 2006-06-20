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

#include <control/history/CHistory.h>

#include <util/Trackable.h>

#include <qtutil/QObjectThreadSafe.h>

class QtHistoryWidget;
class QtHistoryItem;

/**
 * Qt Presentation component for history.
 *
 * @author Mathieu Stute
 */
class QtHistory : public QObjectThreadSafe, public PHistory, public Trackable {
	Q_OBJECT
public:

	QtHistory ( CHistory & cHistory );

	virtual ~QtHistory();

	void updatePresentation ();

	void addHistoryMemento(HistoryMemento::State, const std::string & date,
		const std::string & time, int duration, const std::string & name, unsigned int id);

	void removeHistoryMemento(unsigned int id);

	void clearAllEntries();

	void clearSmsEntries();

	void clearChatEntries();

	void clearIncomingCallEntries();

	void clearOutgoingCallEntries();

	void clearMissedCallEntries();

	void clearRejectedCallEntries();

	QWidget * getWidget();

public Q_SLOTS:

	void replayItem ( QtHistoryItem * item );

	void removeItem( unsigned int id);

private Q_SLOTS:

	void resetUnseenMissedCalls();

private:

	void initThreadSafe();

	void updatePresentationThreadSafe();

	void historyLoadedEventHandler(CHistory &);

	void mementoAddedEventHandler(CHistory &, unsigned int id);

	void mementoUpdatedEventHandler(CHistory &, unsigned int id);

	void mementoRemovedEventHandler(CHistory &, unsigned int id);

	QtHistoryWidget * _historyWidget;

	CHistory & _cHistory;
};

#endif	//QTHISTORY_H
