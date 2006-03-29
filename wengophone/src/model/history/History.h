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

#ifndef OW_HISTORY_H
#define OW_HISTORY_H

#include "HistoryMemento.h"
#include "HistoryMementoCollection.h"

#include <util/Event.h>
#include <thread/Mutex.h>
#include <serialization/Serializable.h>

/**
 * History (Care Taker in the Memento pattern)
 *
 * exemple: serialization example
 *	<pre>
 *	std::ofstream ofs("history.log");
 *	ofs << History::history.serialize();
 *	ofs.close();
 *	</pre>
 * 
 * exemple: unserialize a History
 *	<pre>
 * 	std::string lines;
 *
 *	// assum you have read the save file
 *	// and you have the content of this file
 *	// in the std::string lines
 * 
 *	History::history.unserialize(lines);
 *	</pre>
 *
 * @author Mathieu Stute
 */
class History : public Serializable {
public:

	/** Singleton. */
	static History & getInstance();
	
	/**
	 * The history has been loaded
	 */
	Event<void (History &)> historyLoadedEvent;

	/**
	 * The history has been saved
	 */
	Event<void (History &)> historySavedEvent;
	
	/**
	 * A memento has been added
	 */
	Event<void (History &, int id)> mementoAddedEvent;
	
	/**
	 * A memento has been updated
	 */
	Event<void (History &, int id)> mementoUpdatedEvent;
	
	/**
	 * A memento has been removed
	 */
	Event<void (History &, int id)> mementoRemovedEvent;

	/**
	 * destructor
	 */
	~History();
	
	/**
	 * load the history from a file
	 * @param filename filename to load
	 */
	void load(std::string filename);
	
	/**
	 * save the history to a file
	 * @param filename filename
	 */
	void save(std::string filename);

	/**
	 * get a HistoryMemento by its id
	 * @param id the id
	 * @return the HistoryMemento with the given id
	 */
	HistoryMemento * getMemento(int id);

	/**
	 * get a HistoryMementoCollection containing all mementos
	 * that match state
	 * @param state filter memento by State
	 * @param count specify number of mementos to be returned
	 * @return a pointer to a HistoryMementoCollection
	 */
	HistoryMementoCollection * getMementos(HistoryMemento::State state, int count = -1);

	/**
	 * remove a memento by its id
	 * @param id the id
	 */
	void removeMemento(int id);
	
	/**
	 * Clear all entries of the history
	 */
	void clear();

	/**
	 * return the number of HistoryMemento's
	 * @return the number of HistoryMemento's
	 */
	int size();

	/**
	 * update a memento duration (for incoming/outgoing calls)
	 * @param callId callId associated to the memento
	 * @param duration duration
	 */
	void updateCallDuration(int callId, int duration);
	
	/**
	 * update a memento internal state (for SMS)
	 * @param callId callId associated to the memento
	 * @param state new state
	 */
	void updateSMSState(int callId, HistoryMemento::State state);

	/**
	 * add a history memento to the history
	 * @param memento the memento to add
	 * @return the id of the memento
	 */
	int addMemento(HistoryMemento * memento);

	/**
	 * Return a string representing this object
	 * @return a string representing this object
	 */
	std::string toString();

	/**
	 * return a pointer to the HistoryMementoCollection
	 * @return a pointer to the HistoryMementoCollection
	 */
	HistoryMementoCollection * getHistoryMementoCollection();

private:

	/**
	 * Default constructor
	 */
	History();

	/**
	 * Serialize the history
	 */
	std::string serialize();
	
	/**
	 * Serialize the history
	 */
	bool unserialize(const std::string & data);
	
	/**
	 * Private HistoryMementoList that store
	 * all HistoryMemento objects.
	 */
	HistoryMementoCollection *_collection;
	
	/**	 global static mutex for singleton access */
	static Mutex _mutex;
};
#endif //OW_HISTORY_H
