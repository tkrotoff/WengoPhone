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
#ifndef OWQTEMOTICONBUTTON_H
#define OWQTEMOTICONBUTTON_H

#include <QtGui/QtGui>
#include "QtEmoticon.h"

/**
 *
 * @ingroup presentation
 * @author Mr K.
 */
class QtEmoticonButton : public QPushButton
{
	Q_OBJECT

public:
	QtEmoticonButton ( QWidget * parent = 0 );

	QtEmoticonButton ( const QString & text, QWidget * parent = 0 );

	QtEmoticonButton ( const QIcon & icon, const QString & text, QWidget * parent = 0 );

	void setEmoticon(QtEmoticon emoticon);

public Q_SLOTS:

	void buttonClickedSlot();

Q_SIGNALS:

	void buttonClicked(QtEmoticon emoticon);

protected:

	void init();

	QtEmoticon _emoticon;
};

#endif //OWQTEMOTICONBUTTON_H
