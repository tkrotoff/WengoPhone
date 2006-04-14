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

#ifndef LIST_H
#define LIST_H

#include <vector>

/**
 * List.
 *
 * @see java.util.List<E>
 * @see QList
 * @author Tanguy Krotoff
 */
template<typename T>
class List : public std::vector<T> {
public:

	/**
	 * Appends the specified element to the end of this list.
	 *
	 * @param element element to be appended to this list
	 */
	void operator+=(const T & element) {
		push_back(element);
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
		T & t = (*this)[index];
		erase(begin() + index);
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
		int count = size();
		int i;
		for (i = 0; i < count; i++) {
			if ((*this)[i] == element) {
				break;
			}
		}
		if (i < count) {
			erase(begin() + i);
			return true;
		}
		return false;
	}

	/**
	 * @see remove()
	 */
	T & operator-=(unsigned index) {
		return remove(index);
	}

	/**
	 * @see remove()
	 */
	bool operator-=(const T & element) {
		return remove(element);
	}

	/**
	 * Gets the number of occurrences of an element contained in this list.
	 *
	 * @param element element to find inside this list
	 * @return number of occurrences of the specified element contained in this list
	 */
	unsigned contains(const T & element) const {
		unsigned j = 0;
		for (unsigned i = 0; i < size(); i++) {
			if ((*this)[i] == element) {
				j++;
			}
		}

		return j;
	}
};

#endif	//LIST_H
