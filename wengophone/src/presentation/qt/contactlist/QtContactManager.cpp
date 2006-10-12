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

#include "QtContactManager.h"

#include "QtContactList.h"
#include "QtContactPixmap.h"
#include "QtContactInfo.h"
#include "QtContactListManager.h"
#include "QtContactTreeKeyFilter.h"
#include "QtConferenceAction.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/profile/QtProfileDetails.h>

#include <control/CWengoPhone.h>
#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/config/Config.h>
#include <model/config/ConfigManager.h>
#include <model/phoneline/PhoneLine.h>
#include <model/phonecall/PhoneCall.h>
#include <model/profile/UserProfile.h>

#include <sipwrapper/EnumPhoneCallState.h>

#include <util/WebBrowser.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <qtutil/SafeConnect.h>
#include <qtutil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

QtContactManager::QtContactManager(CUserProfile & cUserProfile, CWengoPhone & cWengoPhone,
	QtContactList & qtContactList, QObject * parent, QTreeWidget * target)
	: QObject(parent),
	_cWengoPhone(cWengoPhone),
	_cUserProfile(cUserProfile),
	_qtContactList(qtContactList) {

	LANGUAGE_CHANGE(this);
	languageChanged();

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	_tree = target;
	_previous = NULL;
	_lastClicked = NULL;
	_hideUsers = config.getShowOfflineContacts();
	_sortContacts = true;
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

	QtContactListManager::getInstance()->setTreeWidget(target);
	target->setMouseTracking(true);
	QtContactTreeKeyFilter * keyFilter = new QtContactTreeKeyFilter(this, target);

	SAFE_CONNECT(target, SIGNAL(itemSelectionChanged()), SLOT(treeViewSelectionChanged()));
	SAFE_CONNECT(target, SIGNAL(itemClicked(QTreeWidgetItem *, int)), SLOT(itemClicked(QTreeWidgetItem *, int)));

	SAFE_CONNECT(keyFilter, SIGNAL(openItem(QTreeWidgetItem *)), SLOT(openUserInfo(QTreeWidgetItem *)));
	SAFE_CONNECT(keyFilter, SIGNAL(closeItem(QTreeWidgetItem *)), SLOT(closeUserInfo()));
	SAFE_CONNECT(keyFilter, SIGNAL(deleteItem(QTreeWidgetItem *)), SLOT(deleteContact()));
	SAFE_CONNECT(keyFilter, SIGNAL(enterPressed(QTreeWidgetItem *)),SLOT(defaultAction(QTreeWidgetItem *)));

	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cWengoPhone.getPresentation();

	SAFE_CONNECT_RECEIVER(this, SIGNAL(inviteToConferenceClicked(QString, PhoneCall *)),
		qtWengoPhone, SLOT(addToConference(QString, PhoneCall *)));
}

void QtContactManager::startSMS() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	QTreeWidgetItem * item = _tree->currentItem();
	if (ul && item) {
		ul->startSMS(item->text(0));
	}
}

void QtContactManager::startChat() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	QTreeWidgetItem * item = _tree->currentItem();
	if (ul && item) {
		ul->startChat(item->text(0));
	}
}

void QtContactManager::sendFile() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	QTreeWidgetItem * item = _tree->currentItem();
	if (ul && item) {
		ul->sendFile(item->text(0));
	}
}

void QtContactManager::editContact() {
	QTreeWidgetItem * item = _tree->currentItem();
	if (item) {
		editContact(item->text(0));
	}
}

void QtContactManager::editContact(QString contactId) {
	ContactProfile contactProfile = _cUserProfile.getCContactList().getContactProfile(contactId.toStdString());
	QtProfileDetails qtProfileDetails(_cUserProfile, contactProfile, _tree, tr("Edit Contact"));
	if (qtProfileDetails.show()) {
		_cUserProfile.getCContactList().updateContact(contactProfile);
	}
}

