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

#ifndef OWQTCHATTOASTER_H
#define OWQTCHATTOASTER_H

#include "IQtToaster.h"

#include <QObject>

class QtToaster;

class QWidget;
class QString;
class QPixmap;
namespace Ui { class ChatToaster; }

/**
 * Shows a toaster when a chat is incoming.
 *
 * @author Tanguy Krotoff
 */
class QtChatToaster : public QObject, public IQtToaster {
	Q_OBJECT
public:

	QtChatToaster(QWidget * parent);

	~QtChatToaster();

	void setTitle(const QString & title);

	void setMessage(const QString & message);

	void setPixmap(const QPixmap & pixmap);

	void show();

Q_SIGNALS:

	void chatButtonClicked();

private Q_SLOTS:

	void chatButtonSlot();

private:

	void close();

	Ui::ChatToaster * _ui;

	QWidget * _chatToasterWidget;

	QtToaster * _toaster;
};

#endif	//OWQTCHATTOASTER_H