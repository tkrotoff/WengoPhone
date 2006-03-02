#ifndef QTSTATUSPIXMAP_H
#define QTSTATUSPIXMAP_H

#include <QtGui>

class QtContactPixmap
{
public:
	enum contactPixmap { ContactOnline=1, ContactDND, ContactInvisible, ContactBRB, ContactAway, ContactNotAvailable, ContactForward,
						 ContactIM, ContactCall, ContactVideo,
						 ContactGroupOpen, ContactGroupClose
						};
	
	static QtContactPixmap * getInstance();

	void setPixmap (contactPixmap status,const  QPixmap & px);
	QPixmap & getPixmap(contactPixmap  status);
	
	protected:
	QtContactPixmap ( );
	QtContactPixmap (const QtContactPixmap&) {};
	QtContactPixmap & operator= (const QtContactPixmap &) {};
	
protected:
	QHash<contactPixmap, QPixmap>	_contactPixmap;
	
private:
	static QtContactPixmap * _instance;

};
#endif 
