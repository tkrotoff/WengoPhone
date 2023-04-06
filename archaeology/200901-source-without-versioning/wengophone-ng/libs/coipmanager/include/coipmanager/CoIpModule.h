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

#ifndef OWCOIPMODULE_H
#define OWCOIPMODULE_H

#include <coipmanager/coipmanagerdll.h>

#include <util/Identifiable.h>
#include <util/Interface.h>

#include <QtCore/QObject>

class CoIpManager;

/**
 * Base class for all CoIp module.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API CoIpModule : public QObject, public Identifiable {
	Q_OBJECT
public:

	CoIpModule(CoIpManager & coIpManager);

	CoIpModule(const CoIpModule & coIpModule);

	virtual ~CoIpModule();

	/**
	 * Checks if the Module/Session has been already started or not.
	 *
	 * @return true if Module/Session already started; false otherwise
	 */
	bool isStarted() const;
	void setStarted(bool started);

Q_SIGNALS:

	/**
	 * Sent when the module is finished.
	 */
	void moduleFinishedSignal();

public Q_SLOTS:

	/**
	 * Starts the Module.
	 */
	virtual void start() = 0;

	/**
	 * Pauses the Module.
	 *
	 * No effect if paused or stopped.
	 */
	virtual void pause() = 0;

	/**
	 * Resumes the Module.
	 *
	 * No effect if not paused.
	 */
	virtual void resume() = 0;

	/**
	 * Stops the Module.
	 */
	virtual void stop() = 0;

protected:

	/**
	 * True if the Module/Session has been already started; false otherwise.
	 *
	 * Code factorization, to use in subclasses.
	 *
	 * @see CoIpModule::isStarted()
	 * @see CoIpModule::start()
	 * @see CoIpModule::stop()
	 */
	bool _started;

	/** Link to CoIpManager. */
	CoIpManager & _coIpManager;
};

#endif	//OWCOIPMODULE_H
