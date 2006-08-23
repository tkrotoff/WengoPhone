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

#ifndef OWQTCHATCONTACTLISTITEM_H
#define OWQTCHATCONTACTLISTITEM_H

namespace Ui { class ChatContactListItem; }

#include <QtGui/QWidget>

/**
 * Chat contact list.
 *
 * @author Mathieu Stute
 */
class QtChatContactListItem : public QWidget {
	Q_OBJECT
public:

	enum PictureMode {
		BIG,			// 96x96
		MEDIUM,			// 48x48
		SMALL,			// 24x24
		TINY,			// 12x12
	};

	enum NicknameMode {
		RIGHT,
		LEFT,
		TOP,
		BOTTOM,
		NONE,
	};

	QtChatContactListItem(QWidget * parent, QPixmap picture,
		const QString & nickname, PictureMode pmode = SMALL/*, NicknameMode nmode = RIGHT*/);

private:

	void setupPixmap(QPixmap pixmap);

	void setupNickname(const QString & nickname);

	PictureMode _pictureMode;

	//NicknameMode _nicknameMode;

	Ui::ChatContactListItem * _ui;
};

#endif	//OWQTCHATCONTACTLISTITEM_H
