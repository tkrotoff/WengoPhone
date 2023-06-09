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

#ifndef QTPRESENCEMENUMANAGER_H
#define QTPRESENCEMENUMANAGER_H

#include <QtCore/QMap>
#include <QtCore/QObject>

#include <model/presence/Presence.h>

class CUserProfileHandler;
class QAction;
class QMenu;
class QPixmap;
class QString;
class QtWengoPhone;


/**
 * This class manages the menu actions for the global presence
 *
 * @author Aurelien Gateau
 */
class QtPresenceMenuManager : public QObject {
	Q_OBJECT
public:
	QtPresenceMenuManager(QObject * parent, CUserProfileHandler * cUserProfileHandler, 
		QtWengoPhone * qtWengoPhone);

	/**
	 * Fills menu with actions to modify the global presence
	 */
	void addPresenceActions(QMenu* menu);

private Q_SLOTS:
	void slotGlobalPresenceActionTriggered();
	void logOff();

private:
	void addGlobalPresenceAction(QMenu* menu, EnumPresenceState::PresenceState state, const QPixmap& pix, const QString& text);

	QMap<QObject*, EnumPresenceState::PresenceState> _actionToState;
	CUserProfileHandler* _cUserProfileHandler;
	
	QtWengoPhone * _qtWengoPhone;
};

#endif	// QTPRESENCEMENUMANAGER_H
