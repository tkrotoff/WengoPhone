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
#include <presentation/PContact.h>

#include "QtUserManager.h"
#include "UserTreeEventManager.h"
#include "QtUserList.h"
#include "QtHidenContact.h"
#include "QtContactPixmap.h"
#include "../login/QtEditContactProfile.h"

QtUserManager::QtUserManager(QObject * parent, QTreeWidget * target) : QObject(parent)
{
    _tree = target;
    _previous = NULL;
    _lastClicked=NULL;
	_hideUsers = false;
	_sortUsers = false;

	QtUserList::getInstance()->setTreeWidget(target);
	target->setMouseTracking(true);
	UserTreeEventManager * dnd = new UserTreeEventManager(this,target);

	connect (target,SIGNAL(itemSelectionChanged ()),this,SLOT(treeViewSelectionChanged()));
	connect (target,SIGNAL(itemClicked (QTreeWidgetItem *,int )),this,SLOT(itemClicked(QTreeWidgetItem *,int)));
	connect (dnd,SIGNAL(itemEntered ( QTreeWidgetItem *)),this,SLOT(itemEntered ( QTreeWidgetItem * )));
	connect (dnd,SIGNAL(itemTimeout(QTreeWidgetItem *)),this,SLOT(openUserInfo(QTreeWidgetItem *)));

    _menu = new QMenu(dynamic_cast<QWidget *>(parent));
    _menu->addAction(tr("Call"));
    _menu->addAction(tr("Start Chat"));
    _menu->addAction(tr("Send SMS"));
    _menu->addAction(tr("Send a file"));
    _menu->addAction(tr("Invite to conference"));
    _menu->addSeparator();

    QAction * action = _menu->addAction(tr("Edit contact"));
    connect(action,SIGNAL(triggered(bool)),this,SLOT(editContact(bool)));

    _menu->addAction(tr("Delete contact"));
    _menu->addSeparator();

    _menu->addAction(tr("Block contact"));
    _menu->addAction(tr("Forward to Cell phone"));
    _menu->setWindowOpacity(0.97);
}
void QtUserManager::editContact(bool ){
    QtUserList * ul = QtUserList::getInstance();

    // The current selected item
    QTreeWidgetItem * item = _tree->currentItem();

	// The edit contact window
	QtEditContactProfile editContactDialog(ul->getPContact(item->text(0)));
	//editContact->set

	editContactDialog.exec();
}
void QtUserManager::treeViewSelectionChanged(){
	closeUserInfo();
}

void QtUserManager::itemEntered ( QTreeWidgetItem * item)
{
	QtUserList * ul = QtUserList::getInstance();

	if (ul)
	{
		ul->mouseOn(item->text(0));
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
		ul->setOpenStatus(_previous->text(0),false);
		_previous->setSizeHint(0,QSize(-1,ul->getHeight(_previous->text(0))));
	}
	_tree->viewport()->update();
}

void QtUserManager::openUserInfo( QTreeWidgetItem * i)
{
	QTreeWidgetItem * item = i;
	QtUserList * ul = QtUserList::getInstance();
    if ( _previous != NULL )
    {
        closeUserInfo();
        _previous = item;
        if ( item->parent() ){
			ul->setOpenStatus(_previous->text(0),true);
			item->setSizeHint(0,QSize(-1,ul->getHeight(item->text(0))));
            _tree->openPersistentEditor(item);
        }
    }
    else
    {
        _previous = item;
        if ( item->parent() ){
			ul->setOpenStatus(_previous->text(0),true);
			item->setSizeHint(0,QSize(-1,ul->getHeight(item->text(0))));
            _tree->openPersistentEditor(item);
           }
    }
    _tree->viewport()->update();
}

void QtUserManager::itemClicked ( QTreeWidgetItem * , int ){

    QtUserList * ul = QtUserList::getInstance();
    QTreeWidgetItem * item = _tree->currentItem();

    QRect widgetSize = _tree->rect();
    QPoint  mousepos = _tree->mapFromGlobal(QCursor::pos());

	if (  ! item->parent() )
	{
		if (_tree->isItemExpanded(item) )
			_tree->collapseItem ( item );
		else
			_tree->expandItem ( item );
	}

	ul->mouseClicked(item->text(0),mousepos,widgetSize);

	if (mousepos.x() > ul->getIconsStartPosition(item->text(0)) )
		return;

	if ( ul->getButton(item->text(0)) == Qt::RightButton)
	{
		_menu->popup(QCursor::pos());
		return;
	}

	if ( _previous != NULL )
	{
		closeUserInfo();
		_previous = item;

		if ( item->parent()){
			openUserInfo(item);
		}
	}
	else
	{
		_previous = item;
		if ( item->parent() ){
			openUserInfo( item);
		}
	}
	_tree->viewport()->update();
}

QList<QtHidenContact *> QtUserManager::clearList(QList<QtHidenContact *> list){

	QList<QtHidenContact *>::iterator iter;
	QList<QtHidenContact *> tmp;

	for (iter = list.begin(); iter != list.end(); iter++){

		if  ( !(*iter)->isCleared() )
			tmp.append( (*iter) );
	}
	return tmp;
}


