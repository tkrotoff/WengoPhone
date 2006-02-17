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

#ifndef PWENBOXPLUGIN_H
#define PWENBOXPLUGIN_H

#include "Presentation.h"

#include <string>

/**
 *
 * @ingroup presentation
 * @author Tanguy Krotoff
 */
class PWenboxPlugin : public Presentation {
public:

	virtual void keyNumberPressedEvent(const std::string & number) = 0;

	virtual void keyPickUpPressedEvent() = 0;

	virtual void keyHangUpPressedEvent() = 0;

protected:

	virtual void keyNumberPressedEventThreadSafe(const std::string & number) = 0;

	virtual void keyPickUpPressedEventThreadSafe() = 0;

	virtual void keyHangUpPressedEventThreadSafe() = 0;
};

#endif	//PWENBOXPLUGIN_H
