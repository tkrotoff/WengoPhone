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

#ifndef OWQTCREDITWIDGET_H
#define OWQTCREDITWIDGET_H

#include <qtutil/QObjectThreadSafe.h>

#include <QtCore/QString>

class CWengoPhone;

class QWidget;
namespace Ui { class CreditWidget; }

/**
 * Credit widget of the profile bar
 *
 * Shows the Wengo's credit available for the user.
 *
 * @author Mathieu Stute
 */
class QtCreditWidget : public QObjectThreadSafe {
	Q_OBJECT
public:

	QtCreditWidget(CWengoPhone & cWengoPhone, QWidget * parent);

	virtual ~QtCreditWidget();

	void setPstnNumber(const QString & number);

	void setCallForwardMode(const QString & callForwardMode);

	QWidget * getWidget() const;

	void updatePresentation();

private Q_SLOTS:

	void buyOutClicked();

	void callforwardModeClicked();

	void slotUpdatedTranslation();

private:

	QString _callForwardMode;

	QString _pstnNumber;

	void initThreadSafe();

	void updatePresentationThreadSafe();

	CWengoPhone & _cWengoPhone;

	QWidget * _widget;

	Ui::CreditWidget * _ui;
};

#endif	//OWQTCREDITWIDGET_H
