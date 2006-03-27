#ifndef QTACCOUNTMANAGERWIDGET_H
#define QTACCOUNTMANAGERWIDGET_H

#include <QtGui>

class WengoPhone;

class QtAccountManager : public QDialog
{

	Q_OBJECT
public:

	QtAccountManager(WengoPhone & wengoPhone, QWidget * parent=0);

protected:

	void loadImAccounts();

	WengoPhone & _wengoPhone;

	QWidget * _widget;

	QPushButton * _imAddPushButton;

	QPushButton * _imDeletePushButton;

	QPushButton * _imModifyPushButton;

	QPushButton * _closePushButton;

	QTreeWidget * _treeWidget;

protected Q_SLOTS:

	void addImAccount();
	void deleteImAccount();
	void modifyImAccount();
	void itemDoubleClicked ( QTreeWidgetItem * item, int column );
};
#endif
