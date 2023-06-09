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

#ifndef QTSUBSCRIBE2_H
#define QTSUBSCRIBE2_H

#include <QtCore/QObject>

class CWengoPhone;

class QString;
class QDialog;
class QWidget;
namespace Ui { class SubscribeWengo2; }

/**
 * Wengo account creation widget (2/2).
 *
 * @author Mathieu Stute
 * @author Tanguy Krotoff
 */
class QtSubscribe2 : public QObject {
	Q_OBJECT
public:

	QtSubscribe2(CWengoPhone & cWengoPhone, const QString & nickName, const QString & password, const QString & email, QWidget * parent);

	~QtSubscribe2();

private Q_SLOTS:

	void configureLogin();

	void addIMAccount();

private:

	void show();

	CWengoPhone & _cWengoPhone;

	Ui::SubscribeWengo2 * _ui;

	QDialog * _subscribeWindow;
};

#endif	//QTSUBSCRIBE2_H
