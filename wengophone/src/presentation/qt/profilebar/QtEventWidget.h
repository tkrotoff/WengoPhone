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

#ifndef QTEVENTWIDGET_H
#define QTEVENTWIDGET_H

#include "ui_EventWidget.h"

#include <model/webservices/info/WsInfo.h>

#include <qtutil/QObjectThreadSafe.h>

class CUserProfile;
class CWengoPhone;
class QWidget;

/**
 * Event widget of the profile bar.
 *
 * @author Mr K
 * @author Mathieu Stute
 */
class QtEventWidget : public QObjectThreadSafe {
	Q_OBJECT
public:

	QtEventWidget (CWengoPhone & cWengoPhone, CUserProfile & cUserProfile, QWidget * parent = 0, Qt::WFlags f = 0);

	void setVoiceMail(int count);

	void setMissedCall(int count);

	void updatePresentation();

	QWidget * getWidget();


private Q_SLOTS:

	void voiceMailClicked();

	void missedCallClicked();
	
	void slotUpdatedTranslation();

private:

	void initThreadSafe();

	void updatePresentationThreadSafe();

	CUserProfile & _cUserProfile;

	CWengoPhone & _cWengoPhone;

	QWidget * _widget;

	Ui::EventWidget * _ui;

	int _voiceMailCount;

	int _missedCallCount;
};
#endif
