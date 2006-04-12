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
#include "QtChatRoomInviteDlg.h"

#include <util/Logger.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <model/contactlist/ContactList.h>

#include <model/contactlist/ContactGroup.h>

#include "../contactlist/QtContactPixmap.h"
#include "QtChatRoomTreeWidgetItem.h"
#include "QtChatRoomListWidgetItem.h"

QtChatRoomInviteDlg::QtChatRoomInviteDlg(IMChatSession & chatSession, ContactList & contactList, QWidget * parent , Qt::WFlags f ) :
QDialog(parent,f), _contactList(contactList), _chatSession(chatSession)
{
    _widget =WidgetFactory::create(":/forms/chat/ChatRoomInviteWidget.ui", this);
    QGridLayout * layout = new QGridLayout();
    layout->addWidget(_widget);
    layout->setMargin(0);
    setLayout(layout);

	setupGui();
}

void QtChatRoomInviteDlg::setupGui(){

	_contactListTreeWidget = Object::findChild<QTreeWidget *>(_widget,"contactListTreeWidget");

	_inviteListWidget = Object::findChild<QListWidget *>(_widget,"inviteListWidget");

	_addPushButton = Object::findChild<QPushButton *>(_widget,"addPushButton");

	_removePushButton = Object::findChild<QPushButton *>(_widget,"removePushButton");

	_startPushButton = Object::findChild<QPushButton *>(_widget,"startPushButton");

	connect (_addPushButton,SIGNAL(clicked()), SLOT(addToConference()));
	connect (_removePushButton,SIGNAL(clicked()),SLOT(removeFromConference()));
	connect (_startPushButton,SIGNAL(clicked()),SLOT(startConference()));

	// Remove the column header
	_contactListTreeWidget->header()->hide();

	fillContact();
}

void QtChatRoomInviteDlg::startConference(){
	// _chatSession
	QList<QListWidgetItem *>selectList =  _inviteListWidget->findItems("*",Qt::MatchWildcard);
	QList<QListWidgetItem *>::iterator iter;
	for (iter = selectList.begin(); iter != selectList.end(); iter ++ ){
		QtChatRoomListWidgetItem * item = dynamic_cast<QtChatRoomListWidgetItem *> (*iter);
		_chatSession.addIMContact(*(item->getContact().getAvailableIMContact(_chatSession)));
		_selectedContact.append( (item->getContact()) );
	}
	accept();
 }

void QtChatRoomInviteDlg::addToConference(){
	// Get list of selected item in the treeview

	QList<QTreeWidgetItem *> selectList =  _contactListTreeWidget->selectedItems ();
	QList<QTreeWidgetItem *>::iterator iter;

	for (iter = selectList.begin(); iter != selectList.end(); iter++){
		if ( (*iter)->childCount() == 0 ){
			QtChatRoomTreeWidgetItem * item = dynamic_cast<QtChatRoomTreeWidgetItem *>( (*iter) );
			_contactListTreeWidget->setItemSelected(item,false);
			if ( _inviteListWidget->findItems(item->text(0),Qt::MatchExactly).isEmpty() ){

				QtChatRoomListWidgetItem * listItem = new QtChatRoomListWidgetItem(item->getContact(), _inviteListWidget);
				listItem->setText( item->text(0) );
				listItem->setIcon( item->icon(0) );
			}
		}
	}
}

void QtChatRoomInviteDlg::removeFromConference(){
	QList<QListWidgetItem *>selectList = _inviteListWidget->selectedItems();
	QList<QListWidgetItem *>::iterator iter;

	for (iter = selectList.begin(); iter != selectList.end(); iter ++ ){
		_inviteListWidget->takeItem(_inviteListWidget->row((*iter)));
		delete (*iter);
	}
}

void QtChatRoomInviteDlg::fillContact(){
	ContactList::ContactGroupSet groupList = _contactList.getContactGroupSet();

	ContactList::ContactGroupSet::iterator iter;

	for (iter = groupList.begin(); iter != groupList.end(); iter++){
		QTreeWidgetItem * groupItem = new QTreeWidgetItem( _contactListTreeWidget);
		groupItem->setFlags(Qt::ItemIsEnabled);
		groupItem->setText(0,QString::fromUtf8((*iter).getName().c_str()) );
		fillGroup(groupItem, & (*iter));
	}
}

void QtChatRoomInviteDlg::fillGroup(QTreeWidgetItem * group, const ContactGroup * cgroup){
	int size = cgroup->size();
	QtContactPixmap::contactPixmap status;

	for (int i = 0; i < size; i++ ){
		if ( (*cgroup)[i]->getAvailableIMContact(_chatSession) != NULL ){
			QtChatRoomTreeWidgetItem * item = new QtChatRoomTreeWidgetItem (*(*cgroup)[i],group );
			item->setText(0,QString::fromStdString (  (*cgroup)[i]->getDisplayName() ));
			switch ( (*cgroup)[i]->getPresenceState()) {
				case EnumPresenceState::PresenceStateOnline:
					status = QtContactPixmap::ContactOnline;
					break;
				case EnumPresenceState::PresenceStateOffline:
					status = QtContactPixmap::ContactOffline;
					break;
				case EnumPresenceState::PresenceStateDoNotDisturb:
					status = QtContactPixmap::ContactDND;
					break;
				case EnumPresenceState::PresenceStateAway:
					status = QtContactPixmap::ContactAway;
					break;
				case EnumPresenceState::PresenceStateInvisible:
					status = QtContactPixmap::ContactInvisible;
					break;
				case EnumPresenceState::PresenceStateForward:
					status = QtContactPixmap::ContactForward;
					break;
			}
		item->setIcon(0,QIcon(QtContactPixmap::getInstance()->getPixmap(status)));
		}
	}
}

QtChatRoomInviteDlg::SelectedContact QtChatRoomInviteDlg::getSelectedContact() const{

	return _selectedContact;
}
