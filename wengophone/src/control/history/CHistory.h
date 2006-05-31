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

#ifndef OW_CHISTORY_H
#define OW_CHISTORY_H

#include <util/Event.h>
#include <model/history/History.h>

class CWengoPhone;
class PHistory;
class Thread;

#include <string>

/**
 * @class CHistory, control for History
 * @ingroup control
 * @author Mathieu Stute
 */
class CHistory {
public:

	/**
	 * The history has been loaded.
	 */
	Event<void (CHistory &)> historyLoadedEvent;
	
	/**
	 * A memento has been added.
	 */
	Event<void (CHistory &, unsigned int id)> mementoAddedEvent;
	
	/**
	 * A memento has been updated.
	 */
	Event<void (CHistory &, unsigned int id)> mementoUpdatedEvent;

	/**
	 * A memento has been removed.
	 */
	Event<void (CHistory &, unsigned int id)> mementoRemovedEvent;

	/**
	 * Unseen missed calls changed event.
	 */
	Event<void (CHistory &, int count)> unseenMissedCallsChangedEvent;

	/**
	 * Default constructor.
	 */
	CHistory(History & history, CWengoPhone & cWengoPhone, Thread & modelThread);

	/**
	 * @see History::removeMemento
	 */
	void removeHistoryMemento(unsigned int id);
	void removeHistoryMementoThreadSafe(unsigned int id);

	/**
	 * @see History::clear
	 */
	void clear(HistoryMemento::State state = HistoryMemento::Any);
	void clearThreadSafe(HistoryMemento::State state);

	/**
	 * @see History::replay
	 */
	void replay(unsigned int id);
	void replayThreadSafe(unsigned int id);

	/**
	 * @see History::getMementos
	 */
	 HistoryMementoCollection * getMementos(HistoryMemento::State state, int count = -1);

	/**
	 * Get the memento data.
	 *
	 * @param id the memento id
	 * @return the memento data (the sms text)
	 */
	std::string getMementoData(unsigned int id);
	
	/**
	 * Get the memento peer.
	 *
	 * @param id the memento id
	 * @return the memento peer
	 */
	std::string getMementoPeer(unsigned int id);

	/**
	 * Reset unseen missed calls.
	 *
	 */
	void resetUnseenMissedCalls();
	void resetUnseenMissedCallsThreadSafe();

	/**
	 * Returns the unseen missed calls count.
	 *
	 * @return unseen missed calls count
	 */
	int getUnseenMissedCalls();

	/**
	 * retrive the CWengoPhone.
	 *
	 * @return the CWengoPhone
	 */
	CWengoPhone & getCWengoPhone() const;

	/**
	 * retrive the History.
	 *
	 * @return the History
	 */
	History & getHistory();

private:

	void historyMementoAddedEventHandler(History &, unsigned int id);

	void historyMementoUpdatedEventHandler(History &, unsigned int id);

	void historyMementoRemovedEventHandler(History &, unsigned int id);

	void historyLoadedEventHandler(History &);

	void unseenMissedCallsChangedEventhandler(History &, int count);

	/** link to the model. */
	History & _history;

	/** link to the CWengoPhone. */
	CWengoPhone & _cWengoPhone;

	/** link to the presentation via an interface. */
	PHistory * _pHistory;

	/** Reference to model Thread. Used to post event to the model thread. */
	Thread & _modelThread;
};
#endif	//OW_CHISTORY_H
