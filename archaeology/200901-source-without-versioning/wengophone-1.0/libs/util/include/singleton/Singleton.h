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

#ifndef SINGLETON_H
#define SINGLETON_H

/**
 * Design Pattern Singleton.
 *
 * Example:
 * <pre>
 * class Toto : public Singleton<Toto> {
 *     friend class Singleton<Toto>;
 * public:
 *     void doSomething() { }
 * private:
 *     Toto() { }
 * };
 *
 * Toto & toto = Toto::getInstance();
 * toto.doSomething();
 * </pre>
 *
 * @author Tanguy Krotoff
 */
template<typename T>
class Singleton {
public:

	/**
	 * Gets the instance of the object.
	 *
	 * @return instance of the object
	 */
	static T & getInstance() {
		static T instance;

		return instance;
	}

protected:

	Singleton() { }
	virtual ~Singleton() { }
	Singleton(const Singleton &) { }
	Singleton & operator=(const Singleton &) { }
};

#endif	//SINGLETON_H
