#include "QtHistoryItem.h"

QtHistoryItem::QtHistoryItem(int type ) : QTreeWidgetItem(type){
}
QtHistoryItem::QtHistoryItem ( const QStringList & strings, int type ) : QTreeWidgetItem(strings,type){
}
QtHistoryItem::QtHistoryItem ( QTreeWidget * parent, int type ) : QTreeWidgetItem(parent,type){
}
QtHistoryItem::QtHistoryItem ( QTreeWidget * parent, const QStringList & strings, int type  ) : QTreeWidgetItem(parent,strings,type){
}
QtHistoryItem::QtHistoryItem ( QTreeWidget * parent, QtHistoryItem * preceding, int type ) : QTreeWidgetItem(parent,preceding,type){
}
QtHistoryItem::QtHistoryItem ( QtHistoryItem * parent, int type  ) : QTreeWidgetItem(parent,type) {
}
QtHistoryItem::QtHistoryItem ( QtHistoryItem * parent, const QStringList & strings, int type ) : QTreeWidgetItem(parent,strings,type){
}
QtHistoryItem::QtHistoryItem ( QtHistoryItem * parent, QtHistoryItem * preceding, int type ) : QTreeWidgetItem(parent,preceding,type){
}
QtHistoryItem::QtHistoryItem ( const QtHistoryItem & other ) : QTreeWidgetItem(other){
}

int QtHistoryItem::getId() const{
	return _id;
}
void QtHistoryItem::setId(int id){
	_id = id;
}

void QtHistoryItem::setItemType(QtHistoryItem::HistoryType type){
	_type = type;
}

QtHistoryItem::HistoryType QtHistoryItem::getItemType() const{
	return _type;
}



QtHistoryItem::~QtHistoryItem()
{
}