void QtContactManager::deleteContact() {
	QTreeWidgetItem * item = _tree->currentItem();
	if (!item) {
		return;
	}

	if (QMessageBox::question(_tree,
		tr("Delete contact"),
		tr("Do you really want to delete this contact?"),
		tr("&Yes"),
		tr("&No"),
		QString(),
		0, 1) == 0) {

		_cUserProfile.getCContactList().removeContact(item->text(0).toStdString());
	}
}

void QtContactManager::treeViewSelectionChanged() {
	if (_button == Qt::NoButton) {
		closeUserInfo();
	}
}

void QtContactManager::closeUserInfo() {
	if (_previous) {
		if (!_previous->parent()) {
			//It's a group
			return;
		}

		_tree->closePersistentEditor(_previous);
		QtContactListManager * ul = QtContactListManager::getInstance();
		ul->setOpenStatus(_previous->text(0), false);
		_previous->setSizeHint(0, QSize(-1, ul->getHeight(_previous->text(0))));
		_previous = NULL;

		_tree->viewport()->update();
		_tree->doItemsLayout();
	}
}

void QtContactManager::openUserInfo(QTreeWidgetItem * item) {
	if (_previous) {
		closeUserInfo();
	}

	if (!item) {
		return;
	}

	if (item->parent()) {
		QtContactListManager * ul = QtContactListManager::getInstance();
		_previous = item;
		ul->setOpenStatus(_previous->text(0), true);
		item->setSizeHint(0, QSize(-1, ul->getHeight(item->text(0))));
		_tree->openPersistentEditor(item);
		_tree->viewport()->update();
		_tree->doItemsLayout();
		_tree->scrollToItem (item);
	}
}

void QtContactManager::itemClicked(QTreeWidgetItem * item, int) {
	if ((_lastClicked == item) && (_waitForDoubleClick)) {
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
		groupClicked(item);
		return;
	}

	QtContactListManager * ul = QtContactListManager::getInstance();
	if (ul->getButton(item->text(0)) == Qt::RightButton) {
		itemRightClicked(item);
		return;
	}

	if (item->parent()) {
		if (_previous == item) {
			closeUserInfo();
		} else {
			openUserInfo(item);
		}
	}

	_tree->viewport()->update();
	_button = Qt::NoButton;
}

void QtContactManager::itemRightClicked(QTreeWidgetItem * item) {
	OWSAFE_DELETE(_menu);
	_menu = createMenu();
	_menu->popup(QCursor::pos());

	//clearSelection () is bugged ! We have to clear the selection ourself
	clearTreeSelection();
	_tree->setItemSelected(item, true);
	_button = Qt::NoButton;
}

void QtContactManager::groupClicked(QTreeWidgetItem * item) {
	if (_button == Qt::RightButton) {
		groupRightClicked(item->text(0));
		return;
	}

	if (_tree->isItemExpanded(item)) {
		_tree->collapseItem(item);
	} else {
		_tree->expandItem(item);
	}
}

void QtContactManager::clearTreeSelection() {
	QList <QTreeWidgetItem * > selectedList = _tree->selectedItems();
	QList <QTreeWidgetItem * >::iterator it;
	for (it = selectedList.begin(); it != selectedList.end(); it ++) {
		_tree->setItemSelected((*it), false);
	}
}

void QtContactManager::defaultAction(QTreeWidgetItem * item) {
	_button = Qt::NoButton;
	if (item) {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		QtContactListManager * ul = QtContactListManager::getInstance();
		QString userId = item->text(0);
		ContactProfile contactProfile = _qtContactList.getCContactList().getContactProfile(userId.toStdString());

		if (config.getGeneralClickStartChat()) {
			ul->startChat(userId);
			return;
		}

		if (config.getGeneralClickStartFreeCall()) {
			if (!contactProfile.getFirstFreePhoneNumber().empty()) {
				ul->startFreeCall(userId);
				return;
			}
		}

		if (config.getGeneralClickCallCellPhone()) {
			if (!contactProfile.getMobilePhone().empty()
				|| !contactProfile.getHomePhone().empty()) {
				ul->startCall(userId);
				return;
			}
		}

		if (!ul->getWengoPhoneNumber(userId).isEmpty()) {
			ul->startCall(userId);
		}
	}
}

