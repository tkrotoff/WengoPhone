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

#ifndef QTSMS_H
#define QTSMS_H

#include <presentation/PSms.h>

#include <control/webservices/sms/CSms.h>

#include <qtutil/QObjectThreadSafe.h>

#include <QString>

class QtWengoPhone;

class QWidget;
class QPushButton;
class QTextEdit;
class QDialog;
namespace Ui { class SmsWindow; }

/**
 * Qt Presentation component for SMS.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class QtSms : public QObjectThreadSafe, public PSms {
	Q_OBJECT
public:

	QtSms(CSms & cSms);

	QWidget * getWidget() const;

	void updatePresentation();

	void setPhoneNumber(const QString & phoneNumber);

	void setText(const QString & text);

private Q_SLOTS:

	void sendButtonClicked();

private:

	void initThreadSafe();

	void updatePresentationThreadSafe();

	void smsStatusEventHandler(Sms & sender, int smsId, Sms::SmsStatus status);

	void smsStatusEventHandlerThreadSafe(Sms::SmsStatus status);

	CSms & _cSms;

	QtWengoPhone * _qtWengoPhone;

	Ui::SmsWindow * _ui;

	QDialog * _smsWindow;
};

#endif	//QTSMS_H
