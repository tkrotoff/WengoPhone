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
#ifndef QTTOASTER_H
#define QTTOASTER_H

#include <QtGui>

class QtWengoStyleLabel;


class QtToaster : public QWidget
{
	Q_OBJECT

public:

	QtToaster(QWidget * parent = 0, Qt::WFlags f = 0 );

	void setCloseButtonPixmap(const QPixmap & pixmap);

	void setTitle(const QString & title);

	void setMessage(const QString & message);

	void setButton1Title(const QString & title);

	void setButton1Pixmap(const QPixmap & pixmap);

	void setButton2Title(const QString & title);

	void setButton2Pixmap(const QPixmap & pixmap);

	void setButton3Title(const QString & title);

	void setButton3Pixmap(const QPixmap & pixmap);

	void hideButton(int num);

	void showToaster();

	void setCloseTimer(int timer);

	void setPixmap(const QPixmap &pixmap);

public Q_SLOTS:

	void closeToaster();

Q_SIGNALS:

    void button1Clicked();

    void button2Clicked();

    void button3Clicked();

protected:

	void setupGui();

	QWidget * _widget;

	QLabel * _title;

	QLabel * _message;

	QLabel * _pixmapLabel;

	QPushButton * _button1;

	QPushButton * _button2;

	QPushButton * _button3;

	QPushButton *  _closeButton;

	QPoint _startPosition;

	int _timerId;

	int _closeTimerId;

	int _closeTimer;

	bool _show;

protected Q_SLOTS:

    void button1Slot();

    void button2Slot();

    void button3Slot();

protected:

    void timerEvent(QTimerEvent *event);
};

#endif
