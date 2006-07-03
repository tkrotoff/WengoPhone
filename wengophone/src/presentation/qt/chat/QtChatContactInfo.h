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

#ifndef OWQTCHATCONTACTINFO_H
#define OWQTCHATCONTACTINFO_H

#include <QString>

class QtChatContactInfo
{
public:

	QtChatContactInfo();

	QtChatContactInfo(const QString & bgColor, const QString & textColor,const QString & nickName);

	QtChatContactInfo(const QtChatContactInfo & other);

	virtual ~QtChatContactInfo();

	bool getIsNull() const { return _isNull;}

	QtChatContactInfo & operator=(QtChatContactInfo & other);

	QString getBgColor() const { return _bgColor; }

	QString getTextColor() const { return _textColor; }

	QString getNickName() const { return _nickName; }

	QString getHeader() const;

private:

	QString _bgColor;

	QString _textColor;

	QString _nickName;

	bool _isNull;

};

#endif //OWQTCHATCONTACTINFO_H