void QtUserManager::safeUserStateChanged(){

	safeHideOffLineUsers();

	QtHidenContact * hidenContact;
	QList<QtHidenContact *>::iterator iter;

	QList<QtHidenContact *> newHidenList;

	for ( iter = _hidenContacts.begin(); iter != _hidenContacts.end(); iter++ ){

		hidenContact = (QtHidenContact *)(*iter);

		if ( hidenContact->getUser()->getStatus() != QtContactPixmap::ContactNotAvailable ){

			if ( hidenContact->getIndex() > hidenContact->getParentItem()->childCount() )
				hidenContact->getParentItem()->insertChild(hidenContact->getParentItem()->childCount(),
				                                            hidenContact->getItem());
			else
				hidenContact->getParentItem()->insertChild(hidenContact->getIndex(),hidenContact->getItem());
		}
		else{
			newHidenList.append(hidenContact);
		}
	}
	_hidenContacts = newHidenList;

	safeSortUsers();
}
void QtUserManager::userStateChanged(){
	QMutexLocker lock(&_mutex);
	safeUserStateChanged();
}

void QtUserManager::safeHideOffLineUsers(){
	//TODO: Add the code to manage hidden groups

	if (! _hideUsers)
		return;
	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;
	QList<QTreeWidgetItem *> itemList = _tree->findItems("*",Qt::MatchWildcard);
	QList<QTreeWidgetItem *> deleteList;
	QList<QTreeWidgetItem *>::iterator i;

	for ( i = itemList.begin(); i != itemList.end(); i++ ){

		QTreeWidgetItem * item;
		QTreeWidgetItem * group = (QTreeWidgetItem *) (*i);

		if ( group->parent() == 0 ){
			// We have all parents (if groups are not hiden)
			deleteList.clear();
			int count = group->childCount ();

			for ( int t = 0; t < count; t++ ){
				item = group->child(t);
				user = ul->getUser(item->text(0));
				if ( user->getStatus() == QtContactPixmap::ContactNotAvailable )
				{
					// Take the widget and put it in _hidenContacts
					int index = group->indexOfChild ( item );
					QtHidenContact * hiden = new QtHidenContact(item,item->parent(),user,index,this);
					_hidenContacts.append(hiden);
					deleteList.append(item);
				}
			}
			// Delete the childs
			QList<QTreeWidgetItem *>::iterator deleteIterator;
			for ( deleteIterator = deleteList.begin(); deleteIterator != deleteList.end(); deleteIterator++)
			{
				group->takeChild( group->indexOfChild( (*deleteIterator) ) );
			}
		}
	}
}

void QtUserManager::hideOffLineUsers(){

	QMutexLocker lock(&_mutex);
	_hideUsers = true;
	safeHideOffLineUsers();
}

void QtUserManager::safeSortUsers(){
//FIXME: only use sortList or deleteList
	if (!  _sortUsers )
		return;

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	QList<QTreeWidgetItem *> itemList = _tree->findItems("*",Qt::MatchWildcard);
	QList<QTreeWidgetItem *> deleteList;
	QList<QtHidenContact> sortList;
	QList<QTreeWidgetItem *>::iterator i;

	for ( i = itemList.begin(); i != itemList.end(); i++ ){

		QTreeWidgetItem * item;
		QTreeWidgetItem * group = (QTreeWidgetItem *) (*i);

		if ( group->parent() == 0 ){
			// We have all parents (if groups are not hiden)
			deleteList.clear();
			sortList.clear();

			int count = group->childCount ();

			for ( int t = 0; t < count; t++ ){
				item = group->child(t);
				user = ul->getUser(item->text(0));
				// Take the widget and put it in sortList
				int index = group->indexOfChild ( item );
				QtHidenContact hiden = QtHidenContact(item,item->parent(),user,index,this);
				sortList.append(hiden);
				deleteList.append(item);
			}
			// Delete the childs
			QList<QTreeWidgetItem *>::iterator deleteIterator;
			for ( deleteIterator = deleteList.begin(); deleteIterator != deleteList.end(); deleteIterator++)
			{
				group->takeChild( group->indexOfChild( (*deleteIterator) ) );
			}
			// Sort and reinsert items
			qSort(sortList.begin(),sortList.end());

			QList<QtHidenContact>::iterator insertIterator;
			for ( insertIterator = sortList.begin(); insertIterator != sortList.end(); insertIterator++)
			{
				group->insertChild(group->childCount(), (*insertIterator).getItem());
			}
		}
	}


}
void QtUserManager::sortUsers(){
	QMutexLocker lock(&_mutex);
	_sortUsers = true;
	safeSortUsers();
}
void QtUserManager::safeShowAllUsers(){
	QtHidenContact * hidenContact;
	QList<QtHidenContact *>::iterator iter;

	for ( iter = _hidenContacts.begin(); iter != _hidenContacts.end(); iter++ ){

		hidenContact = (QtHidenContact *)(*iter);

		if ( hidenContact->getIndex() > hidenContact->getParentItem()->childCount() )
			hidenContact->getParentItem()->insertChild(hidenContact->getParentItem()->childCount(),
														hidenContact->getItem());
		else
			hidenContact->getParentItem()->insertChild(hidenContact->getIndex(),hidenContact->getItem());

		delete hidenContact;
		}
	_hidenContacts.clear();
}

void QtUserManager::showAllUsers(){
	QMutexLocker lock(&_mutex);
	_hideUsers = false;
	safeShowAllUsers();
}
