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
#include "QtUserManager.h"
#include "UserTreeEventManager.h"
#include "QtUserList.h"

QtUserManager::QtUserManager(QObject * parent, QTreeWidget * target) : QObject(parent)
{
    _tree = target;
    _previous = NULL;
    _lastClicked=NULL;
	
	target->setMouseTracking(true);
    UserTreeEventManager * dnd = new UserTreeEventManager(this,target);
    
    
    
    connect (target,SIGNAL(itemSelectionChanged ()),this,SLOT(treeViewSelectionChanged()));
    connect (target,SIGNAL(itemClicked (QTreeWidgetItem *,int )),this,SLOT(itemClicked(QTreeWidgetItem *,int)));
	connect (dnd,SIGNAL(itemEntered ( QTreeWidgetItem *)),this,SLOT(itemEntered ( QTreeWidgetItem * )));
	connect (dnd,SIGNAL(itemTimeout(QTreeWidgetItem *)),this,SLOT(openUserInfo(QTreeWidgetItem *)));
	
    _menu = new QMenu(dynamic_cast<QWidget *>(parent));
    _menu->addAction("Call");
    _menu->addAction("Start Chat");
    _menu->addAction("Send SMS");
    _menu->addAction("Send a file");
    _menu->addAction("Invite to conference");
    _menu->addSeparator();
    _menu->addAction("Edit contact");
    _menu->addAction("Delete contact");
    _menu->addSeparator();
    _menu->addAction("Block contact");
    _menu->addAction("Forward to Cell phone");
    _menu->setWindowOpacity(0.97);
}

void QtUserManager::treeViewSelectionChanged(){
    if ( _previous != NULL ){
        _tree->closePersistentEditor(_previous,0);
        _previous->setSizeHint(0,QSize(-1,MIN_ITEM_SIZE));
    }
    _tree->viewport()->update();
}

void QtUserManager::itemEntered ( QTreeWidgetItem * item)
{
	QtUserList * ul = QtUserList::getInstance();

	if (ul)
	{
		ul->mouseOn(item->text(0));
		/*
		if ( _previous != NULL )
		{
			_tree->closePersistentEditor(_previous,0);
			// _previous->setSizeHint(0,QSize(-1,MIN_ITEM_SIZE));
			_previous->setSizeHint(0,QSize(-1,QFontMetrics(_tree->font()).height()) );
			_previous = item;
			_tree->viewport()->update();
		}	
		*/
		closeUserInfo();
	}
	_tree->viewport()->update();

}

void QtUserManager::closeUserInfo()
{
	QtUserList * ul = QtUserList::getInstance();
	if ( _previous != NULL )
	{
		_tree->closePersistentEditor(_previous,0);
		_previous->setSizeHint(0,QSize(-1,QFontMetrics(_tree->font()).height()) );
		ul->setOpenStatus(_previous->text(0),false);
	}
	_tree->viewport()->update();
}

void QtUserManager::openUserInfo( QTreeWidgetItem * i)
{
	QTreeWidgetItem * item = i;
	
    if ( _previous != NULL )
    {
        
        _tree->closePersistentEditor(_previous,0);
		//_previous->setSizeHint(0,QSize(-1,MIN_ITEM_SIZE));
		_previous->setSizeHint(0,QSize(-1,QFontMetrics(_tree->font()).height()) );
        _previous = item;
        
        if ( item->childCount() == 0 ){
            item->setSizeHint(0,QSize(-1,MAX_ITEM_SIZE));
            _tree->openPersistentEditor(_previous);
        }
    }
    else
    {
        _previous = item;
        if ( item->childCount() == 0 ){
            item->setSizeHint(0,QSize(-1,MAX_ITEM_SIZE));
            _tree->openPersistentEditor(_previous);
            }
        else
            //item->setSizeHint(0,QSize(-1,MIN_ITEM_SIZE));
			_previous->setSizeHint(0,QSize(-1,QFontMetrics(_tree->font()).height()) );
    }
    _tree->viewport()->update();
}

void QtUserManager::itemClicked ( QTreeWidgetItem * , int ){
    
    QtUserList * ul = QtUserList::getInstance();
    QTreeWidgetItem * item = _tree->currentItem();

    QRect widgetSize = _tree->rect();
    QPoint  mousepos = _tree->mapFromGlobal(QCursor::pos());
    
    ul->mouseClicked(item->text(0),mousepos,widgetSize);
	
	
	
	if (mousepos.x() > ul->getIconsStartPosition(item->text(0)) )
		return;
	

	qDebug() << ul->getButton(item->text(0));
	
	if ( ul->getButton(item->text(0)) == Qt::RightButton)
	{
		_menu->popup(QCursor::pos());
		return;
	}

	if ( _previous != NULL )
	{
		
		_tree->closePersistentEditor(_previous,0);
		//_previous->setSizeHint(0,QSize(-1,MIN_ITEM_SIZE));
		_previous->setSizeHint(0,QSize(-1,QFontMetrics(_tree->font()).height()) );
		_previous = item;
		
		if ( item->childCount() == 0 ){
			item->setSizeHint(0,QSize(-1,MAX_ITEM_SIZE));
			_tree->openPersistentEditor(_previous);
		}
	}
	else
	{
		_previous = item;
		if ( item->childCount() == 0 ){
			item->setSizeHint(0,QSize(-1,MAX_ITEM_SIZE));
			_tree->openPersistentEditor(_previous);
			}
		else
			//item->setSizeHint(0,QSize(-1,MIN_ITEM_SIZE));
			_previous->setSizeHint(0,QSize(-1,QFontMetrics(_tree->font()).height()) );
	}
	_tree->viewport()->update();
	
}