void QtContactManager::safeUserStateChanged() {
	OWSAFE_DELETE(_menu);
	redrawContacts();
}

void QtContactManager::userStateChanged() {
	safeUserStateChanged();
}

void QtContactManager::hideOffLineContacts() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	_hideUsers = config.getShowOfflineContacts();
	redrawContacts();
}

void QtContactManager::hideGroups() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	_hideGroups = config.getShowGroups();
	redrawContacts();
}

void QtContactManager::sortContacts(bool bypassTimer) {
	safeSortContacts(bypassTimer);
}

void QtContactManager::safeSortContacts(bool bypassTimer) {
	if (!_sortContacts) {
		return;
	}

	if (!bypassTimer) {
		if (_canSort) {
			_canSort = false;
			if (_sortTimerId != -1) {
				killTimer(_sortTimerId);
			}
			_sortTimerId = startTimer(1000);
			_wantSort = false;
		} else {
			_wantSort = true;
			return;
		}
	}

	QList < QTreeWidgetItem * > itemList = _tree->findItems("*", Qt::MatchWildcard);
	QtContactInfoList onlineContact;
	QtContactInfoList idleContact;
	QtContactInfoList dndContact;
	QtContactInfoList offlineContact;
	QtContactInfoList othersContact;
	QList < QTreeWidgetItem * >::iterator it;

	for (it = itemList.begin(); it != itemList.end(); it++) {

		QTreeWidgetItem * item;
		QTreeWidgetItem * group = (QTreeWidgetItem *) (*it);

		if (group->parent() == 0) {
			//We have all parents (if groups are not hidden)
			onlineContact.clear();
			idleContact.clear();
			dndContact.clear();
			offlineContact.clear();
			othersContact.clear();
			int count = group->childCount();
			for (int t = 0; t < count; t++) {
				item = group->child(t);
				QString contactId = item->text(0);
				//Take the widget and put it in sortList
				int index = group->indexOfChild(item);
				QtContactInfo qtContactInfo = QtContactInfo(item, item->parent(), item->text(0), index, this);
				switch (qtContactInfo.getStatus()) {
				case QtContactPixmap::ContactOnline:
					onlineContact.append(qtContactInfo);
					break;
				case QtContactPixmap::ContactOffline:
					offlineContact.append(qtContactInfo);
					break;
				case QtContactPixmap::ContactDND:
					dndContact.append(qtContactInfo);
					break;
				case QtContactPixmap::ContactAway:
					idleContact.append(qtContactInfo);
					break;
				default:
					othersContact.append(qtContactInfo);
				}
			}

			//Sort and reinsert items
			qSort(onlineContact.begin(), onlineContact.end());
			qSort(offlineContact.begin(),offlineContact.end());
			qSort(dndContact.begin(),dndContact.end());
			qSort(idleContact.begin(),idleContact.end());
			qSort(othersContact.begin(),othersContact.end());

			QtContactInfoList::iterator insertIterator;
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
}

bool QtContactManager::canShowUser(const ContactProfile * cprofile) {
	if (_hideUsers) {
		if (cprofile->getPresenceState() == EnumPresenceState::PresenceStateOffline) {
			return false;
		}
		if (cprofile->getPresenceState() == EnumPresenceState::PresenceStateUnknown) {
			return false;
		}
	}
	return true;
}

void QtContactManager::redrawContacts() {
	if (_canShow) {
		_canShow = false;
		if (_showTimerId != -1) {
			killTimer(_showTimerId);
		}
		_showTimerId = startTimer(500);
		_wantShow = false;
	} else {
		_wantShow = true;
		return;
	}

 	ContactProfile cprofile;
	QtContactListManager * ul = QtContactListManager::getInstance();
	QTreeWidgetItem * group;
	std::vector<std::string> contactIds = _qtContactList.getCContactList().getContactIds();
	std::vector<std::string>::iterator it;
	//First, remove all item from the treeview and all users from userlist
	_tree->clear();
	ul->clear();

	for (it = contactIds.begin(); it != contactIds.end(); it++) {
		cprofile = _qtContactList.getCContactList().getContactProfile((*it));
		if (canShowUser(&cprofile)) {
			//Create new group if needed
			std::string contactGroupId = cprofile.getGroupId();
			QList < QTreeWidgetItem * > list;
			if (groupsAreHidden()) {
				list = _tree->findItems(QtContactList::DEFAULT_GROUP_NAME, Qt::MatchExactly);
			} else {
				list =_tree->findItems(QString::fromStdString(contactGroupId), Qt::MatchExactly);
			}
			//If no group exists, create the group
			if (list.isEmpty()) {
				group = new QTreeWidgetItem(_tree);
				if (groupsAreHidden()) {
					group->setText(0, QtContactList::DEFAULT_GROUP_NAME);
				} else {
					group->setText(0, QString::fromStdString(contactGroupId));
				}
				_tree->setItemExpanded(group, true);
			} else {
				group = list[0];
			}
			//We have the group, now add users to the group
			QTreeWidgetItem * newContact = NULL;
			QtContact * qtContact = NULL;
			QString contactName;
			newContact = new QTreeWidgetItem(group);
			newContact->setText(0, QString::fromStdString((*it)));
			newContact->setFlags(newContact->flags() | Qt::ItemIsEditable);
			qtContact = new QtContact((*it), _cWengoPhone, _tree);
			ul->addContact(qtContact);
		}
	}

	safeSortContacts(true);
}

QMenu * QtContactManager::createConferenceMenu() {
	PhoneLine * phoneLine = dynamic_cast <PhoneLine *>
		(_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getActivePhoneLine());
	QMenu * menu = new QMenu(_trStringInviteToConference);
	if (phoneLine) {
		PhoneLine::PhoneCallList phoneCallList = phoneLine->getPhoneCallList();
		PhoneLine::PhoneCallList::iterator it;
		for (it = phoneCallList.begin(); it != phoneCallList.end(); it++) {
			if ((*it)->getState() != EnumPhoneCallState::PhoneCallStateClosed) {
				QtConferenceAction * action = new
					QtConferenceAction(QString::fromStdString((*it)->getPeerSipAddress().getUserName()), menu);
				action->setPhoneCall((*it));
				SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(inviteToConference()));
				menu->addAction(action);
			}
		}
	}
	return menu;
}

void QtContactManager::inviteToConference() {
	QObject * source = sender();
	if (source) {
		QtConferenceAction * action = dynamic_cast < QtConferenceAction * > (source);
		QtContactListManager * ul = QtContactListManager::getInstance();
		//The current selected item
		QTreeWidgetItem * item = _tree->currentItem();
		QString contactId = item->text(0);
		if (ul->contains(contactId)) {
			QString phone = ul->getPreferredNumber(contactId);
			inviteToConferenceClicked(phone, action->getPhoneCall());
		}
	} else {
		LOG_FATAL("don't call this function directly");
	}
}

QMenu * QtContactManager::createMenu() {
	QAction * action = NULL;
	QString contactId = _tree->currentItem()->text(0);
	QMenu * menu = new QMenu(dynamic_cast <QWidget *> (parent()));
	QtContactListManager * ul = QtContactListManager::getInstance();

	//Call menu
	if (ul->hasPhoneNumber(contactId)) {
		QMenu * callMenu = menu->addMenu(QIcon(":/pics/actions/accept-phone.png"), _trStringCall);

		if (!ul->getMobilePhone(contactId).isEmpty()) {
			action = callMenu->addAction( _trStringMobilePhone);
			SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(startMobileCall()));
		}

		if (!ul->getHomePhone(contactId).isEmpty()) {
			action = callMenu->addAction(_trStringHomePhone);
			SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(startHomeCall()));
		}

		if (!ul->getWorkPhone(contactId).isEmpty()) {
			action = callMenu->addAction(_trStringWorkPhone);
			SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(startWorkCall()));
		}

		if (!ul->getWengoPhoneNumber(contactId).isEmpty()) {
			action = callMenu->addAction(_trStringWengoPhone);
			SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(startWengoCall()));
		}
	}

	action = menu->addAction(QIcon(":/pics/actions/chat.png"), _trStringStartChat);
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(startChat()));

	action = menu->addAction(QIcon(":/pics/actions/send-sms-16.png"), _trStringSendSMS);
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(startSMS()));

	action = menu->addAction(QIcon(":/pics/filetransfer/send-file.png"), _trStringSendFile);
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(sendFile()));

	//FIXME Desactivated for the moment due to a crash
	//menu->addMenu(createConferenceMenu());

	menu->addSeparator();

	QString website = ul->getWebsite(contactId);
	action = menu->addAction(QIcon(":/pics/contact/home.png"), _trStringWebsite);
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(goToWebsite()));
	if (website.isEmpty()) {
		action->setEnabled(false);
	}

	menu->addSeparator();

	action = menu->addAction(_trStringEditContact);
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(editContact()));

	action = menu->addAction(_trStringDeleteContact);
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(deleteContact()));

	//menu->addSeparator();
	//menu->addAction(_trStringBlockContact);
	//menu->addAction(_trStringForwardToCellPhone);

	return menu;
}

