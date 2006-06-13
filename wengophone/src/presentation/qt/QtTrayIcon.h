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

#ifndef OWQTTRAYICON_H
#define OWQTTRAYICON_H

#include <QtGui>

class QtWengoPhone;
class TrayIcon;

class QtTrayIcon : public QObject {
	Q_OBJECT
public:

	QtTrayIcon(QObject * parent);

	QMenu * createStatusMenu();

	void updateCallMenu();

public Q_SLOTS:

	void setTrayMenu();

	void setSystrayIcon(QVariant status);

	void sysTrayDoubleClicked(const QPoint& );

	void hide();

	void slotSystrayMenuCallWengo(QAction * action);

	void slotSystrayMenuCallMobile(QAction * action);

	void slotSystrayMenuCallLandLine(QAction * action);

	void slotSystrayMenuStartChat(QAction * action);

	void slotSystrayMenuSendSms(QAction * action);

private:

	QMenu * createCallWengoTrayMenu();

	QMenu * createCallMobileTrayMenu();

	QMenu * createCallLandLineTrayMenu();

	QtWengoPhone * _qtWengoPhone;

	QMenu * _trayMenu;

	QMenu * _callMenu;

	QMenu * _sendSmsMenu;

	QMenu * _startChatMenu;

	QMenu * _callWengoMenu;

	QMenu * _callMobileMenu;

	QMenu * _callLandLineMenu;

	TrayIcon * _trayIcon;
};

#endif	//OWQTTRAYICON_H
