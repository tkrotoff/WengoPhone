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

#ifndef OW_HISTORYMEMENTOLIST_H
#define OW_HISTORYMEMENTOLIST_H

#include "HistoryMemento.h"

#include <list>
#include <string>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/archive_exception.hpp>
#include <boost/serialization/map.hpp>

typedef std::map<int, HistoryMemento *> HistoryMap;

/**
 * HistoryMementoCollection stores a set of HistoryMemento
 *
 * @author Mathieu Stute
 */
class HistoryMementoCollection {
	friend class History;
public:

	
	static const unsigned int SERIALIZATION_VERSION = 1;
	
	/**
	 * Default constructor
	 */
	HistoryMementoCollection();
	
	/**
	 * Default destructor
	 */
	~HistoryMementoCollection();
	
	/**
	 * Add a HistoryMemento
	 * @param memento the HistoryMemento to add
	 * @return the id of the memento
	 */
	int addMemento(HistoryMemento * memento);
	
	/**
	 * get a HistoryMemento by its id
	 * @param id the id
	 * @return the HistoryMemento with the given id
	 */
	HistoryMemento * getMemento(int id);
	
	/**
	 * remove a memento by its id
	 * @param id the id
	 */
	void removeMemento(int id);
	
	/**
	 * Clear all mementos
	 */
	void clear(HistoryMemento::State state);

	/**
	 * Return the number of entries
	 * @return the number of entries
	 */
	int size();
	
	/**
	 * Return a string representing this object
	 * @return a string representing this object
	 */
	std::string toString();

	/**
	 * Returns an iterator pointing to the beginning of the map.
	 * @return an iterator pointing to the beginning of the map.
	 */
	HistoryMap::iterator begin();
	
	/**
	 * Returns an iterator pointing to the end of the map.
	 * @return an iterator pointing to the end of the map.
	 */
	HistoryMap::iterator end();

private:

	/**
	 * get a HistoryMementoCollection containing all mementos
	 * that match state
	 * @param state filter memento by State
	 * @param count specify number of mementos to be returned
	 * @return a pointer to a HistoryMementoCollection
	 */
	HistoryMementoCollection * getMementos(HistoryMemento::State state, int count = -1);

	/**
	 * Return the HistoryMemento associated to the given callId
	 * @param callId the callId
	 * @return the HistoryMemento associated to callId
	 */
	int getMementoByCallId(int callId);

	/**
	 * Return the HistoryMemento associated to the given SMS id
	 * @param callId the SMS id
	 * @return the HistoryMemento associated to SMS id
	 */
	int getMementoBySMSId(int callId);

	/**
	 * Add a HistoryMemento & do not increment the static int _historyId
	 * @param memento the HistoryMemento to add
	 * @param id the id of the memento
	 */
	void privateAdd(int id, HistoryMemento * memento);
	
	/**
	 * Set the max entries
	 * @param max max entries
	 */
	void setMaxEntries(int max);
	
	/**	 global static history id for mementos */
	static unsigned int _historyId;

	/**	 max entries of the history */
	int _maxEntries;

	/**	 private collection, handle a (id, memento) pair map */
	HistoryMap _privateCollection;
	
	friend class boost::serialization::access;
	
	/**	 serialialization method */
	template < class Archive >
	void serialize(Archive & ar, const unsigned int version) {
		ar & BOOST_SERIALIZATION_NVP(_historyId);
		ar & BOOST_SERIALIZATION_NVP(_privateCollection);
	}
};

BOOST_CLASS_VERSION(HistoryMementoCollection, HistoryMementoCollection::SERIALIZATION_VERSION)

#endif //OW_HISTORYMEMENTOLIST_H
