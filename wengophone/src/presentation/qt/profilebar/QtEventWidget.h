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

#include <QtGui>
#include "QtClickableLabel.h"

#include <model/webservices/info/WsInfo.h>

/**
 *
 * @author Mr K
 * @author Mathieu Stute
 */
class QtEventWidget : public QWidget {
	Q_OBJECT

public:

	typedef QList<QtClickableLabel *> QtClickableLabelList;

	QtEventWidget (QWidget * parent = 0, Qt::WFlags f = 0);

	void setNewMessages(int count);

protected:

	QtClickableLabel * _missedCallLabel;

	QtClickableLabel * _newMessagesLabel;

	QtClickableLabelList _missedCallLabelList;

	QtClickableLabelList _messagesLabelList;

	void addMissedCall(const QString & nickName);

	void addMessage(const QString & nickName);
};

#endif
