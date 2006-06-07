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

#include "QtUserManager.h"

#include "QtContactList.h"
#include "QtContactPixmap.h"
#include "QtHidenContact.h"
#include "QtUserList.h"
#include "QtUserTreeEventFilter.h"
#include "UserTreeEventManager.h"
#include "QtConferenceAction.h"
#include "../QtWengoPhone.h"

#include <presentation/qt/profile/QtProfileDetails.h>

#include <control/CWengoPhone.h>
#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <model/contactlist/ContactList.h>
#include <model/phoneline/PhoneLine.h>
#include <model/phonecall/PhoneCall.h>
#include <model/contactlist/Contact.h>

#include <sipwrapper/EnumPhoneCallState.h>

#include <util/Logger.h>

QtUserManager::QtUserManager(CUserProfile & cUserProfile, CWengoPhone & cWengoPhone, QtContactList & qtContactList,
   QObject * parent, QTreeWidget * target)
   : QObject(parent), _cUserProfile(cUserProfile), _cWengoPhone(cWengoPhone), _qtContactList(qtContactList) {

    Config & config = ConfigManager::getInstance().getCurrentConfig();
    _tree = target;
    _previous = NULL;
    _lastClicked = NULL;
    _hideUsers = config.getShowOfflineContacts();
    _sortUsers = true;
    _hideGroups = config.getShowGroups();
    _menu = NULL;
    _timerId = -1;
    _sortTimerId = -1;
    _showTimerId = -1;
    _waitForDoubleClick = false;
    _canSort = true;
    _wantSort = false;
	_canShow = true;
	_wantShow = false;

    QtUserList::getInstance()->setTreeWidget(target);
    target->setMouseTracking(true);
    UserTreeEventManager * dnd = new UserTreeEventManager(qtContactList.getCContactList(), this, target);
    QtUserTreeEventFilter * keyFilter = new QtUserTreeEventFilter(this, target);

    connect(target, SIGNAL(itemSelectionChanged()), this, SLOT(treeViewSelectionChanged()));
    connect(target, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemClicked(QTreeWidgetItem *, int)));
    connect(dnd, SIGNAL(itemEntered(QTreeWidgetItem *)), this, SLOT(itemEntered(QTreeWidgetItem *)));
    connect(dnd, SIGNAL(itemTimeout(QTreeWidgetItem *)), this, SLOT(openUserInfo(QTreeWidgetItem *)));
    connect(dnd, SIGNAL(mouseClicked(Qt::MouseButton)), SLOT(setMouseButton(Qt::MouseButton)));

    connect(keyFilter, SIGNAL(openItem(QTreeWidgetItem *)), SLOT(openUserInfo(QTreeWidgetItem *)));
    connect(keyFilter, SIGNAL(closeItem(QTreeWidgetItem *)), SLOT(closeUserInfo()));
    connect(keyFilter, SIGNAL(deleteItem(QTreeWidgetItem *)), SLOT(deleteContact()));
    connect(keyFilter, SIGNAL(enterPressed(QTreeWidgetItem *)),SLOT(defaultAction(QTreeWidgetItem *)));

    QtWengoPhone * qtWengoPhone = dynamic_cast < QtWengoPhone * > (_cWengoPhone.getPresentation());

    if (!connect(this, SIGNAL(inviteToConferenceClicked(QString, PhoneCall *)),
    qtWengoPhone, SLOT(addToConference(QString, PhoneCall *)))) {
       LOG_DEBUG("Unable to connect signal\n");
    }

}

void QtUserManager::startSMS(bool checked) {

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();
	if( ul && item ) {
		user = ul->getUser(item->text(0));
		user->startSMS();
	}
}

void QtUserManager::startChat(bool) {

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	//The current selected item
	QTreeWidgetItem * item = _tree->currentItem();
	if( ul && item ) {
		user = ul->getUser(item->text(0));
		user->startChat();
	}
}

