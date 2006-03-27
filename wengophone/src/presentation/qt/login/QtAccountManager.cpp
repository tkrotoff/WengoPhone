#include <model/profile/UserProfile.h>
#include <model/WengoPhone.h>

#include <qtutil/WidgetFactory.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>
#include <qtutil/Object.h>

#include "QtAccountManager.h"
#include "QtProtocolSettings.h"
#include "QtImAccountItem.h"


QtAccountManager::QtAccountManager(WengoPhone & wengoPhone, QWidget * parent) : QDialog(parent),
_wengoPhone(wengoPhone) {

	_widget = WidgetFactory::create( ":/forms/login/accountManager.ui", this );
	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );


	_imAddPushButton = Object::findChild<QPushButton *>( _widget, "imAddPushButton" );
	_imDeletePushButton = Object::findChild<QPushButton *>( _widget, "imDeletePushButton" );
	_imModifyPushButton = Object::findChild<QPushButton *>( _widget, "imModifyPushButton" );
	_closePushButton = Object::findChild<QPushButton *>( _widget, "closePushButton");
	_treeWidget = Object::findChild<QTreeWidget *>( _widget, "treeWidget" );

	loadImAccounts();

	connect (_imAddPushButton, SIGNAL (clicked()), SLOT(addImAccount()));
	connect (_imDeletePushButton , SIGNAL (clicked()), SLOT(deleteImAccount()));
	connect (_imModifyPushButton, SIGNAL (clicked()), SLOT(modifyImAccount()));
	connect (_closePushButton, SIGNAL(clicked()),SLOT(accept()));


	connect ( _treeWidget, SIGNAL(itemDoubleClicked ( QTreeWidgetItem *, int )),
	          SLOT(itemDoubleClicked ( QTreeWidgetItem *, int )));
}

void QtAccountManager::itemDoubleClicked ( QTreeWidgetItem * item, int column ){

	QtImAccountItem * imItem = dynamic_cast<QtImAccountItem *>(item);

	IMAccount * account = imItem->getImAccount();

	QtProtocolSettings ps(_wengoPhone,QtProtocolSettings::MODIFY,this);
	ps.setImAccount(account);
	ps.exec();
}

void QtAccountManager::addImAccount() {

	UserProfile & userProfile = _wengoPhone.getCurrentUserProfile();

	QtProtocolSettings ps(_wengoPhone,QtProtocolSettings::ADD,this);
	if (ps.exec()){
		/*
		 ok, now the new imaccount (if any) is saved, reload all imaccount in the
		 treeWidget
		 */
		loadImAccounts();
	}
}

void QtAccountManager::deleteImAccount() {
}

void QtAccountManager::modifyImAccount() {
}

void QtAccountManager::loadImAccounts() {

	QString protocol;
	EnumIMProtocol::IMProtocol improto;
	UserProfile & userProfile = _wengoPhone.getCurrentUserProfile();

	IMAccountHandler & imh = userProfile.getIMAccountHandler();

	QStringList l;

	IMAccountHandler::iterator i;

	_treeWidget->clear();

	for (i=imh.begin(); i != imh.end(); i++) {
			IMAccount * im = &(*i);
			l << QString::fromStdString(im->getLogin());
			l << "";
			improto = im->getProtocol();

			switch (improto) {
						case EnumIMProtocol::IMProtocolMSN:
						l << "MSN";
						break;
						case EnumIMProtocol::IMProtocolYahoo:
						l << "Yahoo";
						break;
						case EnumIMProtocol::IMProtocolAIM:
						l << "AIM/ICQ";
						break;
						case EnumIMProtocol::IMProtocolJabber:
						l << "Jabber";
						break;
						// FIXME: Delete this one
						case EnumIMProtocol::IMProtocolSIPSIMPLE:
						l << "OpenWengo";
						break;
						default:
						l << "Unknow protocol";
				}

			QtImAccountItem * item = new QtImAccountItem(_treeWidget,l);
			item->setCheckState(1,Qt::Checked);
			item->setImAccount(im);
			item->setUserProfile(&userProfile);
			item->setWengoPhone(&_wengoPhone);
		}

	//getIMAccountHandler

}
