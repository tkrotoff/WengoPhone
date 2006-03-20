#ifndef QTSTATUSBAR_H
#define QTSTATUSBAR_H

#include <QtGui>

#include "QtClickableLabel.h"
#include "QtNickNameWidget.h"

class QtStatusBar : public QWidget
{
	Q_OBJECT

public:
	QtStatusBar ( QWidget * parent = 0, Qt::WFlags f = 0 );

protected:

	QGridLayout * _gridlayout;

	QGridLayout * _widgetLayout;

	QtClickableLabel * _statusLabel;

	QtClickableLabel * _nicknameLabel;

	QtClickableLabel * _eventsLabel;

	QtClickableLabel * _creditLabel;

	QtNickNameWidget * _nickNameWidget;

	bool _nickNameWidgetVisible;

	QPixmap _statusPixmap;

	QMenu * _statusMenu;

	void createStatusMenu();

public Q_SLOTS:

	void statusClicked();

	void nicknameClicked();

	void eventsClicked();

	void creditClicked();

	// Status menu actions
	void onlineClicked(bool checked);

	void dndClicked(bool checked);

	void invisibleClicked(bool checked);

	void brbClicked(bool checked);

	void awayClicked(bool checked);

	void notAvailableClicked(bool checked);

	void forwardClicked(bool checked);
};

#endif