void QtUserManager::editContact(bool) {

	QtUserList * ul = QtUserList::getInstance();

	//The current selected item
	QTreeWidgetItem * item = _tree->currentItem();
	if (!item)
        return;
	ContactProfile contactProfile = _cUserProfile.getCContactList().getContactProfile(item->text(0).toStdString());
	QtProfileDetails qtProfileDetails(_cWengoPhone, contactProfile, _tree);
	if (qtProfileDetails.show()) {
		_cUserProfile.getCContactList().updateContact(contactProfile);
	}
	LOG_DEBUG("edit contact");
}

void QtUserManager::deleteContact() {

	QtUserList * ul = QtUserList::getInstance();
	QTreeWidgetItem * item = _tree->currentItem();
	if (!item)
        return;
	if ( QMessageBox::question(_tree,
        tr("Delete contact"),
        tr("Do you really want to delete this contact ?"),
        tr("&Yes"),
        tr("&No"),
        QString(),
        0,1) == 0){
            _cUserProfile.getCContactList().removeContact(item->text(0).toStdString());
        }
}

void QtUserManager::treeViewSelectionChanged() {

	if (_button == Qt::NoButton)
		closeUserInfo();
}

void QtUserManager::itemEntered(QTreeWidgetItem * item) {

	QtUserList * ul = QtUserList::getInstance();
	if (ul) {
		ul->mouseOn(item->text(0));
		//		closeUserInfo();

	}
	_tree->viewport()->update();
}

void QtUserManager::closeUserInfo() {

	QtUserList * ul = QtUserList::getInstance();
	if (_previous != NULL) {
		_tree->closePersistentEditor(_previous, 0);
		ul->setOpenStatus(_previous->text(0), false);
		_previous->setSizeHint(0, QSize(-1, ul->getHeight(_previous->text(0))));
		_previous = NULL;
	}
	_tree->viewport()->update();
	_tree->doItemsLayout();
}

void QtUserManager::openUserInfo(QTreeWidgetItem * i) {

	QTreeWidgetItem * item = i;
	QtUserList * ul = QtUserList::getInstance();
	if (_previous != NULL) {
		closeUserInfo();
		_previous = item;
		if (item->parent()) {
			ul->setOpenStatus(_previous->text(0), true);
			item->setSizeHint(0, QSize(-1, ul->getHeight(item->text(0))));
			_tree->openPersistentEditor(item);
		}
	}
	else {
		_previous = item;
		if (item->parent()) {
			ul->setOpenStatus(_previous->text(0), true);
			item->setSizeHint(0, QSize(-1, ul->getHeight(item->text(0))));
			_tree->openPersistentEditor(item);
		}
	}
	_tree->viewport()->update();
	_tree->doItemsLayout();
    _tree->scrollToItem ( i );
}

