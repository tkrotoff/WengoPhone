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

#include <qtutil/WidgetFactory.h>
#include <QtGui>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>
#include <qtutil/Object.h>



#include "QtProtocolSettings.h"

QtProtocolSettings::QtProtocolSettings ( WengoPhone & wengoPhone, EditMode mode,QWidget * parent, Qt::WFlags f ) : QDialog ( parent, f ),
_wengoPhone(wengoPhone) {

	_widget = WidgetFactory::create( ":/forms/login/ProtocolSettings.ui", this );
	QGridLayout * layout = new QGridLayout();
	layout->addWidget( _widget );
	layout->setMargin( 0 );
	setLayout( layout );

	_mode = mode;
	_imAccount = NULL;
	_protocolComboBox = Object::findChild<QComboBox *>(_widget,"protocolComboBox" );

	_basicStackedWidget = Object::findChild<QStackedWidget *>(_widget,"basicStackedWidget" );
	_advancedStackedWidget = Object::findChild<QStackedWidget *>(_widget,"advancedStackedWidget" );
	_okButton = Object::findChild<QPushButton *>(_widget,"okButton" );
	_cancelButton = Object::findChild<QPushButton *>(_widget,"cancelButton" );

	setupChilds();
	//readFromConfig();
	if ( _mode != MODIFY)
		setupGui();
}

void QtProtocolSettings::setImAccount(IMAccount * imaccount) {
	if ( ! _imAccount ){
	 _imAccount = imaccount;
	 setupGui();
	}
 };



