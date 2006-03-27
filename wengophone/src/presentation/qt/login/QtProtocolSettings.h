#ifndef QTPROTOCOLSETTINGS_H
#define QTPROTOCOLSETTINGS_H

#include <model/profile/UserProfile.h>
#include <model/WengoPhone.h>
#include <util/Logger.h>

#include <QtGui>

class QtProtocolSettings : public QDialog
{
    Q_OBJECT

public:
    enum ProtocolPage {AIM,GADU,GROUPWISE,JABBER,MSN,NAPSTER,SILC,SIMPLE,SAMETIME,YAHOO};

    enum EditMode{ADD,MODIFY};

    enum AvailableProtocols { ProtocolMsn=0, ProtocolAim , ProtocolJabber, ProtocolYahoo };

    QtProtocolSettings (WengoPhone & wengoPhone, EditMode mode, QWidget * parent=0, Qt::WFlags f=0);

    public Q_SLOTS:

    virtual void accept();

    void setImAccount(IMAccount * imaccount);

protected:

    void setupGui();

    void setupChilds();

    void readFromConfig();

    void saveConfig();

	void initMsnPage();

	void initAimPage();

	void initYahooPage();

	EditMode _mode;

	IMAccount * _imAccount;

	WengoPhone & _wengoPhone;

    QWidget *   _widget;

    QComboBox  * _protocolComboBox;

    QStackedWidget * _basicStackedWidget;

    QStackedWidget * _advancedStackedWidget;

    QPushButton * _okButton;

    QPushButton * _cancelButton;

    QMap<QString,QVariant> _config;

	// AIM /ICQ Options

	QLineEdit * _aimScreenNameEdit;

	QLineEdit * _aimPasswordEdit;

	QLineEdit * _aimAliasEdit;

	QCheckBox * _aimRememberPasswordCheckBox;

	QCheckBox * _aimNewMailNotificationCheckBox;

	QLineEdit * _aimAuthHostEdit;

	QLineEdit * _aimAuthEdit;

	QLineEdit * _aimEncodingEdit;

	QComboBox * _aimProxyCombo;

	// Gadu Gadu Options (not supported)

	QLineEdit * _gaduScreenNameEdit;

	QLineEdit * _gaduPasswordEdit;

	QLineEdit * _gaduAliasEdit;

	QCheckBox * _gaduRememberPasswordCheckBox;

	QLineEdit * _gaduNicknameEdit;

	QComboBox * _gaduProxyComboBox;

	// GroupeWise Options (not supported)

	QLineEdit * _groupWiseScreenNameEdit;

	QLineEdit * _groupWisePasswordEdit;

	QLineEdit * _groupWiseAliasEdit;

	QCheckBox * _groupWiseRememberPasswordCheckBox;

	QLineEdit * _groupWiseServerAdressEdit;

	QLineEdit * _groupWiseServerPortEdit;

	QComboBox * _groupWiseProxyComboBox;

	// Jabber Options

	QLineEdit * _jabberSreenNameEdit;

	QLineEdit * _jabberServerEdit;

	QLineEdit * _jabberResourceEdit;

	QLineEdit * _jabberPasswordEdit;

	QLineEdit * _jabberAliasEdit;

	QCheckBox * _jabberRememberPasswordCheckBox;

	QPushButton * _jabberOpenPushButton;

	QPushButton * _jabberRemovePushButton;

	QCheckBox * _jabberUseTLSCheckBox;

	QCheckBox * _jabberRequireTLSCheckBox;

	QCheckBox * _jabberForceOldSSLCheckBox;

	QCheckBox * _jabberAllowPlainTextAuthCheckBox;

	QLineEdit * _jabberConnectPortEdit;

	QLineEdit * _jabberConnectServerEdit;

	QComboBox * _jabberProxyComboBox;

	// MSN Options

	QLineEdit * _msnScreenNameEdit;

	QLineEdit * _msnPasswordEdit;

	QLineEdit * _msnAliasEdit;

	QCheckBox * _msnRememberPasswordCheckBox;

	QCheckBox * _msnNewMailNotificationCheckBox;

	QPushButton * _msnOpenPushButton;

	QPushButton * _msnRemovePushButton;

	QLineEdit * _msnLoginServerEdit;

