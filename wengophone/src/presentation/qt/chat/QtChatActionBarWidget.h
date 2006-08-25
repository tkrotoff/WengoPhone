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

#ifndef OWQTCHATACTIONBARWIDGET_H
#define OWQTCHATACTIONBARWIDGET_H

#include "QtWengoStyleBar.h"

class QtWengoPhone;
class QtChatWidget;
class WengoStyleLabel;
class QString;
class QWidget;

/**
 * Chat action bar widget.
 * Displays available actions from a chat session.
 *
 * TODO verify that there is nothing left elsewhere that could be brought inhere (slots, ...)
 *
 * @author Nicolas Couturier
 * @author Mathieu Stute
 */
class QtChatActionBarWidget : public QtWengoStyleBar {
	Q_OBJECT
public:

	/**
	 * Constructor.
	 */
	QtChatActionBarWidget(QtWengoPhone * qtWengoPhone, QtChatWidget * chatWidget, QWidget * parent);

public Q_SLOTS:

	void callContact();

private:

	QtWengoPhone * _qtWengoPhone;

	QtChatWidget * _chatWidget;

	WengoStyleLabel * _callLabel;

	WengoStyleLabel * _endLabel;
};

#endif	//OWQTCHATACTIONBARWIDGET_H
