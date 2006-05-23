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

#include "ui_CreditWidget.h"

#include <qtutil/QObjectThreadSafe.h>

#include <QtGui>

class CWengoPhone;
class QtClickableLabel;
class CWengoPhone;
class MousePressEventFilter;

/**
 * Credit widget
 *
 * @author Mr K.
 * @author Mathieu Stute
 */
class QtCreditWidget : public QObjectThreadSafe {
	Q_OBJECT
public:

	QtCreditWidget(CWengoPhone & cWengoPhone, QWidget * parent = 0, Qt::WFlags f = 0);

	void setPstnNumber(const QString & number);

	void setCallForwardMode(const QString & mode);

	QWidget * getWidget();

	void updatePresentation();

private Q_SLOTS:

	void buyOutClicked();

	void callforwardModeClicked();

	void slotUpdatedTranslation();

protected:

	QString _callForwardMode;

	QString _pstnNumber;

	void initThreadSafe();

	void updatePresentationThreadSafe();

	QWidget * _widget;

	CWengoPhone & _cWengoPhone;

	Ui::CreditWidget * _ui;

	MousePressEventFilter * _callForwardMouseFilter;
};
#endif