void QtUserManager::itemClicked(QTreeWidgetItem * item, int) {

	QtUserList * ul = QtUserList::getInstance();
    if ( (_lastClicked == item) && (_waitForDoubleClick)) {
        defaultAction(item);
        _button = Qt::NoButton;
        return;
    }


    _waitForDoubleClick = true;
    killTimer(_timerId);
    _timerId = startTimer(qApp->doubleClickInterval ());
    _lastClicked = item;

    QRect widgetSize = _tree->rect();
    QPoint mousepos = _tree->mapFromGlobal(QCursor::pos());
    if (!item->parent()) {
        if (_button == Qt::RightButton) {
            groupRightClicked(item->text(0));
            return;
        }

        if (_tree->isItemExpanded(item)) {
            _tree->collapseItem(item);
        }
        else {
            _tree->expandItem(item);
        }
    }

    ul->mouseClicked(item->text(0), mousepos, widgetSize);

    if (mousepos.x() > ul->getIconsStartPosition(item->text(0)))
        return;

    if (ul->getButton(item->text(0)) == Qt::RightButton) {
        if (!_menu) {
            _menu = createMenu();
            _menu->popup(QCursor::pos());
            // clearSelection () is bugged ! We have to clear the selection ourself
            QList <QTreeWidgetItem * > selectedList = _tree->selectedItems();
            QList <QTreeWidgetItem * >::iterator iter;
            for ( iter = selectedList.begin(); iter != selectedList.end(); iter ++){
                _tree->setItemSelected((*iter),false);
            }
            _tree->setItemSelected ( item, true );
        }
        else {
            delete _menu;
            _menu = createMenu();
            _menu->popup(QCursor::pos());
            // clearSelection () is bugged ! We have to clear the selection ourself
            QList <QTreeWidgetItem * > selectedList = _tree->selectedItems();
            QList <QTreeWidgetItem * >::iterator iter;
            for ( iter = selectedList.begin(); iter != selectedList.end(); iter ++){
                _tree->setItemSelected((*iter),false);
            }
            _tree->setItemSelected ( item, true );
        }
        _button = Qt::NoButton;
        return;
    }
/*
    if (_previous == item) {
        closeUserInfo();
    }
*/
    else if (_previous != NULL) {
        closeUserInfo();
        _previous = item;

        if (item->parent()) {
            openUserInfo(item);
        }

    }
    else {
        _previous = item;
        if (item->parent()) {
            openUserInfo(item);
        }
    }
    _tree->viewport()->update();
    _button = Qt::NoButton;
}
void QtUserManager::itemDoubleClicked(QTreeWidgetItem * item, int col) {

    defaultAction(item);
}

void QtUserManager::defaultAction(QTreeWidgetItem * item){

    QtUser * user;
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QtUserList * ul = QtUserList::getInstance();

    _button = Qt::NoButton;
	if (item) {
		QString userId = item->text(0);
		user = ul->getUser(userId);
		ContactProfile contactProfile = _qtContactList.getCContactList().getContactProfile(userId.toStdString());

		if (config.getGeneralClickStartChat()) {
			ul->startChat(userId);
			return;
		}
		if (config.getGeneralClickStartFreeCall()) {
		    QString str = QString::fromUtf8(contactProfile.getFreePhoneNumber().c_str());
		    if ( !str.isEmpty()){
                ul->startFreeCall(userId);
                return;
		    }
		}
		if (config.getGeneralClickCallCellPhone()) {
            QString str = QString::fromUtf8(contactProfile.getMobilePhone().c_str());
            if ( !str.isEmpty()){
                if (EnumPresenceState::PresenceStateOnline != contactProfile.getPresenceState()) {
                    ul->startCall(userId);
                    return;
                }
            }
		}
 		if (!user->getWengoPhoneNumber().isEmpty()) {
			ul->startCall(userId);
		}
//		ul->startChat(userId);
	}
}

QList < QtHidenContact * > QtUserManager::clearList(QList < QtHidenContact * > list) {

	QList < QtHidenContact * >::iterator iter;
	QList < QtHidenContact * > tmp;

	for (iter = list.begin(); iter != list.end(); iter++) {

		if (!(* iter)->isCleared())
			tmp.append((* iter));
	}
	return tmp;
}

void QtUserManager::safeUserStateChanged() {
	safeHideOffLineUsers();
    if (_menu){
        delete _menu;
        _menu=NULL;
    }
	safeShowAllUsers();
}

void QtUserManager::userStateChanged() {

	safeUserStateChanged();
}

void QtUserManager::safeHideOffLineUsers() {
    showAllUsers();
}

void QtUserManager::hideOffLineUsers() {
    Config & config = ConfigManager::getInstance().getCurrentConfig();
    _hideUsers = config.getShowOfflineContacts();
	safeHideOffLineUsers();
}

void QtUserManager::hideGroups(){
    Config & config = ConfigManager::getInstance().getCurrentConfig();
    _hideGroups = config.getShowGroups();
	if ( _hideGroups ){
	    safeShowAllUsers();
	}
	else{
	    safeHideGroup();
	}
}


