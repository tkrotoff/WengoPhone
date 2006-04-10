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
	_menu = NULL;

	QtUserList::getInstance()->setTreeWidget(target);
	target->setMouseTracking(true);
	UserTreeEventManager * dnd = new UserTreeEventManager(this,target);

	connect (target,SIGNAL(itemSelectionChanged ()),this,SLOT(treeViewSelectionChanged()));
	connect (target,SIGNAL(itemClicked (QTreeWidgetItem *,int )),this,SLOT(itemClicked(QTreeWidgetItem *,int)));
	connect (dnd,SIGNAL(itemEntered ( QTreeWidgetItem *)),this,SLOT(itemEntered ( QTreeWidgetItem * )));
	connect (dnd,SIGNAL(itemTimeout(QTreeWidgetItem *)),this,SLOT(openUserInfo(QTreeWidgetItem *)));
/*
	QAction * action;

    _menu = new QMenu(dynamic_cast<QWidget *>(parent));
    _callAction = _menu->addAction(tr("Call"));

    action = _menu->addAction(tr("Start Chat"));
    connect (action,SIGNAL(triggered(bool)),SLOT(startChat(bool)));

    action = _menu->addAction(tr("Send SMS"));
    connect (action,SIGNAL(triggered(bool)),SLOT(startSMS(bool)));

    _menu->addAction(tr("Invite to conference"));
    _menu->addSeparator();

    action = _menu->addAction(tr("Edit contact"));
    connect(action,SIGNAL(triggered(bool)),this,SLOT(editContact(bool)));

    _menu->addAction(tr("Delete contact"));
    _menu->addSeparator();

    _menu->addAction(tr("Block contact"));

    _menu->addAction(tr("Forward to Cell phone"));

    _menu->setWindowOpacity(0.97);
*/
}

void QtUserManager::startSMS(bool checked){

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

    // The current selected item
    QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));

	user->startSMS();
}

void QtUserManager::startChat(bool){
	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

    // The current selected item
    QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));

	user->startChat();
}

void QtUserManager::editContact(bool ){
    QtUserList * ul = QtUserList::getInstance();

    // The current selected item
    QTreeWidgetItem * item = _tree->currentItem();

	// The edit contact window
	QtEditContactProfile editContactDialog(ul->getCContact(item->text(0)));
	//editContact->set

	editContactDialog.exec();
}
void QtUserManager::treeViewSelectionChanged(){
	closeUserInfo();
}

void QtUserManager::itemEntered ( QTreeWidgetItem * item )
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
		if ( ! _menu ){
			_menu = createMenu();
			_menu->popup(QCursor::pos());
		}
		else
		{
			delete _menu;
			_menu = createMenu();
			_menu->popup(QCursor::pos());
		}
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

		if ( hidenContact->getUser()->getStatus() != QtContactPixmap::ContactOffline ){

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
				if ( user->getStatus() == QtContactPixmap::ContactOffline )
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
			// Work around Qt bug ...
			_tree->setItemExpanded ( group, false );
			_tree->setItemExpanded ( group, true );
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

QMenu * QtUserManager::createMenu(){


	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

    // The current selected item
    QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));


	QAction * action;

	QMenu * menu;

    menu = new QMenu(dynamic_cast<QWidget *>(parent()));
    // _callAction = menu->addAction(tr("Call"));
    // Call menu
    if ( user->havePhoneNumber()){

		QMenu * callMenu = menu->addMenu( tr("Call") );
		if ( ! user->getMobilePhone().isEmpty() ){
			action = callMenu->addAction(tr ("Mobile phone") );
			connect(action,SIGNAL(triggered(bool)),SLOT(startMobileCall(bool)));
		}

		if ( ! user->getHomePhone().isEmpty() ){
			action = callMenu->addAction(tr ("Home phone") );
			connect(action,SIGNAL(triggered(bool)),SLOT(startHomeCall(bool)));
		}

		if ( ! user->getWorkPhone().isEmpty() ){
			action = callMenu->addAction(tr ("Work phone") );
			connect(action,SIGNAL(triggered(bool)),SLOT(startWorkCall(bool)));
		}

		if ( ! user->getWengoPhoneNumber().isEmpty() ){
			action = callMenu->addAction(tr ("Wengophone") );
			connect(action,SIGNAL(triggered(bool)),SLOT(startWengoCall(bool)));
		}
    }


    action = menu->addAction(tr("Start Chat"));
    connect (action,SIGNAL(triggered(bool)),SLOT(startChat(bool)));

    action = menu->addAction(tr("Send SMS"));
    connect (action,SIGNAL(triggered(bool)),SLOT(startSMS(bool)));

    menu->addAction(tr("Invite to conference"));

    menu->addSeparator();

    action = menu->addAction(tr("Edit contact"));
    connect(action,SIGNAL(triggered(bool)),this,SLOT(editContact(bool)));

    menu->addAction(tr("Delete contact"));

    menu->addSeparator();

    menu->addAction(tr("Block contact"));

    menu->addAction(tr("Forward to Cell phone"));

    menu->setWindowOpacity(0.97);

    return menu;

}
void QtUserManager::startMobileCall(bool checked){

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

    // The current selected item
    QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));
	user->startCall(user->getMobilePhone());
}

void QtUserManager::startHomeCall(bool checked){
	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

    // The current selected item
    QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));
	user->startCall(user->getHomePhone());

}

void QtUserManager::startWorkCall(bool checked){

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

    // The current selected item
    QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));
	user->startCall(user->getWorkPhone());
}

void QtUserManager::startWengoCall(bool checked){

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

    // The current selected item
    QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));
	user->startCall(user->getWengoPhoneNumber());
}
