#ifndef OWCONTACTINFO_H
#define OWCONTACTINFO_H

#include <QObject>
#include "QtContactPixmap.h"

class QTreeWidgetItem;
class QtContact;


/**
 * Store contact informations for sorting
 *
 * @author Mr K
 */
class QtContactInfo : public QObject {
	Q_OBJECT
public:

	QtContactInfo(QTreeWidgetItem * item, QTreeWidgetItem * parentItem, const QString &contactId, int index, QObject * parent = 0);

	QtContactInfo(const QtContactInfo & other);

	virtual ~QtContactInfo();

	QTreeWidgetItem * getItem();

	QTreeWidgetItem * getParentItem();

	QString getContactId() const;

	int getIndex() const;

	void clear();

	bool isCleared();

	QtContactPixmap::ContactPixmap getStatus();

	virtual bool operator < (const QtContactInfo & other) const;

	virtual QtContactInfo & operator = (const QtContactInfo & other);

protected:

	QTreeWidgetItem * _item;

	QTreeWidgetItem * _parentItem;

	QString _contactId;

	int _index;

	bool _clear;
};

#endif	//OWCONTACTINFO_H