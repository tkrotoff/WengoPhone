#ifndef QTNICKNAMEWIDGET_H
#define QTNICKNAMEWIDGET_H

#include <QtGui>
#include "QtClickableLabel.h"

class QtNickNameWidget : public QWidget
{
	Q_OBJECT

public:

	QtNickNameWidget (QWidget * parent = 0, Qt::WFlags f = 0);

protected:

	QtClickableLabel * _msnLabel;

	QtClickableLabel * _yahooLabel;

	QtClickableLabel * _wengoLabel;

	QtClickableLabel * _aimLabel;

	QtClickableLabel * _jabberLabel;

	QtClickableLabel * _avatarLabel;

	QLineEdit * _nickNameEdit;

	QGridLayout * _protocolLayout;

	QGridLayout * _widgetLayout;

	QMenu * _msnIMAccountMenu;

	QMenu * _yahooIMAccountMenu;

	QMenu * _wengoIMAccountMenu;

	QMenu * _aimIMAccountMenu;

	QMenu * _jabberIMAccountMenu;


	void showMsnMenu();

	void showYahooMenu();

	void showWengoMenu();

	void showAimMenu();

	void showJabberMenu();

public Q_SLOTS:

	void msnClicked();

	void yahooClicked();

	void wengoClicked();

	void aimClicked();

	void jabberClicked();

	void avatarClicked();


	// Menus actions

	void wengoOnlineClicked(bool checked);
	void wengoDNDClicked(bool checked);
	void wengoInvisibleClicked(bool checked);
	void wengoBRBClicked(bool checked);
	void wengoAwayClicked(bool checked);
	void wengoNotAvailableClicked(bool checked);
	void wengoForwardClicked(bool checked);

	void yahooAvailableClicked(bool checked);
	void yahooBusyClicked(bool checked);
	void yahooAwayClicked(bool checked);
	void yahooNotHereClicked(bool checked);
	void yahooOnPhoneClicked(bool checked);
	void yahooHiddenClicked(bool checked);
	void yahooDisconnectClicked(bool checked);
	void yahooReconnectClicked(bool checked);
	void yahooBRBClicked(bool checked);

	void msnOnlineClicked(bool checked);
	void msnDNDClicked(bool checked);
	void msnBRBClicked(bool checked);
	void msnAwayClicked(bool checked);
	void msnOnPhoneClicked(bool checked);
	void msnLunchClicked(bool checked);
	void msnOffLineClicked(bool checked);
	void msnDisconnectClicked(bool checked);
	void msnReconnectClicked(bool checked);

	void aimAvailableClicked(bool checked);
	void aimAwayClicked(bool checked);
	void aimNAClicked(bool checked);
	void aimPrivacyClicked(bool checked);
	void aimOffLineClicked(bool checked);
	void aimDisconnectClicked(bool checked);
	void aimReconnectClicked(bool checked);

	void jabberAvailableClicked(bool checked);
	void jabberAwayClicked(bool checked);
	void jabberChatClicked(bool checked);
	void jabberDNDClicked(bool checked);
	void jabberNAClicked(bool checked);
	void jabberDisconnectClicked(bool checked);
	void jabberReconnectClicked(bool checked);

};

#endif