void QtUserManager::safeHideGroup(){
    safeShowAllUsers();
}

void QtUserManager::safeSortUsers(bool bypassTimer) {

	if (!_sortUsers)
		return;

    if (!bypassTimer){
        if (_canSort){
            _canSort=false;
            if (_sortTimerId != -1)
                killTimer(_sortTimerId);
            _sortTimerId = startTimer(1000);
            _wantSort = false;
        }else{
            _wantSort = true;
            return;
        }
    }
	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	QList < QTreeWidgetItem * > itemList = _tree->findItems("*", Qt::MatchWildcard);
	QList <QtHidenContact> onlineContact;
	QList <QtHidenContact> idleContact;
	QList <QtHidenContact> dndContact;
	QList <QtHidenContact> offlineContact;
	QList <QtHidenContact> othersContact;

	QList < QTreeWidgetItem * >::iterator i;

	for (i = itemList.begin(); i != itemList.end(); i++) {

		QTreeWidgetItem * item;
		QTreeWidgetItem * group = (QTreeWidgetItem *) (* i);

		if (group->parent() == 0) {
			// We have all parents (if groups are not hiden)
            onlineContact.clear();
            idleContact.clear();
            dndContact.clear();
            offlineContact.clear();
            othersContact.clear();
			int count = group->childCount();

			for (int t = 0; t < count; t++) {
				item = group->child(t);
				user = ul->getUser(item->text(0));

				// Take the widget and put it in sortList
				int index = group->indexOfChild(item);
				QtHidenContact hiden = QtHidenContact(item, item->parent(), user, index, this);
				switch (hiden.getStatus()){
                    case QtContactPixmap::ContactOnline:
                        onlineContact.append(hiden);
                        break;
                    case QtContactPixmap::ContactOffline:
                        offlineContact.append(hiden);
                        break;
                    case QtContactPixmap::ContactDND:
                        dndContact.append(hiden);
                        break;
                    case QtContactPixmap::ContactAway:
                        idleContact.append(hiden);
                        break;
                    default:
                        othersContact.append(hiden);
				}
			}

			// Sort and reinsert items
            qSort(onlineContact.begin(), onlineContact.end());
            qSort(offlineContact.begin(),offlineContact.end());
            qSort(dndContact.begin(),dndContact.end());
            qSort(idleContact.begin(),idleContact.end());
            qSort(othersContact.begin(),othersContact.end());

			QList < QtHidenContact >::iterator insertIterator;
			for (insertIterator = onlineContact.begin(); insertIterator != onlineContact.end(); insertIterator++) {
				group->takeChild(group->indexOfChild((* insertIterator).getItem()));
				group->insertChild(group->childCount(), (* insertIterator).getItem());
			}
			for (insertIterator = idleContact.begin(); insertIterator != idleContact.end(); insertIterator++) {
				group->takeChild(group->indexOfChild((* insertIterator).getItem()));
				group->insertChild(group->childCount(), (* insertIterator).getItem());
			}
			for (insertIterator = dndContact.begin(); insertIterator != dndContact.end(); insertIterator++) {
				group->takeChild(group->indexOfChild((* insertIterator).getItem()));
				group->insertChild(group->childCount(), (* insertIterator).getItem());
			}
			for (insertIterator = offlineContact.begin(); insertIterator != offlineContact.end(); insertIterator++) {
				group->takeChild(group->indexOfChild((* insertIterator).getItem()));
				group->insertChild(group->childCount(), (* insertIterator).getItem());
			}
			for (insertIterator = othersContact.begin(); insertIterator != othersContact.end(); insertIterator++) {
				group->takeChild(group->indexOfChild((* insertIterator).getItem()));
				group->insertChild(group->childCount(), (* insertIterator).getItem());
			}
		}
	}
	QCoreApplication::processEvents();
}

void QtUserManager::sortUsers() {
	//QMutexLocker lock(&_mutex);
	_sortUsers = true;
	safeSortUsers();
}

