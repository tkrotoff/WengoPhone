#ifndef QTUSER_H
#define QTUSER_H
#include <QtGui>
#include "QtContactPixmap.h"
class QtUser : QObject
{
	Q_OBJECT
public:
	QtUser ( QObject * parent = 0);
	virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index);
	
	QString		getId();
	void		setId(const QString & id);
	
	QString		getUserName();
	void		setUserName(const QString & uname);
	
	void 		haveIM( bool value){ _haveIM = value;};
	bool		haveIM(){ return _haveIM;};
	bool		haveCall(){ return _haveCall;};
	void		haveCall(bool value){_haveCall=value;};
	bool		haveVideo(){ return _haveVideo;};
	void		haveVideo(bool value){ _haveVideo=value;};
	
	void		setMouseOn(bool value) { _mouseOn = value;};
	
	void		setStatus(QtContactPixmap::contactPixmap status);
	
	void		mouseClicked(const QPoint & pos,const QRect & rec);
	
	QtContactPixmap::contactPixmap getStatus();
	
	void		setFunction(bool im, bool call, bool video);
	
	int			getIconsStartPosition() {return _iconsStartPosition;};
	
	void		setButton(const Qt::MouseButton button);
	
	void		setOpenStatus(bool value);
	
	Qt::MouseButton getButton();
	
public Q_SLOTS:
Q_SIGNALS:
	void clicked(QtUser * user,int prt);
protected:	
	QString		_userId;
	QString		_userName;
	
	QPixmap		_imPixmap;
	QPixmap		_callPixmap;
	QPixmap		_videoPixmap;

	QtContactPixmap::contactPixmap _status;
	
	bool		_haveIM;
	bool		_haveCall;
	bool		_haveVideo;
	bool		_mouseOn;
	bool		_openStatus;
	int			_iconsStartPosition;
	
	Qt::MouseButton _mouseButton;
};

#endif

