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
#include "QtEmoticonButton.h"

QtEmoticonButton::QtEmoticonButton ( QWidget * parent ) : QPushButton(parent)
{
	init();
}
QtEmoticonButton::QtEmoticonButton ( const QString & text, QWidget * parent ) : QPushButton(text,parent)
{
	init();
}
QtEmoticonButton::QtEmoticonButton ( const QIcon & icon, const QString & text, QWidget * parent ) : QPushButton(icon,text,parent)
{
	init();
}

void QtEmoticonButton::init()
{
	setFlat(true);
	connect (this,SIGNAL(clicked()),this,SLOT(buttonClickedSlot()));
}
void QtEmoticonButton::buttonClickedSlot()
{
	buttonClicked(_emoticon);
}

void QtEmoticonButton::setEmoticon(QtEmoticon emoticon)
{
	_emoticon = emoticon;
	setIcon(QIcon(emoticon.getButtonPixmap()));
	setIconSize(emoticon.getButtonPixmap().size());
}
