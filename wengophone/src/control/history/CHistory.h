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

class History;
class CWengoPhone;
class PHistory;

/**
 * @class CHistory, control for History
 * @ingroup control
 * @author Mathieu Stute
 */
class CHistory {
public:

	/**
	 * The history has been loaded
	 */
	Event<void (CHistory &)> historyLoadedEvent;
	
	/**
	 * A memento has been added
	 */
	Event<void (CHistory &, int id)> mementoAddedEvent;
	
	/**
	 * A memento has been updated
	 */
	Event<void (CHistory &, int id)> mementoUpdatedEvent;

	/**
	 * A memento has been removed
	 */
	Event<void (CHistory &, int id)> mementoRemovedEvent;

	/**
	 * Default constructor
	 */
	CHistory(History & history, CWengoPhone & cWengoPhone);

	/**
	 * @see History::removeMemento
	 */
	void removeHistoryMemento(int id);

	/**
	 * @see History::clear
	 */
	void clear();

	/**
	 * retrive the CWengoPhone
	 * @return the CWengoPhone
	 */
	CWengoPhone & getCWengoPhone() const;

	/**
	 * retrive the History
	 * @return the History
	 */
	History & getHistory();

private:

	void historyMementoAddedEventHandler(History &, int id);
	
	void historyMementoUpdatedEventHandler(History &, int id);

	void historyMementoRemovedEventHandler(History &, int id);

	void historyLoadedEventHandler(History &);

	/** link to the model. */
	History & _history;

	/** link to the CWengoPhone. */
	CWengoPhone & _cWengoPhone;

	/** link to the presentation via an interface. */
	PHistory * _pHistory;
};
#endif	//OW_CHISTORY_H