void QtContactManager::startMobileCall() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	QTreeWidgetItem * item = _tree->currentItem();
	if (ul && item) {
		QtContact * qtContact = ul->getContact(item->text(0));
		qtContact->startCall(qtContact->getMobilePhone());
	}
}

void QtContactManager::startHomeCall() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	QTreeWidgetItem * item = _tree->currentItem();
	if (ul && item) {
		QtContact * qtContact = ul->getContact(item->text(0));
		qtContact->startCall(qtContact->getHomePhone());
	}
}

void QtContactManager::startWorkCall() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	QTreeWidgetItem * item = _tree->currentItem();
	if (ul && item) {
		QtContact * qtContact = ul->getContact(item->text(0));
		qtContact->startCall(qtContact->getWorkPhone());
	}
}

void QtContactManager::startWengoCall() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	QTreeWidgetItem * item = _tree->currentItem();
	if (ul && item) {
		QtContact * qtContact = ul->getContact(item->text(0));
		qtContact->startCall(qtContact->getWengoPhoneNumber());
	}
}

void QtContactManager::goToWebsite() {
	QtContactListManager * ul = QtContactListManager::getInstance();
	QTreeWidgetItem * item = _tree->currentItem();
	if (ul && item) {
		QtContact * qtContact = ul->getContact(item->text(0));
		QString website = qtContact->getWebsite();
		if (!website.contains("http://")) {
			website = "http://" + website;
		}
		WebBrowser::openUrl(website.toStdString());
	}
}

