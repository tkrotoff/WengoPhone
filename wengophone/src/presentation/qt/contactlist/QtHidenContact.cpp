#include "QtHidenContact.h"


QtHidenContact::QtHidenContact ( QTreeWidgetItem * item, QTreeWidgetItem * parentItem, QtUser * user, int index, QObject * parent) : QObject(parent) {
	_user = user;
	_item = item;
	_parentItem = parentItem;
	_index = index;
}

QtHidenContact::~QtHidenContact(){
}

QTreeWidgetItem * QtHidenContact::getItem(){
	return _item;
}

QTreeWidgetItem * QtHidenContact::getParentItem(){
	return _parentItem;
}

QtUser * QtHidenContact::getUser(){
	return _user;
}

int QtHidenContact::getIndex(){
	return _index;
}