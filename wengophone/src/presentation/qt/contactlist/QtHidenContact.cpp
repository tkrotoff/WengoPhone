#include "QtHidenContact.h"
#include "QtUser.h"

QtHidenContact::QtHidenContact ( QTreeWidgetItem * item, QTreeWidgetItem * parentItem, QtUser * user, int index, QObject * parent) : QObject(parent) {
	_user = user;
	_item = item;
	_parentItem = parentItem;
	_index = index;
	_clear = false;
}

QtHidenContact::QtHidenContact ( const QtHidenContact & other){

	_user = other._user;
	_item = other._item;
	_parentItem  = other._parentItem;
	_index = other._index;
	_clear = other._clear;
}

QtHidenContact::~QtHidenContact(){
}

void QtHidenContact::clear(){
	_clear = true;

}

bool QtHidenContact::isCleared(){
	return _clear;
}

QTreeWidgetItem * QtHidenContact::getItem(){
	return _item;
}

QTreeWidgetItem * QtHidenContact::getParentItem(){
	return _parentItem;
}

QtUser * QtHidenContact::getUser() const{
	return _user;
}

int QtHidenContact::getIndex() const {
	return _index;
}

bool QtHidenContact::operator<(const QtHidenContact & other ) const {

	return (_user->getUserName().toUpper() < other.getUser()->getUserName().toUpper()) ;
}


QtHidenContact & QtHidenContact::operator =(const QtHidenContact & other ){
	_user = other._user;
	_item = other._item;
	_parentItem  = other._parentItem;
	_index = other._index;
	_clear = other._clear;
	return *this;
}
