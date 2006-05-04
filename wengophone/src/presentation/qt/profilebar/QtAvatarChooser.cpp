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

#include "QtAvatarChooser.h"

QtAvatarChooser::QtAvatarChooser(QWidget * parent, Qt::WFlags f) : QWidget(parent,f){
    setWindowFlags(Qt::Popup);
    loadEmoticonsList();
}

void QtAvatarChooser::init(){

    QGridLayout layout(this);

    QList <QPixmap >::iterator iter;

    for (iter = _emoticonsList.begin(); iter != _emoticonsList.end(); iter++){
        QLabel * label = new QLabel(this);
        label->setPixmap( (*iter) );
    }
}


void QtAvatarChooser::loadEmoticonsList(){
    QDir dir("pics/avatars");
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);

    QStringList nameFilter;
    nameFilter << QString("*.png");

    QStringList entryList = dir.entryList( nameFilter, QDir::Files, QDir::Name );

    QStringList::iterator iter;

    for (iter = entryList.begin(); iter != entryList.end(); iter++){
        QPixmap pixmap = QPixmap( (*iter) );
        _emoticonsList.append( pixmap );
    }
}
