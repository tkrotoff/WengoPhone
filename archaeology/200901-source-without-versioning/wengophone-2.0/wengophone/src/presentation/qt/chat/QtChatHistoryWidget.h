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

#ifndef OWQTCHATHISTORYWIDGET_H
#define OWQTCHATHISTORYWIDGET_H

#include <QtGui/QTextBrowser>

class QPoint;
class QAction;
class QWidget;

/**
 * Implements the chat text zone with a custom context menu.
 *
 * @author Nicolas Couturier
 * @author Mathieu Stute
 */
class QtChatHistoryWidget : public QTextBrowser {
	Q_OBJECT
public:

	QtChatHistoryWidget(QWidget * parent);

public Q_SLOTS:

	void saveHistoryAsHtmlSlot();

private Q_SLOTS:

	/**
	 * An URL has been clicked we must open a web browser
	 *
	 * @param link the clicked url
	 */
	void urlClicked(const QUrl & link);
};

#endif	//OWQTCHATHISTORYWIDGET_H
