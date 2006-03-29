#ifndef QTHIDEDCONTACT_H
#define QTHIDENCONTACT_H

#include <QtGui>

class QtUser;

class QtHidenContact : public QObject
{
	Q_OBJECT

public:

	QtHidenContact ( QTreeWidgetItem * item, QTreeWidgetItem * parentItem, QtUser * user, int index,QObject * parent = 0);

	virtual ~QtHidenContact();

	QTreeWidgetItem * getItem();

	QTreeWidgetItem * getParentItem();

	QtUser * getUser();

	int getIndex();

protected:

	QTreeWidgetItem * _item;

	QTreeWidgetItem * _parentItem;

	QtUser * _user;

	int _index;

};

#endif