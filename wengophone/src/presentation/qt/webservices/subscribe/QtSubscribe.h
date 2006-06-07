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

#ifndef QTSUBSCRIBE_H
#define QTSUBSCRIBE_H

#include <presentation/PSubscribe.h>

#include <control/webservices/subscribe/CSubscribe.h>

#include <qtutil/QObjectThreadSafe.h>

#include <string>

class CSubscribe;
class QtWengoPhone;

class QDialog;
class QWidget;
namespace Ui { class SubscribeWengo1; }

/**
 * Wengo account creation widget (1/2).
 *
 * @author Mathieu Stute
 * @author Tanguy Krotoff
 */
class QtSubscribe : public QObjectThreadSafe, public PSubscribe {
	Q_OBJECT
public:

	QtSubscribe(CSubscribe & cSubscribe);

	virtual ~QtSubscribe();

	QWidget * getWidget() const;

	void updatePresentation() { }

	void show();

private Q_SLOTS:

	void sendRequest();

private:

	void initThreadSafe();

	void updatePresentationThreadSafe() { }

	void wengoSubscriptionEventHandler(WsSubscribe & sender, int id, WsSubscribe::SubscriptionStatus status,
		const std::string & errorMessage, const std::string & password);

	void wengoSubscriptionEventHandlerThreadSafe(WsSubscribe::SubscriptionStatus status, std::string password);

	void showErrorMessage(const QString & errorMessage);

	Ui::SubscribeWengo1 * _ui;

	QDialog * _subscribeWindow;

	CSubscribe & _cSubscribe;

	QtWengoPhone * _qtWengoPhone;

	bool _subscribeButtonEnabled;

	static const char * TERMSOFSERVICE_LINK;

	static const char * TERMSOFSERVICEPDF_LINK;
};

#endif	//QTSUBSCRIBE_H
