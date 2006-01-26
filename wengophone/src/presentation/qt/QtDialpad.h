/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef QTDIALPAD_H
#define QTDIALPAD_H

#include <QObject>

class QWidget;
class QString;
class QtWengoPhone;

class QtDialpad : public QObject {
	Q_OBJECT
public:

	QtDialpad(QtWengoPhone * qtWengoPhone);

	QWidget * getWidget() const {
		return _dialpadWidget;
	}

private Q_SLOTS:

	void oneButtonClicked();

	void twoButtonClicked();

	void threeButtonClicked();

	void fourButtonClicked();

	void fiveButtonClicked();

	void sixButtonClicked();

	void sevenButtonClicked();

	void eightButtonClicked();

	void nineButtonClicked();

	void zeroButtonClicked();

	void starButtonClicked();

	void sharpButtonClicked();

private:

	void buttonClicked(const QString & num);

	QWidget * _dialpadWidget;

	QtWengoPhone * _qtWengoPhone;
};

#endif	//QTDIALPAD_H
