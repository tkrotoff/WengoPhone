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

#ifndef LIST_H
#define LIST_H

#include <exception/OutOfRangeException.h>

#include <vector>

/**
 * List.
 *
 * @see java.util.List<E>
 * @see QList
 * @author Tanguy Krotoff
 */
template<typename T>
class List {
public:

	List() {
		_list.empty();
	}

	virtual ~List() {
		clear();
	}

	/**
	 * Appends the specified element to the end of this list.
	 *
	 * @param element element to be appended to this list 
	 */
	void add(const T & element) {
		_list.push_back(element);
	}

	/**
	 * @see add()
	 */
	void operator+=(const T & element) {
		add(element);
	}

	/**
	 * Removes the element at the specified position in this list.
	 *
	 * Shifts any subsequent elements to the left (subtracts one from their indices).
	 *
	 * @param index position of the element to removed
	 * @return element removed at position index
	 */
	T & remove(unsigned index) {
		T & t = get(index);
		_list.erase(_list.begin() + index);
		return t;
	}

	/**
	 * Removes the first occurrence in this list of the specified element.
	 *
	 * Does not delete the element, just remove it from the list.
	 *
	 * @param element to remove from the list
	 * @return true if the element was removed; false otherwise
	 */
	bool remove(const T & element) {
		int count = _list.size();
		int i;
		for (i = 0; i < count; i++) {
			if (_list[i] == element) {
				break;
			}
		}
		if (i < count) {
			_list.erase(_list.begin() + i);
			return true;
		}
		return false;
	}

	/**
	 * @see remove()
	 */
	T & operator-=(unsigned index) {
		remove(index);
	}

	/**
	 * @see remove()
	 */
	bool operator-=(const T & element) {
		return remove(element);
	}

	/**
	 * Removes all of the elements from this list.
	 *
	 * This list will be empty after this call returns.
	 */
	void clear() {
		_list.clear();
	}

	/**
	 * Returns true if this list contains no elements.
	 *
	 * @return true if this list contains no elements
	 */
	bool isEmpty() const {
		return _list.empty();
	}

	/**
	 * Gets the number of occurrences of an element contained in this list.
	 *
	 * @param element element to find inside this list
	 * @return number of occurrences of the specified element contained in this list
	 */
	unsigned contains(const T & element) const {
		unsigned j = 0;
		for (unsigned i = 0; i < _list.size(); i++) {
			if (_list[i] == element) {
				j++;
			}
		}

		return j;
	}

	/**
	 * Returns the number of elements in this list.
	 *
	 * @return the number of elements in this list
	 */
	unsigned size() const {
		return _list.size();
	}

	/**
	 * Returns the element at the specified position in this list.
	 *
	 * Permits to use List as an array.
	 * <pre>
	 * for (unsigned i = 0; i  list.size(); i++) {
	 *     tmp = list[i];
	 * }
	 * </pre>
	 *
	 * Warning: slow operation.
	 *
	 * @param index index of element to return
	 * @return the element at the specified position in this list
	 * @throw OutOfRangeException if index >= size()
	 */
	T & operator[](unsigned index) throw (OutOfRangeException) {
		return get(index);
	}

	/**
	 * @see operator[]
	 */
	T & get(unsigned index) throw (OutOfRangeException) {
		if (index >= _list.size()) {
			throw OutOfRangeException("index out of range");
		} else {
			return _list[index];
		}
	}

	/**
	 * @see operator[]
	 */
	const T & get(unsigned index) const throw (OutOfRangeException) {
		if (index >= _list.size()) {
			throw OutOfRangeException("index out of range");
		} else {
			return _list[index];
		}
	}

	/**
	 * @see operator[]
	 */
	const T & operator[](unsigned index) const throw (OutOfRangeException) {
		return get(index);
	}

protected:

	/**
	 * The list itself.
	 */
	std::vector<T> _list;
};

#endif	//LIST_H