	QLineEdit * _msnPortEdit;

	QCheckBox * _msnUseHttpCheckBox;

	QCheckBox * _msnShowCustomSmileysCheckBox;

	QComboBox * _msnProxyComboBox;

	// Napster Options (not supported)

	QLineEdit * _napsterScreenNameEdit;

	QLineEdit * _napsterPasswordEdit;

	QLineEdit * _napsterAliasEdit;

	QCheckBox * _napsterRememberPasswordCheckBox;

	QLineEdit * _napsterServerEdit;

	QLineEdit * _napsterPortEdit;

	QComboBox * _napsterProxyComboBox;

	// SILC Options (not supported)

	QLineEdit * _silcSreenNameEdit;

	QLineEdit * _silcNetworkEdit;

	QLineEdit * _silcPasswordEdit;

	QLineEdit * _silcAliasEdit;

	QCheckBox * _silcRememberPasswordCheckBox;

	QPushButton * _silcOpenPushButton;

	QPushButton * _silcRemovePushButton;

	QLineEdit * _silcConnectServerEdit;

	QLineEdit * _silcPortEdit;

	QLineEdit * _silcPublicKeyFileEdit;

	QLineEdit * _silcPrivateKeyFileEdit;

	QComboBox * _silcCipherComboBox;

	QComboBox * _silcHMACComboBox;

	QCheckBox * _silcPublicKeyAuthCheckBox;

	QCheckBox * _silcRejectWatchingCheckBox;

	QCheckBox * _silcBlockInvitesCheckBox;

	QCheckBox * _silcBlocksIMCheckBox;

	QCheckBox * _silcRejectOnlineStatusCheckBox;

	QCheckBox * _silcBlockMessageCheckBox;

	QCheckBox * _silcAutomaticallyOpenWhiteboardCheckBox;

	QCheckBox * _silcDigitallySignCheckBox;

	QComboBox * _silcProxyComboBox;

	// SIMPLE Options (not supported)

	QLineEdit * _simpleScreenNameEdit;

	QLineEdit * _simpleServerEdit;

	QLineEdit * _simplePasswordEdit;

	QLineEdit * _simpleAliasEdit;

	QCheckBox * _simpleRememberPasswordCheckBox;

	QCheckBox * _simplePublishStatusCheckBox;

	QLineEdit * _simpleConnectPortEdit;

	QCheckBox * _simpleUDPCheckBox;

	QCheckBox * _simpleProxyCheckBox;

	QLineEdit * _simpleProxyEdit;

	QComboBox * _simpleProxyComboBox;

	// SIMETIME (not supported)

	QLineEdit * _sametimeScreenNameEdit;

	QLineEdit * _sametimePasswordEdit;

	QLineEdit * _sametimeAliasEdit;

	QCheckBox * _simeTimeRememberPasswordCheckBox;

	QLineEdit * _sametimeServerEdit;

	QLineEdit * _sametimePortEdit;

	QCheckBox * _sametimeForceLoginCheckBox;

	QCheckBox * _sametimeHideClientCheckBox;

	QComboBox * _sametimeProxyComboBox;

	// Yahoo

	QLineEdit * _yahooScreenNameEdit;

	QLineEdit * _yahooPasswordEdit;

	QLineEdit * _yahooAliasEdit;

	QCheckBox * _yahooRememberPasswordCheckBox;

	QCheckBox * _yahooNewMailNotificationCheckBox;

	QPushButton * _yahooOpenPushButton;

	QPushButton * _yahooRemovePushButton;

	QCheckBox * _yahooJapanCheckBox;

	QLineEdit * _yahooPagerHostEdit;

	QLineEdit * _yahooJapanPagerHostEdit;

	QLineEdit * _yahooPagerPortEdit;

	QLineEdit * _yahooFileTransferHostEdit;

	QLineEdit * _yahooJapanFileTransferHostEdit;

	QLineEdit * _yahooFileTransferPortEdit;

	QLineEdit * _yahooChatRoomLocalEdit;

	QCheckBox * _yahooIgnoreConferenceCheckBox;

	QComboBox * _yahooProxyComboBox;

public Q_SLOTS:

	void setCurrentPage(AvailableProtocols index);

	void setCurrentPageProxy(int index);
};

#endif
