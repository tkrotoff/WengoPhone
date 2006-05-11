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

#ifndef QTCONFERENCEACTION_H
#define QTCONFERENCEACTION_H

#include <QtGui>

class PhoneCall;

class QtConferenceAction : public QAction {
	Q_OBJECT
public:

	QtConferenceAction(const QString & text, QObject * parent);

	QtConferenceAction(QObject * parent);

	QtConferenceAction(const QIcon & icon, const QString & text, QObject * parent);

	void setPhoneCall(PhoneCall * phoneCall);

	PhoneCall * getPhoneCall();

protected:

	PhoneCall * _phoneCall;
};

#endif	//QTCONFERENCEACTION_H
