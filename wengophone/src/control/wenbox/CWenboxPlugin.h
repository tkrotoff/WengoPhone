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

#ifndef CWENBOXPLUGIN_H
#define CWENBOXPLUGIN_H

#include <wenbox/Wenbox.h>

class CWengoPhone;
class PWenboxPlugin;
class WenboxPlugin;

/**
 *
 * @ingroup control
 * @author Tanguy Krotoff
 */
class CWenboxPlugin {
public:

	CWenboxPlugin(WenboxPlugin & wenboxPlugin, CWengoPhone & cWengoPhone);

	~CWenboxPlugin();

	CWengoPhone & getCWengoPhone() const {
		return _cWengoPhone;
	}

private:

	void keyPressedEventHandler(IWenbox & sender, IWenbox::Key key);

	/** Direct link to the model. */
	WenboxPlugin & _wenboxPlugin;

	/** Direct link to the presentation via an interface. */
	PWenboxPlugin * _pWenboxPlugin;

	CWengoPhone & _cWengoPhone;
};

#endif	//CWENBOXPLUGIN_H
