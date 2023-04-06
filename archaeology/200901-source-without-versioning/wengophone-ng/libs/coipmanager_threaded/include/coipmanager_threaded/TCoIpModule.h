/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWTCOIPMODULE_H
#define OWTCOIPMODULE_H

#include <coipmanager_threaded/tcoipmanagerdll.h>

#include <QtCore/QObject>

class CoIpModule;
class TCoIpManager;

/**
 * Threaded CoIpModule.
 *
 * @see CoIpModule
 * @ingroup TCoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_THREADED_API TCoIpModule : public QObject {
	Q_OBJECT
public:

	TCoIpModule(TCoIpManager & tCoIpManager, CoIpModule * module);

	virtual ~TCoIpModule();

	/**
	 * @see CoIpModule::start
	 */
	void start();

	/**
	 * @see CoIpModule::pause
	 */
	void pause();

	/**
	 * @see CoIpModule::resume
	 */
	void resume();

	/**
	 * @see CoIpModule::stop
	 */
	void stop();

Q_SIGNALS:

	/**
	 * @see CoIpModule::moduleFinishedSignal()
	 */
	void moduleFinishedSignal();

protected:

	/** Link to TCoIpManager. */
	TCoIpManager & _tCoIpManager;

	/** Link to CoIpModule. */
	CoIpModule * _module;
};

#endif	//OWTCOIPMODULE_H
