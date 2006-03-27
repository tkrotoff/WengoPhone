#include "QtImAccountItem.h"


QtImAccountItem::QtImAccountItem ( QTreeWidget * parent, const QStringList & strings, int type ) :
QTreeWidgetItem (parent,strings,type){
}

void QtImAccountItem::setImAccount(IMAccount * imaccount){
	_imAccount = imaccount;
}

IMAccount * QtImAccountItem::getImAccount(){
	return _imAccount;
}
