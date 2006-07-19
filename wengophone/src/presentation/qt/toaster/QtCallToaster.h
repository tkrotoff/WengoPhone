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

#ifndef OWQTCALLTOASTER_H
#define OWQTCALLTOASTER_H

#include <QObject>
#include <QPoint>

class QString;
class QPixmap;
class QTimerEvent;

namespace Ui { class CallToaster; }

/**
 * Shows a toaster when a phone call or a chat is incoming.
 *
 * A toaster is a small window in the lower right of the desktop.
 *
 * @author Tanguy Krotoff
 */
class QtCallToaster : public QObject {
	Q_OBJECT
public:

	QtCallToaster(QWidget * parent);

	/**
	 * Sets the toaster window title.
	 *
	 * @param title toaster window title
	 */
	void setTitle(const QString & title);

	/**
	 * Sets the toaster window message.
	 *
	 * @param message toaster message
	 */
	void setMessage(const QString & message);

	/**
	 * Sets the toaster window picture.
	 *
	 * @param pixmap toaster picture
	 */
	void setPixmap(const QPixmap & pixmap);

	/**
	 * Shows the toaster widget.
	 */
	void show();

public Q_SLOTS:

	void close();

Q_SIGNALS:

	void hangupButtonClicked();

	void callButtonClicked();

private Q_SLOTS:

	void hangupButtonSlot();

	void callButtonSlot();

private:

	void setCloseTimer(int timer);

	void timerEvent(QTimerEvent * event);

	Ui::CallToaster * _ui;

	QWidget * _callToasterWidget;

	QPoint _startPosition;

	int _timerId;

	int _closeTimerId;

	int _closeTimer;

	bool _show;
};

#endif	//OWQTCALLTOASTER_H