void QtContactManager::removeContact(const QString & contactId) {
	QtContactListManager * ul = QtContactListManager::getInstance();
	QtContact * qtContact = NULL;
	bool found = false;
	QList <QTreeWidgetItem *> itemList = _tree->findItems("*", Qt::MatchWildcard);
	QList <QTreeWidgetItem *>::iterator it;
	for (it = itemList.begin(); ((it != itemList.end()) && !found); it++) {
		QTreeWidgetItem * item;
		QTreeWidgetItem * group = (QTreeWidgetItem *) (*it);
		if (!group->parent()) {
			int count = group->childCount();
			for (int i = 0; (i < count) && !found; i++) {
				item = group->child(i);
				qtContact = ul->getContact(item->text(0));
				if (qtContact->getId() == contactId) {
					closeUserInfo();
					item = group->takeChild(i);
					ul->removeContact(qtContact);
					OWSAFE_DELETE(item);
					found = true;
				}
			}
		}
	}
}

void QtContactManager::setMouseButton(Qt::MouseButton button) {
	_button = button;
}

void QtContactManager::moveContact(const std::string & dstGroupId,
	const std::string & srcGroupId, const std::string & contactId) {

	LOG_DEBUG("moving contact=" + contactId +
		" from=" + srcGroupId +
		" to=" + dstGroupId);

	//If groups are hidden, there is nothing to move...
	if (groupsAreHidden()) {
		return;
	}

	//Looking for the contact in the destination group
	//If it is inside this group nothing is done
	QTreeWidgetItem * group = findQtreeWidgetItem(QString::fromStdString(dstGroupId));
	if (!group) {
		return;
	}
	if (findContactInGroup(group, QString::fromStdString(contactId))) {
		return;
	}

	//Removing the Contact from the old group
	group = findQtreeWidgetItem(QString::fromStdString(srcGroupId));
	if (!group) {
		return;
	}

	int count = group->childCount();
	QtContact * qtContact = NULL;
	QtContactListManager * ul = QtContactListManager::getInstance();
	for (int i = 0; i < count; i++) {
		qtContact = ul->getContact(group->child(i)->text(0));
		if (qtContact->getId() == QString::fromStdString(contactId)) {
			group->takeChild(i);
			break;
		}
	}

	if (qtContact) {
		//Adding the user to the destination group
		group = findQtreeWidgetItem(QString::fromStdString(dstGroupId));
		//No group exists. Creating the group
		if (!group) {
			_qtContactList.contactGroupAddedEvent(dstGroupId);
			group = findQtreeWidgetItem(QString::fromStdString(dstGroupId));
		}
		QTreeWidgetItem * newContact = NULL;
		newContact = new QTreeWidgetItem(group);
		newContact->setText(0, QString::fromStdString(contactId));
		newContact->setFlags(newContact->flags() | Qt::ItemIsEditable);
	}
}

