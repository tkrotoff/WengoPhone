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

#include <qtutil/SafeConnect.h>

#include <util/Logger.h>

#include <QtCore/QObject>

void SafeConnect::connect(const QObject * sender, const char * signal, const QObject * receiver, const char * method, Qt::ConnectionType type) {
	if (!QObject::connect(sender, signal, receiver, method, type)) {
		LOG_FATAL("couldn't connect signal=" + String(signal) + " to method=" + String(method));
	}
}

void SafeConnect::disconnect(const QObject * sender, const char * signal, const QObject * receiver, const char * method) {
	if (!QObject::disconnect(sender, signal, receiver, method)) {
		LOG_FATAL("couldn't disconnect signal=" + String(signal) + " to method=" + String(method));
	}
}