void QtUserManager::safeShowAllUsers() {

    if (_canShow){
        _canShow=false;
        if (_showTimerId != -1)
            killTimer(_showTimerId);
        _showTimerId = startTimer(500);
        _wantShow = false;
    }else{
        _wantShow = true;
        return;
    }

    QCoreApplication::processEvents(); // Clear the events buffer

    ContactProfile cprofile;
	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;
    QTreeWidgetItem * group;
    std::vector<std::string> contactIds = _qtContactList.getCContactList().getContactIds();
    std::vector<std::string>::iterator contactIterator;

    // First, remove all item from the treeview and all users from userlist
    _tree->clear();
    ul->clear();

    for (contactIterator=contactIds.begin();contactIterator!=contactIds.end();contactIterator++){
        cprofile = _qtContactList.getCContactList().getContactProfile((*contactIterator));
        if (canShowUser(&cprofile)){
            // Create new group if needed
            std::string contactGroupId = cprofile.getGroupId();
            QList < QTreeWidgetItem * > list;
            if (groupsAreHiden()){
                list = _tree->findItems("WENGO2006CLISTHIDE",Qt::MatchExactly);
            }else{
                list =_tree->findItems(QString::fromStdString(contactGroupId), Qt::MatchExactly);
            }
            // If no group exists, create the group
            if (list.isEmpty()) {
                group = new QTreeWidgetItem(_tree);
                if (groupsAreHiden()){
                    group->setText(0, "WENGO2006CLISTHIDE");
                }else{
                    group->setText(0, QString::fromStdString(contactGroupId));
                }
                _tree->setItemExpanded(group, true);
            }else{
                group = list[0];
            }
            // We have the group, now add users to the group
            QTreeWidgetItem * newContact = NULL;
            QtUser * user = NULL;
            QString contactName;

            newContact = new QTreeWidgetItem(group);
            newContact->setText(0, QString::fromStdString((*contactIterator)));
            newContact->setFlags(newContact->flags() | Qt::ItemIsEditable);
            user = new QtUser((*contactIterator), _cWengoPhone);
            ul->addUser(user);
        }
    }
    safeSortUsers(true);
    //_tree->doItemsLayout();
}


bool QtUserManager::canShowUser(const ContactProfile * cprofile){
    if (_hideUsers){
        if (cprofile->getPresenceState() == EnumPresenceState::PresenceStateOffline )
            return false;
        if (cprofile->getPresenceState() == EnumPresenceState::PresenceStateUnknown)
            return false;
    }
    return true;
}

void QtUserManager::showAllUsers() {
	//QMutexLocker lock(&_mutex);
	safeShowAllUsers();
}

QMenu * QtUserManager::createConferenceMenu() {

	PhoneLine * phoneLine = dynamic_cast < PhoneLine * >
	   (_cWengoPhone.getCUserProfile()->getUserProfile().getActivePhoneLine());

	QMenu * menu = new QMenu(tr("Invite to conference"));
	if (phoneLine) {
		PhoneLine::PhoneCallList phoneCallList = phoneLine->getPhoneCallList();

		PhoneLine::PhoneCallList::iterator it;

		for (it = phoneCallList.begin(); it != phoneCallList.end(); it++) {

			if ((* it)->getState() != EnumPhoneCallState::PhoneCallStateClosed) {
				// QAction * action = menu->addAction(QString::fromStdString( (*it)->getPeerSipAddress().getUserName() ));
				QtConferenceAction * action = new
				   QtConferenceAction(QString::fromStdString((* it)->getPeerSipAddress().getUserName()), menu);
				action->setPhoneCall((* it));
				connect(action, SIGNAL(triggered(bool)), SLOT(inviteToConference()));
				menu->addAction(action);
			}
		}
	}
	return menu;
}

