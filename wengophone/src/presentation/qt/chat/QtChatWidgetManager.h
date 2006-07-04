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

#ifndef OWQTCHATWIDGETMANAGER_H
#define OWQTCHATWIDGETMANAGER_H

#include <QtGui>

/**
 * Class to manage the QTextEdit widget in the chat window
 *
 * @ingroup presentation
 * @author Mr K.
 */
class QtChatWidgetManager : public QObject
{
	Q_OBJECT

public:

	QtChatWidgetManager (QObject * parent = 0, QTextEdit * target = 0);

Q_SIGNALS:

	void enterPressed(Qt::KeyboardModifiers modifier);

	void ctrlTabPressed();

	void deletePressed();

protected:

	bool eventFilter(QObject *obj, QEvent *event);

	bool keyPress(QObject *obj, QEvent *event);

	QTextEdit * _target;

	QObject * _parent;
};

#endif //OWQTCHATWIDGETMANAGER_H
