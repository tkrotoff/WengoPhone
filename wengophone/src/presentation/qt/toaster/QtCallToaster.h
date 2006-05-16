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
#ifndef QTCALLTOASTER_H
#define QTCALLTOASTER_H

#include <QtGui>



namespace Ui { class CallToaster; }

class QtCallToaster : public QWidget
{
Q_OBJECT

public:

    QtCallToaster(QWidget * parent = 0, Qt::WFlags f = 0);

	void setTitle(const QString & title);

	void setMessage(const QString & message);

	void showToaster();

	void setCloseTimer(int timer);

	void setPixmap(const QPixmap &pixmap);

Q_SIGNALS:

    void hangupButtonClicked();

    void callButtonClicked();

protected:

    void setupGui();

    Ui::CallToaster * _ui;

	QPoint _startPosition;

	int _timerId;

	int _closeTimerId;

	int _closeTimer;

	bool _show;

protected:

    void timerEvent(QTimerEvent *event);

protected Q_SLOTS:

    void hangupButtonSlot();

    void callButtonSlot();

public Q_SLOTS:

	void closeToaster();

};

#endif // QTCALLTOASTER_H