void QtUserManager::inviteToConference() { // SLOT

	QObject * source = sender();

	if (source) {
		QtConferenceAction * action = dynamic_cast < QtConferenceAction * > (source);
		QtUserList * ul = QtUserList::getInstance();
		QtUser * user;
		// The current selected item
		QTreeWidgetItem * item = _tree->currentItem();
		user = ul->getUser(item->text(0));
		if (user) {
			QString phone = user->getPreferredNumber();
			inviteToConferenceClicked(phone, action->getPhoneCall());
		}
	}
	else {
		LOG_DEBUG("Don't call this function directly !!! \n");
	}
}

QMenu * QtUserManager::createMenu() {

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));

	QAction * action;

	QMenu * menu;

	menu = new QMenu(dynamic_cast < QWidget * > (parent()));

	// Call menu
	if (user->hasPhoneNumber()) {

		QMenu * callMenu = menu->addMenu(tr("Call"));
		if (!user->getMobilePhone().isEmpty()) {
			action = callMenu->addAction(tr("Mobile phone"));
			connect(action, SIGNAL(triggered(bool)), SLOT(startMobileCall(bool)));
		}

		if (!user->getHomePhone().isEmpty()) {
			action = callMenu->addAction(tr("Home phone"));
			connect(action, SIGNAL(triggered(bool)), SLOT(startHomeCall(bool)));
		}

		if (!user->getWorkPhone().isEmpty()) {
			action = callMenu->addAction(tr("Work phone"));
			connect(action, SIGNAL(triggered(bool)), SLOT(startWorkCall(bool)));
		}

		if (!user->getWengoPhoneNumber().isEmpty()) {
			action = callMenu->addAction(tr("Wengophone"));
			connect(action, SIGNAL(triggered(bool)), SLOT(startWengoCall(bool)));
		}
	}

	action = menu->addAction(tr("Start Chat"));
	connect(action, SIGNAL(triggered(bool)), SLOT(startChat(bool)));

	action = menu->addAction(tr("Send SMS"));
	connect(action, SIGNAL(triggered(bool)), SLOT(startSMS(bool)));

	// menu->addAction(tr("Invite to conference"));
	menu->addMenu(createConferenceMenu());

	menu->addSeparator();

	action = menu->addAction(tr("Edit contact"));
	connect(action, SIGNAL(triggered(bool)), this, SLOT(editContact(bool)));

	action = menu->addAction(tr("Delete contact"));
	connect(action, SIGNAL(triggered(bool)), this, SLOT(deleteContact()));

	// _cWengoPhone.getCContactList()

	menu->addSeparator();

	menu->addAction(tr("Block contact"));

	menu->addAction(tr("Forward to Cell phone"));

	menu->setWindowOpacity(0.97);

	return menu;
}

void QtUserManager::startMobileCall(bool checked) {

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();
	if( ul && item ) {
		user = ul->getUser(item->text(0));
		user->startCall(user->getMobilePhone());
	}
}

void QtUserManager::startHomeCall(bool checked) {
	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();
	if( ul && item ) {
		user = ul->getUser(item->text(0));
		user->startCall(user->getHomePhone());
	}
}

void QtUserManager::startWorkCall(bool checked) {

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();
	if( ul && item ) {
		user = ul->getUser(item->text(0));
		user->startCall(user->getWorkPhone());
	}
}

void QtUserManager::startWengoCall(bool checked) {

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();
	if( ul && item ) {
		user = ul->getUser(item->text(0));
		user->startCall(user->getWengoPhoneNumber());
	}
}

void QtUserManager::removeContact(const QString & contactId) {

	QMutexLocker lock(& _mutex);
	safeRemoveContact(contactId);
}

