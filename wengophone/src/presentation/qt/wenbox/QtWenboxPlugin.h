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

#ifndef QTWENBOXPLUGIN_H
#define QTWENBOXPLUGIN_H

#include <presentation/PWenboxPlugin.h>

#include <qtutil/QObjectThreadSafe.h>

class CWenboxPlugin;
class QComboBox;
class Timer;

class QtWenboxPlugin : public QObjectThreadSafe, public PWenboxPlugin {
public:

	QtWenboxPlugin(CWenboxPlugin & cWenboxPlugin);

	~QtWenboxPlugin();

	void phoneNumberBufferUpdatedEvent(const std::string & phoneNumberBuffer);

	void updatePresentation() { }

private:

	void initThreadSafe();

	void updatePresentationThreadSafe() { }

	void phoneNumberBufferUpdatedEventThreadSafe(const std::string & phoneNumberBuffer);

	void timeoutEventHandler();

	CWenboxPlugin & _cWenboxPlugin;

	QComboBox * _phoneComboBox;

	Timer * _timer;
};

#endif	//QTWENBOXPLUGIN_H