void QtProtocolSettings::setupGui() {
	/* Ok button */
	connect ( _okButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
	/* Cancel button */
	connect ( _cancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );
	/* Protocol selection combo */

	connect( _protocolComboBox, SIGNAL( currentIndexChanged ( int ) ), this, SLOT( setCurrentPageProxy ( int ) ) );

	EnumIMProtocol::IMProtocol improto;


	QString protoStr;
	if ( _mode == MODIFY ){
		improto = _imAccount->getProtocol();
		switch ( improto ){
			case EnumIMProtocol::IMProtocolMSN:
				protoStr = QString("MSN");
				setCurrentPage(ProtocolMsn);
				initMsnPage();
			break;

			case EnumIMProtocol::IMProtocolYahoo:
				protoStr = QString("Yahoo");
				setCurrentPage(ProtocolYahoo);
			break;

			case EnumIMProtocol::IMProtocolAIM:
				protoStr = QString("AIM / ICQ");
				setCurrentPage(ProtocolAim);
			break;

			case EnumIMProtocol::IMProtocolJabber:
				protoStr = QString("Jabber");
				setCurrentPage(ProtocolJabber);
			break;

			default:{
				// FIXME: What to do when protocol is unknow ?
				qDebug() << "Unknow protocol, setting to MSN";
				protoStr = QString("MSN");
				setCurrentPage(ProtocolMsn);
			}
		}

		_protocolComboBox->setCurrentIndex(_protocolComboBox->findText(protoStr));
		_protocolComboBox->setEnabled(false);
		qDebug() << "Combobox disabled";

	}
	else
		setCurrentPage(ProtocolAim);


}

void QtProtocolSettings::initMsnPage(){

	_msnAliasEdit->setText(QString::fromStdString(_imAccount->getLogin()));
	_msnPasswordEdit->setText(QString::fromStdString(_imAccount->getPassword()));

	IMAccountParameters & param = _imAccount->getIMAccountParameters();

	_msnLoginServerEdit->setText(QString::fromStdString(param.getMSNServer()));
	_msnPortEdit->setText(QString("%1").arg(param.getMSNServerPort()));

	if (param.isMSNHttpUsed())
		_msnUseHttpCheckBox->setCheckState(Qt::Checked);
	else
		_msnUseHttpCheckBox->setCheckState(Qt::Unchecked);

	// Remember password ?
	if (param.isPasswordRemembered())
		_msnRememberPasswordCheckBox->setCheckState(Qt::Checked);
	else
		_msnRememberPasswordCheckBox->setCheckState(Qt::Unchecked);

	// New mail notification ?
	if (param.isMailNotified())
		_msnNewMailNotificationCheckBox->setCheckState(Qt::Checked);
	else
		_msnNewMailNotificationCheckBox->setCheckState(Qt::Unchecked);

}

void QtProtocolSettings::initAimPage(){

	IMAccountParameters & param = _imAccount->getIMAccountParameters();

	_aimAliasEdit->setText(QString::fromStdString(_imAccount->getLogin()));
	_aimPasswordEdit->setText(QString::fromStdString(_imAccount->getPassword()));

	_aimAuthHostEdit->setText(QString::fromStdString(param.getOscarServer()));
	_aimAuthEdit->setText(QString("%1").arg(param.getOscarServerPort()));
	_aimEncodingEdit->setText(QString::fromStdString(param.getOscarEncoding()));

	// Remember password ?
	if (param.isPasswordRemembered())
		_aimRememberPasswordCheckBox->setCheckState(Qt::Checked);
	else
		_aimRememberPasswordCheckBox->setCheckState(Qt::Unchecked);

	// New mail notification ?
	if (param.isMailNotified())
		_aimNewMailNotificationCheckBox->setCheckState(Qt::Checked);
	else
		_aimNewMailNotificationCheckBox->setCheckState(Qt::Unchecked);
}

void QtProtocolSettings::initYahooPage(){

	IMAccountParameters & param = _imAccount->getIMAccountParameters();

	_yahooScreenNameEdit->setText(QString::fromStdString(_imAccount->getLogin()));
	_yahooPasswordEdit->setText(QString::fromStdString(_imAccount->getPassword()));


	// Is yahoo japan ?
	if ( param.isYahooJapan() )
		_yahooJapanCheckBox->setCheckState(Qt::Checked);
	else
		_yahooJapanCheckBox->setCheckState(Qt::Unchecked);

	_yahooPagerHostEdit->setText(QString::fromStdString(param.getYahooServer()));
	_yahooPagerPortEdit->setText(QString("%1").arg(param.getYahooServerPort()));

	_yahooFileTransferHostEdit->setText(QString::fromStdString(param.getYahooXferHost()));
	_yahooFileTransferPortEdit->setText(QString("%1").arg(param.getYahooXferPort()));

	_yahooJapanFileTransferHostEdit->setText(QString::fromStdString(param.getYahooJapanXferHost()));

	_yahooChatRoomLocalEdit->setText(QString::fromStdString(param.getYahooRoomListLocale()));

	// Remember password ?
	if (param.isPasswordRemembered())
		_yahooRememberPasswordCheckBox->setCheckState(Qt::Checked);
	else
		_yahooRememberPasswordCheckBox->setCheckState(Qt::Unchecked);

	// New mail notification ?
	if (param.isMailNotified())
		_yahooNewMailNotificationCheckBox->setCheckState(Qt::Checked);
	else
		_yahooNewMailNotificationCheckBox->setCheckState(Qt::Unchecked);

}


void QtProtocolSettings::accept() {
	/* Default dialog action */
	QDialog::accept();
}

void QtProtocolSettings::saveConfig(){


}

void QtProtocolSettings::setCurrentPageProxy(int index){
	qDebug() << "Combobox index changed";
	setCurrentPage( (AvailableProtocols) index);
}


void QtProtocolSettings::setCurrentPage(AvailableProtocols index){

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QWidget * widget;

	StringList defaultProtocols = config.getAvailableProtocols();

	QString selectedProtocol = QString::fromStdString( defaultProtocols[(int)index] );

	qDebug() << "Selected protocol : " << selectedProtocol;
	qDebug() << "Selected protocol : " << (int) index;

	if ( selectedProtocol == "AIM / ICQ" ){
		widget = Object::findChild<QWidget *>(_basicStackedWidget ,"aimPage" );
		_basicStackedWidget->setCurrentWidget(widget);
		widget = Object::findChild<QWidget *>(_advancedStackedWidget, "aimAdvancedSettings");
		_advancedStackedWidget->setCurrentWidget(widget);
	}

	if ( selectedProtocol == "MSN" ){
		widget = Object::findChild<QWidget *>(_basicStackedWidget ,"msnPage" );
		_basicStackedWidget->setCurrentWidget(widget);
		widget = Object::findChild<QWidget *>(_advancedStackedWidget, "msnAdvancedSettings");
		_advancedStackedWidget->setCurrentWidget(widget);
	}
	if ( selectedProtocol == "Jabber" ){
		widget = Object::findChild<QWidget *>(_basicStackedWidget ,"jabberPage" );
		_basicStackedWidget->setCurrentWidget(widget);
		widget = Object::findChild<QWidget *>(_advancedStackedWidget, "jabberAdvancedSettings");
		_advancedStackedWidget->setCurrentWidget(widget);
	}
	if ( selectedProtocol == "Yahoo" ){
		widget = Object::findChild<QWidget *>(_basicStackedWidget ,"yahooPage" );
		_basicStackedWidget->setCurrentWidget(widget);
		widget = Object::findChild<QWidget *>(_advancedStackedWidget, "yahooAdvancedSettings");
		_advancedStackedWidget->setCurrentWidget(widget);
	}
}

void QtProtocolSettings::readFromConfig() {


}

void QtProtocolSettings::setupChilds() {

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	StringList defaultProtocols = config.getAvailableProtocols();

	QStringList qlist;

	/* StringList to QStringList */

	for (unsigned int i = 0; i < defaultProtocols.size(); i++){

		qlist << QString::fromStdString(defaultProtocols[i]);
	}
	_protocolComboBox->clear();
	_protocolComboBox->addItems(qlist);

	// AIM /ICQ Options

	// _aimScreenNameEdit = _widget->findChild<QLineEdit *>( "aimScreenNameEdit" );
	_aimScreenNameEdit = Object::findChild<QLineEdit *>(_widget,"aimScreenNameEdit" );

	_aimPasswordEdit = Object::findChild<QLineEdit *>(_widget,"aimPasswordEdit" );

	_aimAliasEdit = Object::findChild<QLineEdit *>(_widget,"aimAliasEdit" );

	_aimRememberPasswordCheckBox = Object::findChild<QCheckBox *>(_widget,"aimRememberPasswordCheckBox" );

	_aimNewMailNotificationCheckBox = Object::findChild<QCheckBox *>(_widget,"aimNewMailNotificationCheckBox" );

	_aimAuthHostEdit = Object::findChild<QLineEdit *>(_widget,"aimAuthHostEdit" );

	_aimAuthEdit = Object::findChild<QLineEdit *>(_widget,"aimAuthEdit" );

	_aimEncodingEdit = Object::findChild<QLineEdit *>(_widget,"aimEncodingEdit" );

	_aimProxyCombo = Object::findChild<QComboBox *>(_widget,"aimProxyCombo" );

	// Gadu Gadu Options

	_gaduScreenNameEdit = Object::findChild<QLineEdit *>(_widget,"gaduScreenNameEdit" );

	_gaduPasswordEdit = Object::findChild<QLineEdit *>(_widget,"gaduPasswordEdit" );

	_gaduAliasEdit = Object::findChild<QLineEdit *>(_widget,"gaduAliasEdit" );

	_gaduRememberPasswordCheckBox = Object::findChild<QCheckBox *>(_widget,"gaduRememberPasswordCheckBox" );

	_gaduNicknameEdit = Object::findChild<QLineEdit *>(_widget,"gaduNicknameEdit" );

	_gaduProxyComboBox = Object::findChild<QComboBox *>(_widget,"gaduProxyComboBox" );

	// GroupeWise Options

	_groupWiseScreenNameEdit = Object::findChild<QLineEdit *>(_widget,"groupWiseScreenNameEdit" );

	_groupWisePasswordEdit = Object::findChild<QLineEdit *>(_widget,"groupWisePasswordEdit" );

	_groupWiseAliasEdit = Object::findChild<QLineEdit *>(_widget,"groupWiseAliasEdit" );

	_groupWiseRememberPasswordCheckBox = Object::findChild<QCheckBox *>(_widget,"groupWiseRememberPasswordCheckBox" );

	_groupWiseServerAdressEdit = Object::findChild<QLineEdit *>(_widget,"groupWiseServerAdressEdit" );

	_groupWiseServerPortEdit = Object::findChild<QLineEdit *>(_widget,"groupWiseServerPortEdit" );

	_groupWiseProxyComboBox = Object::findChild<QComboBox *>(_widget,"groupWiseProxyComboBox" );

	// Jabber Options

	_jabberSreenNameEdit = Object::findChild<QLineEdit *>(_widget,"jabberSreenNameEdit" );

	_jabberServerEdit = Object::findChild<QLineEdit *>(_widget,"jabberServerEdit" );

	_jabberResourceEdit = Object::findChild<QLineEdit *>(_widget,"jabberResourceEdit" );

	_jabberPasswordEdit = Object::findChild<QLineEdit *>(_widget,"jabberPasswordEdit" );

	_jabberAliasEdit = Object::findChild<QLineEdit *>(_widget,"jabberAliasEdit" );

	_jabberRememberPasswordCheckBox = Object::findChild<QCheckBox *>(_widget,"jabberRememberPasswordCheckBox" );

	_jabberOpenPushButton = Object::findChild<QPushButton *>(_widget,"jabberOpenPushButton" );

	_jabberRemovePushButton = Object::findChild<QPushButton *>(_widget,"jabberRemovePushButton" );

	_jabberUseTLSCheckBox = Object::findChild<QCheckBox *>(_widget,"jabberUseTLSCheckBox" );

	_jabberRequireTLSCheckBox = Object::findChild<QCheckBox *>(_widget,"jabberRequireTLSCheckBox" );

	_jabberForceOldSSLCheckBox = Object::findChild<QCheckBox *>(_widget,"jabberRequireTLSCheckBox" );

	_jabberAllowPlainTextAuthCheckBox = Object::findChild<QCheckBox *>(_widget,"jabberAllowPlainTextAuthCheckBox" );

	_jabberConnectPortEdit = Object::findChild<QLineEdit *>(_widget,"jabberConnectPortEdit" );

	_jabberConnectServerEdit = Object::findChild<QLineEdit *>(_widget,"jabberConnectServerEdit" );

	_jabberProxyComboBox = Object::findChild<QComboBox *>(_widget,"jabberProxyComboBox" );

	// MSN Options

	_msnScreenNameEdit = Object::findChild<QLineEdit *>(_widget,"msnScreenNameEdit" );

	_msnPasswordEdit = Object::findChild<QLineEdit *>(_widget,"msnPasswordEdit" );

	_msnAliasEdit = Object::findChild<QLineEdit *>(_widget,"msnAliasEdit" );

	_msnRememberPasswordCheckBox = Object::findChild<QCheckBox *>(_widget,"msnRememberPasswordCheckBox" );

	_msnNewMailNotificationCheckBox = Object::findChild<QCheckBox *>(_widget,"msnNewMailNotificationCheckBox" );

	_msnOpenPushButton = Object::findChild<QPushButton *>(_widget,"msnOpenPushButton" );

	_msnRemovePushButton = Object::findChild<QPushButton *>(_widget,"msnRemovePushButton" );

	_msnLoginServerEdit = Object::findChild<QLineEdit *>(_widget,"msnLoginServerEdit" );

	_msnPortEdit = Object::findChild<QLineEdit *>(_widget,"msnPortEdit" );

	_msnUseHttpCheckBox = Object::findChild<QCheckBox *>(_widget,"msnUseHttpCheckBox" );

	_msnShowCustomSmileysCheckBox = Object::findChild<QCheckBox *>(_widget,"msnShowCustomSmileysCheckBox" );

	_msnProxyComboBox = Object::findChild<QComboBox *>(_widget,"msnProxyComboBox" );

	// Napster Options

	_napsterScreenNameEdit = Object::findChild<QLineEdit *>(_widget,"napsterScreenNameEdit" );

	_napsterPasswordEdit = Object::findChild<QLineEdit *>(_widget,"napsterPasswordEdit" );

	_napsterAliasEdit = Object::findChild<QLineEdit *>(_widget,"napsterAliasEdit" );

	_napsterRememberPasswordCheckBox = Object::findChild<QCheckBox *>(_widget,"napsterRememberPasswordCheckBox" );

	_napsterServerEdit = Object::findChild<QLineEdit *>(_widget,"napsterServerEdit" );

	_napsterPortEdit = Object::findChild<QLineEdit *>(_widget,"napsterPortEdit" );

	_napsterProxyComboBox = Object::findChild<QComboBox *>(_widget,"napsterProxyComboBox" );

	// SILC Options

	_silcSreenNameEdit = Object::findChild<QLineEdit *>(_widget,"silcSreenNameEdit" );

	_silcNetworkEdit = Object::findChild<QLineEdit *>(_widget,"silcNetworkEdit" );

	_silcPasswordEdit = Object::findChild<QLineEdit *>(_widget,"silcPasswordEdit" );

	_silcAliasEdit = Object::findChild<QLineEdit *>(_widget,"silcAliasEdit" );

	_silcRememberPasswordCheckBox = Object::findChild<QCheckBox *>(_widget,"silcRememberPasswordCheckBox" );

	_silcOpenPushButton = Object::findChild<QPushButton *>(_widget,"silcOpenPushButton" );

	_silcRemovePushButton = Object::findChild<QPushButton *>(_widget,"silcRemovePushButton" );

	_silcConnectServerEdit = Object::findChild<QLineEdit *>(_widget,"silcConnectServerEdit" );

	_silcPortEdit = Object::findChild<QLineEdit *>(_widget,"silcPortEdit" );

	_silcPublicKeyFileEdit = Object::findChild<QLineEdit *>(_widget,"silcPublicKeyFileEdit" );

	_silcPrivateKeyFileEdit = Object::findChild<QLineEdit *>(_widget,"silcPrivateKeyFileEdit" );

	_silcCipherComboBox = Object::findChild<QComboBox *>(_widget,"silcCipherComboBox" );

	_silcHMACComboBox = Object::findChild<QComboBox *>(_widget,"silcHMACComboBox" );

	_silcPublicKeyAuthCheckBox = Object::findChild<QCheckBox *>(_widget,"silcPublicKeyAuthCheckBox" );

	_silcRejectWatchingCheckBox = Object::findChild<QCheckBox *>(_widget,"silcRejectWatchingCheckBox" );

	_silcBlockInvitesCheckBox = Object::findChild<QCheckBox *>(_widget,"silcBlockInvitesCheckBox" );

	_silcBlocksIMCheckBox = Object::findChild<QCheckBox *>(_widget,"silcBlocksIMCheckBox" );

	_silcRejectOnlineStatusCheckBox = Object::findChild<QCheckBox *>(_widget,"silcRejectOnlineStatusCheckBox" );

	_silcBlockMessageCheckBox = Object::findChild<QCheckBox *>(_widget,"silcBlockMessageCheckBox" );

	_silcAutomaticallyOpenWhiteboardCheckBox = Object::findChild<QCheckBox *>(_widget,"silcAutomaticallyOpenWhiteboardCheckBox" );

	_silcDigitallySignCheckBox = Object::findChild<QCheckBox *>(_widget,"silcDigitallySignCheckBox" );

	_silcProxyComboBox = Object::findChild<QComboBox *>(_widget,"silcProxyComboBox" );

	// SIMPLE Options

	_simpleScreenNameEdit = Object::findChild<QLineEdit *>(_widget,"simpleScreenNameEdit" );

	_simpleServerEdit = Object::findChild<QLineEdit *>(_widget,"simpleServerEdit" );

	_simplePasswordEdit = Object::findChild<QLineEdit *>(_widget,"simplePasswordEdit" );

	_simpleAliasEdit = Object::findChild<QLineEdit *>(_widget,"simpleAliasEdit" );

	_simpleRememberPasswordCheckBox = Object::findChild<QCheckBox *>(_widget,"simpleRememberPasswordCheckBox" );

	_simplePublishStatusCheckBox = Object::findChild<QCheckBox *>(_widget,"simplePublishStatusCheckBox" );

	_simpleConnectPortEdit = Object::findChild<QLineEdit *>(_widget,"simpleConnectPortEdit" );

	_simpleUDPCheckBox = Object::findChild<QCheckBox *>(_widget,"simpleUDPCheckBox" );

	_simpleProxyCheckBox = Object::findChild<QCheckBox *>(_widget,"simpleProxyCheckBox" );

	_simpleProxyEdit = Object::findChild<QLineEdit *>(_widget,"simpleProxyEdit" );

	_simpleProxyComboBox = Object::findChild<QComboBox *>(_widget,"simpleProxyComboBox" );

	// SIMETIME

	_sametimeScreenNameEdit = Object::findChild<QLineEdit *>(_widget,"sametimeScreenNameEdit" );

	_sametimePasswordEdit = Object::findChild<QLineEdit *>(_widget,"sametimePasswordEdit" );

	_sametimeAliasEdit = Object::findChild<QLineEdit *>(_widget,"sametimeAliasEdit" );

	_simeTimeRememberPasswordCheckBox = Object::findChild<QCheckBox *>(_widget,"simeTimeRememberPasswordCheckBox" );

	_sametimeServerEdit = Object::findChild<QLineEdit *>(_widget,"sametimeServerEdit" );

	_sametimePortEdit = Object::findChild<QLineEdit *>(_widget,"sametimePortEdit" );

	_sametimeForceLoginCheckBox = Object::findChild<QCheckBox *>(_widget,"sametimeForceLoginCheckBox" );

	_sametimeHideClientCheckBox = Object::findChild<QCheckBox *>(_widget,"sametimeHideClientCheckBox" );

	_sametimeProxyComboBox = Object::findChild<QComboBox *>(_widget,"sametimeProxyComboBox" );

	// Yahoo

	_yahooScreenNameEdit = Object::findChild<QLineEdit *>(_widget,"yahooScreenNameEdit" );

	_yahooPasswordEdit = Object::findChild<QLineEdit *>(_widget,"yahooPasswordEdit" );

	_yahooAliasEdit = Object::findChild<QLineEdit *>(_widget,"yahooAliasEdit" );

	_yahooRememberPasswordCheckBox = Object::findChild<QCheckBox *>(_widget,"yahooRememberPasswordCheckBox" );

	_yahooNewMailNotificationCheckBox = Object::findChild<QCheckBox *>(_widget,"yahooNewMailNotificationCheckBox" );

	_yahooOpenPushButton = Object::findChild<QPushButton *>(_widget,"yahooOpenPushButton" );

	_yahooRemovePushButton = Object::findChild<QPushButton *>(_widget,"yahooRemovePushButton" );

	_yahooJapanCheckBox = Object::findChild<QCheckBox *>(_widget,"yahooJapanCheckBox" );

	_yahooPagerHostEdit = Object::findChild<QLineEdit *>(_widget,"yahooPagerHostEdit" );

	_yahooJapanPagerHostEdit = Object::findChild<QLineEdit *>(_widget,"yahooJapanPagerHostEdit" );

	_yahooPagerPortEdit = Object::findChild<QLineEdit *>(_widget,"yahooPagerPortEdit" );

	_yahooFileTransferHostEdit = Object::findChild<QLineEdit *>(_widget,"yahooFileTransferHostEdit" );

	_yahooJapanFileTransferHostEdit = Object::findChild<QLineEdit *>(_widget,"yahooJapanFileTransferHostEdit" );

	_yahooFileTransferPortEdit = Object::findChild<QLineEdit *>(_widget,"yahooFileTransferPortEdit" );

	_yahooChatRoomLocalEdit = Object::findChild<QLineEdit *>(_widget,"yahooChatRoomLocalEdit" );

	_yahooIgnoreConferenceCheckBox = Object::findChild<QCheckBox *>(_widget,"yahooIgnoreConferenceCheckBox" );

	_yahooProxyComboBox = Object::findChild<QComboBox *>(_widget,"yahooProxyComboBox" );

}