void QtUserManager::safeRemoveContact(const QString & contactId) {
	QtUserList * ul = QtUserList::getInstance();
	QtUser * user = NULL;
	bool found = false;

	QList < QTreeWidgetItem * > itemList = _tree->findItems("*", Qt::MatchWildcard);
	QList < QTreeWidgetItem * >::iterator i;

	for (i = itemList.begin(); (i != itemList.end()) && !found; i++) {
		QTreeWidgetItem * item;
		QTreeWidgetItem * group = (QTreeWidgetItem *) (* i);

		if (group->parent() == 0) {
			int count = group->childCount();

			for (int t = 0; (t < count) && !found; t++) {
				item = group->child(t);
				user = ul->getUser(item->text(0));
				if (user->getId() == contactId) {
					group->takeChild(t);
					ul->removeUser(user);
					found = true;
				}
			}
		}
	}

	removeFromHidenContact(contactId);
}

void QtUserManager::removeFromHidenContact(const QString & contactId) {
	QList < QtHidenContact * >::iterator iter;

	for (iter = _hidenContacts.begin(); iter != _hidenContacts.end(); iter++) {
		if ((* iter)->getUser()->getId() == contactId) {
			_hidenContacts.erase(iter);
			break;
		}
	}
}

void QtUserManager::setMouseButton(Qt::MouseButton button) {
	_button = button;
}

void QtUserManager::moveContact(const QString & contactId,
const QString & srcContactGroupId, const QString & dstContactGroupId) {

	// If groups are hiden, there is nothing to move...
	if (groupsAreHiden())
        return;

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user = NULL;
	bool found = false;

	// Looking for the contact in the destination group
	// If it is inside this group nothing is done
	QList < QTreeWidgetItem * > list = _tree->findItems(dstContactGroupId, Qt::MatchExactly);
	if (list.isEmpty())
        return;
	QTreeWidgetItem * group = list[0];

	int count = group->childCount();

	for (int t = 0; (t < count) && !found; t++) {
		user = ul->getUser(group->child(t)->text(0));
		if (user->getId() == contactId) {
			return;
		}
	}

	// Removing the Contact from the old group
	// We should only one group
	list = _tree->findItems(srcContactGroupId, Qt::MatchExactly);
	if (list.isEmpty())
        return;
	group = list[0];
	count = group->childCount();

	for (int t = 0; (t < count) && !found; t++) {
		user = ul->getUser(group->child(t)->text(0));
		if (user->getId() == contactId) {
			group->takeChild(t);
			found = true;
		}
	}
	// If not found
	if (!found) {
		for (QList < QtHidenContact * >::iterator iter = _hidenContacts.begin(); iter != _hidenContacts.end();
		   iter++) {
			   if ((* iter)->getUser()->getId() == contactId) {
				   user = (* iter)->getUser();
				   _hidenContacts.erase(iter);
				   break;
			   }
		}
	}
	////

	// If the user has been found
	if (user) {
		// Adding the user to the destination group
		list = _tree->findItems(dstContactGroupId, Qt::MatchExactly);

		// No group exists. Creating the group
		if (list.size() == 0) {
			_qtContactList.contactGroupAddedEventSlot(dstContactGroupId);
			list = _tree->findItems(dstContactGroupId, Qt::MatchExactly);
		}

		QTreeWidgetItem * newContact = NULL;
		newContact = new QTreeWidgetItem(list[0]);
		newContact->setText(0, contactId);
		newContact->setFlags(newContact->flags() | Qt::ItemIsEditable);
		////
	}
}

void QtUserManager::timerEvent ( QTimerEvent * event ) {

    if (event->timerId() == _sortTimerId ){
        killTimer(_sortTimerId);
        _sortTimerId = -1;
        _canSort = true;
        if ( _wantSort ) {
            safeSortUsers();
            _wantSort = false;
        }
        return;
    }

    if (event->timerId() == _showTimerId){
        killTimer(_showTimerId);
        _showTimerId = -1;
        _canShow = true;
        if ( _wantShow ) {
            safeShowAllUsers();
            _wantShow = false;
        }
        return;
    }

    if (event->timerId() == _timerId){
        _waitForDoubleClick = false;
        killTimer(_timerId);
        return;
    }
}

bool QtUserManager::groupsAreHiden(){
    return _hideGroups;
}
