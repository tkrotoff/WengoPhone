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

#include "chatwidgetmanager.h"


ChatWidgetManager::ChatWidgetManager(QObject * parent, QTextEdit * target) : QObject (parent)
{
    _parent = parent;
    _target = target;
    
    _target->installEventFilter(this);
    _target->viewport()->installEventFilter(this);
}

bool ChatWidgetManager::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress){
        if (keyPress(obj,event))
            return true;
    }
    return QObject::eventFilter(obj, event);
}

bool ChatWidgetManager::keyPress(QObject *obj, QEvent *event)
{
    QKeyEvent * e = static_cast<QKeyEvent *>(event);
    if (  (e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return) )
    {
        event->accept();
        enterPressed();
        return true;
    }
    return false;
}