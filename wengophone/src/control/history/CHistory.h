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

#ifndef OWCHISTORY_H
#define OWCHISTORY_H

#include <model/history/History.h>

#include <util/Event.h>
#include <util/Trackable.h>

#include <string>

class CWengoPhone;
class PHistory;

/**
 * Control for History.
 *
 * @ingroup control
 * @author Mathieu Stute
 */
class CHistory : public Trackable {
public:

	/**
	 * The history has been loaded.
	 */
	Event<void (CHistory &)> historyLoadedEvent;

	/**
	 * A memento has been added.
	 */
	Event<void (CHistory &, unsigned id)> mementoAddedEvent;

	/**
	 * A memento has been updated.
	 */
	Event<void (CHistory &, unsigned id)> mementoUpdatedEvent;

	/**
	 * A memento has been removed.
	 */
	Event<void (CHistory &, unsigned id)> mementoRemovedEvent;

	/**
	 * Unseen missed calls changed event.
	 */
	Event<void (CHistory &, int count)> unseenMissedCallsChangedEvent;

	/**
	 * Default constructor.
	 */
	CHistory(History & history, CWengoPhone & cWengoPhone);

	~CHistory();

	/**
	 * @see History::removeMemento
	 */
	void removeHistoryMemento(unsigned id);

	/**
	 * @see History::clear
	 */
	void clear(HistoryMemento::State state = HistoryMemento::Any);

	/**
	 * @see History::replay
	 */
	void replay(unsigned id);

	/**
	 * @see History::getMementos
	 */
	 HistoryMementoCollection * getMementos(HistoryMemento::State state, int count = -1);

	/**
	 * Gets the memento data.
	 *
	 * @param id the memento id
	 * @return the memento data (the sms text)
	 */
	std::string getMementoData(unsigned id) const;

	/**
	 * Gets the memento peer.
	 *
	 * @param id the memento id
	 * @return the memento peer
	 */
	std::string getMementoPeer(unsigned id) const;

	/**
	 * Resets unseen missed calls.
	 */
	void resetUnseenMissedCalls();

	/**
	 * Gets the unseen missed calls count.
	 *
	 * @return unseen missed calls count
	 */
	int getUnseenMissedCalls();

	/**
	 * Retreives the CWengoPhone.
	 *
	 * @return the CWengoPhone
	 */
	CWengoPhone & getCWengoPhone() const;

	/**
	 * Retreives the History.
	 *
	 * @return the History
	 */
	History & getHistory();

private:

	void historyMementoAddedEventHandler(History &, unsigned id);

	void historyMementoUpdatedEventHandler(History &, unsigned id);

	void historyMementoRemovedEventHandler(History &, unsigned id);

	void historyLoadedEventHandler(History &);

	void unseenMissedCallsChangedEventhandler(History &, int count);

	void removeHistoryMementoThreadSafe(unsigned id);

	void clearThreadSafe(HistoryMemento::State state);

	void replayThreadSafe(unsigned id);

	void resetUnseenMissedCallsThreadSafe();

	/** Link to the model. */
	History & _history;

	/** Link to the CWengoPhone. */
	CWengoPhone & _cWengoPhone;

	/** Link to the presentation via an interface. */
	PHistory * _pHistory;
};

#endif	//OWCHISTORY_H
