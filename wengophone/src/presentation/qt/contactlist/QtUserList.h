#ifndef QTUSERLIST_H
#define QTUSERLIST_H

#include <QtGui>
#include "QtUser.h"
#include "QtContactPixmap.h"
#include "QtUserList.h"

class QtUserList 
{
public:
	static QtUserList * getInstance();
	
	void	addUser(QtUser * user);
	void	removeUser(const QString & userid);
	
	void	paintUser(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index);
	void	mouseOn(const QString & userid);
	void	setUserStatus(const QString & userid,QtContactPixmap::contactPixmap status);
	
	
	bool	haveIM(const QString & userid);
	
	bool	haveCall(const QString & userid);
	
	bool	haveVideo(const QString & userid);

	QtUser * getUser(const QString & userid);
	
	void	mouseClicked(const QString & userid,const QPoint pos, const QRect & rect); 
	
	int		getIconsStartPosition(const QString & userid);
	
	void	setButton(const QString & userid,const Qt::MouseButton button);
	
	void	setOpenStatus(const QString & userid,bool value);
	
	Qt::MouseButton getButton(const QString & userid);
protected:
	QtUserList ( );
	QtUserList (const QtUserList&) {};
	QtUserList & operator= (const QtUserList &) {};
protected:
	QHash<QString,QtUser *>	_userList;
private:
	static QtUserList * _instance;
	QString			    _lastMouseOn;
};
#endif 