QTreeWidgetItem * QtContactManager::findQtreeWidgetItem(const QString & data) const {
	QList < QTreeWidgetItem * > list = _tree->findItems(data, Qt::MatchExactly);
	if (list.isEmpty()) {
		return NULL;
	}

	return list[0];
}

QtContact * QtContactManager::findContactInGroup(const QTreeWidgetItem * group, const QString & contactId) const {
	QtContactListManager * ul = QtContactListManager::getInstance();

	int count = group->childCount();
	for (int i = 0; i < count; i++) {
		QtContact * qtContact = ul->getContact(group->child(i)->text(0));
		if (qtContact->getId() == contactId) {
			return qtContact;
		}
	}

	return NULL;
}

void QtContactManager::timerEvent(QTimerEvent * event) {
	if (event->timerId() == _sortTimerId) {
		killTimer(_sortTimerId);
		_sortTimerId = -1;
		_canSort = true;
		if (_wantSort) {
			safeSortContacts(true);
			_wantSort = false;
		}
		return;
	}

	if (event->timerId() == _showTimerId) {
		killTimer(_showTimerId);
		_showTimerId = -1;
		_canShow = true;
		if (_wantShow) {
			redrawContacts();
			_wantShow = false;
		}
		return;
	}

	if (event->timerId() == _timerId) {
		_waitForDoubleClick = false;
		killTimer(_timerId);
		return;
	}

	QObject::timerEvent(event);
}

bool QtContactManager::groupsAreHidden() const {
	return _hideGroups;
}

void QtContactManager::languageChanged() {
	_trStringCall = tr("Call");
	_trStringMobilePhone = tr("Mobile phone");
	_trStringHomePhone = tr("Home phone");
	_trStringWorkPhone = tr("Work phone");
	_trStringWengoPhone = tr("WengoPhone");
	_trStringSendSMS = tr("Send SMS");
	_trStringWebsite = tr("Go to website");
	_trStringSendFile = tr("Send file");
	_trStringEditContact = tr("Edit contact");
	_trStringDeleteContact = tr("Delete contact");
	_trStringBlockContact = tr("Block contact");
	_trStringForwardToCellPhone = tr("Forward to mobile phone");
	_trStringStartChat = tr("Start chat");
	_trStringInviteToConference = tr("Invite to conference");
}
