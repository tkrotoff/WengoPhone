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

#include "QtUserWidgetAvatarManager.h"

#include "QtUserWidget.h"

#include <control/contactlist/CContact.h>

#include <util/Picture.h>

QtUserWidgetAvatarManager::QtUserWidgetAvatarManager(QtUserWidget * userWidget, QFrame * target) 
: QObject(userWidget), _userWidget(userWidget)
{
	_target = target;
	_isSet = false;
}

void QtUserWidgetAvatarManager::setAvatar(QByteArray svg){
	_svg = svg;
	_svgRenderer.load(_svg);
	_isSet = true;
}

QByteArray QtUserWidgetAvatarManager::getSvg()
{
	return _svg;
}

bool QtUserWidgetAvatarManager::eventFilter(QObject *obj, QEvent *event)
{
	switch (event->type())
    {
        case QEvent::Paint:
            paintEvent(dynamic_cast<QPaintEvent *>(event));
            return true;
        default:
            return QObject::eventFilter(obj, event);
    }
}

void QtUserWidgetAvatarManager::paintEvent(QPaintEvent * event)
{
	/*
	if (_isSet)
	{
		// Get a painter
		QPainter painter(_target);
		_svgRenderer.render(&painter);
	}
	*/
	QPainter painter(_target);

	Picture picture = _userWidget->getCContact().getIcon();

	QPixmap pixmap, scaledPixmap;
	std::string data = picture.getData();
	pixmap.loadFromData((uchar *)data.c_str(), data.size());

	QRect rect = _userWidget->getAvatarLabel()->rect();
	scaledPixmap = pixmap.scaled(rect.size());

	painter.drawPixmap(rect, scaledPixmap, rect);
}

void QtUserWidgetAvatarManager::loadAvatar(QString path)
{
	QFile file(path);
	QByteArray tmp;
	if (file.open(QIODevice::ReadOnly))
	{
		tmp = file.readAll ();
		if ( !tmp.isEmpty() )
		{
			setAvatar(tmp);
		}
		file.close();
	}
}
