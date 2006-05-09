#ifndef QTHIDEDCONTACT_H
#define QTHIDENCONTACT_H

#include <QtGui>

class QtUser;

class QtHidenContact : public QObject
{
	Q_OBJECT

public:

	QtHidenContact ( QTreeWidgetItem * item, QTreeWidgetItem * parentItem, QtUser * user, int index,QObject * parent = 0);

	QtHidenContact ( const QtHidenContact & other);


	virtual ~QtHidenContact();

	QTreeWidgetItem * getItem();

	QTreeWidgetItem * getParentItem();

	QtUser * getUser() const;

	int getIndex() const;

	void clear();

	bool isCleared();

	virtual bool operator <(const QtHidenContact & other ) const ;

    virtual QtHidenContact & operator =(const QtHidenContact & other );

protected:

	QTreeWidgetItem * _item;

	QTreeWidgetItem * _parentItem;

	QtUser * _user;

	int _index;

	bool _clear;

};

#endif
