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

#ifndef OWQTMACAPPLICATION_H
#define OWQTMACAPPLICATION_H

#include <QtGui/QApplication>

/**
 * QApplication subclass for MacOS X.
 *
 * @author Philippe Bernery
 */
class QtMacApplication : public QApplication {
	Q_OBJECT
public:

	QtMacApplication(int & argc, char ** argv);

	virtual ~QtMacApplication();

Q_SIGNALS:

	/** kHICommandQuit received. */
	void applicationMustQuit();

	/** kHICommandShowAll received. */
	void applicationMustShow();

protected:

	bool macEventFilter(EventHandlerCallRef caller, EventRef event);

	bool _quitCalled;

};

#endif //OWQTMACAPPLICATION_H
