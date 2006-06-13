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

#ifndef OWQTCONTACTPIXMAP_H
#define OWQTCONTACTPIXMAP_H

#include <QHash>
#include <QPixmap>


/**
 * Store status pixmaps
 *
 * @author Mr K
 */

class QtContactPixmap {
public:

	enum ContactPixmap {
		ContactUnknown,
		ContactOnline,
		ContactOffline,
		ContactDND,
		ContactInvisible,
		ContactAway,
		ContactForward,
		ContactIM,
		ContactCall,
		ContactVideo,
		ContactGroupOpen,
		ContactGroupClose
	};

	static QtContactPixmap * getInstance();

	void setPixmap(ContactPixmap status, const QPixmap & px);

	QPixmap & getPixmap(ContactPixmap status);

protected:

	QtContactPixmap();

	QtContactPixmap(const QtContactPixmap & other) : _contactPixmap(other._contactPixmap) { }

	QtContactPixmap & operator = (const QtContactPixmap & other) {
		_contactPixmap = other._contactPixmap;
		return * this;
	}

	QHash < ContactPixmap, QPixmap > _contactPixmap;

private:

	static QtContactPixmap * _instance;
};

#endif	//OWQTCONTACTPIXMAP_H
