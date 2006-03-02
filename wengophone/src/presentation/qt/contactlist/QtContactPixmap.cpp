#include "QtContactPixmap.h"


QtContactPixmap * QtContactPixmap::_instance = 0;

QtContactPixmap::QtContactPixmap( )
{
}

QtContactPixmap * QtContactPixmap::getInstance()
{
	if (_instance == 0)
	{
		_instance = new QtContactPixmap;
	}
	return _instance;
}

void QtContactPixmap::setPixmap (contactPixmap status,const  QPixmap & px)
{
	_contactPixmap[status] = px;
}
QPixmap & QtContactPixmap::getPixmap(contactPixmap status) 
{
	return _contactPixmap[status];
}
