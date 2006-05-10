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

#ifndef QTCREDITWIDGET_H
#define QTCREDITWIDGET_H

#include <QtGui>

class CWengoPhone;
class QtClickableLabel;

/**
 * Credit widget
 *
 * @author Mr K.
 * @author Mathieu Stute
 */
class QtCreditWidget : public QWidget {
	Q_OBJECT
public:

	QtCreditWidget(QWidget * parent = 0, Qt::WFlags f = 0);

	void setCWengoPhone(CWengoPhone * cwengophone);

	void setPstnNumber(const QString & number);

	void setCallForwardMode(const QString & mode);

private Q_SLOTS:

	void buyOutClicked();

	void callforwardModeClicked();

private:

	QLabel * _pstnNumber;

	QtClickableLabel * _callForwardMode;

	QGridLayout * _gridLayout;

protected:

	CWengoPhone * _cWengoPhone;
};
#endif
