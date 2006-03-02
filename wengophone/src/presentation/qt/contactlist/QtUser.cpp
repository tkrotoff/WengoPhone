#include "QtUser.h"

QtUser::QtUser (QObject * parent) : QObject (parent)
{
	_mouseOn = false;
	_haveIM = false;
	_haveCall = false;
	_haveVideo = false;
}

void QtUser::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index)
{
    QRect r;
	QPixmap px;
	QtContactPixmap * spx;
	int x;
    bool parentItem = false;
	
	QLinearGradient lg( QPointF(1,1), QPointF(option.rect.width(),1));
	lg.setSpread(QGradient::RepeatSpread);
	lg.setColorAt ( 0, option.palette.midlight().color());
	lg.setColorAt ( .8, QColor(210, 216, 234));
	
	spx = QtContactPixmap::getInstance();
	/*
    if ( (option.state & QStyle::State_Selected)){
        painter->fillRect(option.rect,option.palette.highlight());
        painter->setPen(option.palette.highlightedText().color() );
    }
	*/
	if (_mouseOn)
	{
		// painter->fillRect(option.rect,option.palette.midlight());
		painter->fillRect(option.rect,QBrush(lg));
		painter->setPen(option.palette.text().color());
	}
    else
    {
        painter->setPen(option.palette.text().color() );
    }
	// Draw the status pixmap
	px = spx->getPixmap(_status);
	
    r = option.rect;
	x = r.left();
	painter->drawPixmap (x,r.top(),px);
	x+=px.width()+5;
	r.setLeft(x);
    
    if (  ! index.child(0,0).isValid() ){
        painter->setFont(option.font);
        painter->drawText(r,Qt::AlignLeft,index.data().toString(),0);
    }
    else
    {
        QFont f = option.font;
        f.setBold(true);
        painter->setFont(f);
        painter->drawText(r,Qt::AlignLeft,index.data().toString(),0);
        parentItem=true;
    }
    /*
            Draw Functions icons
        */
	x=option.rect.width();
	if (_mouseOn)
	{
		if (_status != QtContactPixmap::ContactNotAvailable)
		{
			
			px = spx->getPixmap(QtContactPixmap::ContactVideo);
			if (haveVideo())
			{
				x-=px.width();
				painter->drawPixmap (x,r.top(),px);
			}
			else
				x-=px.width();
				
			px = spx->getPixmap(QtContactPixmap::ContactCall);
			if (haveCall())
			{
				x-=px.width();
				painter->drawPixmap (x,r.top(),px);
			}
			else
				x-=px.width();
				
			px = spx->getPixmap(QtContactPixmap::ContactIM);
			if (haveIM())
			{
				px = spx->getPixmap(QtContactPixmap::ContactIM);
				x-=px.width();
				painter->drawPixmap (x,r.top(),px);
			}
			else
				x-=px.width();
			_iconsStartPosition = x;
		}
		else
			_iconsStartPosition = x;
	}
		
}

QString  QtUser::getId()
{
	return _userId;
}
void	QtUser::setId(const QString & id)
{
	_userId = id;
}

void QtUser::setUserName(const QString & uname)
{
	_userName = uname;
}
QString	QtUser::getUserName()
{
	return _userName;
}

void QtUser::setStatus(QtContactPixmap::contactPixmap status)
{
	_status = status;
}

QtContactPixmap::contactPixmap QtUser::getStatus(){
	return _status;
}

void QtUser::mouseClicked(const QPoint & pos, const QRect & rect)
{
	QPixmap px;
	QtContactPixmap * spx;	
	int x;
	
	spx = QtContactPixmap::getInstance();
	
	x=rect.width();
	
	
	
	px = spx->getPixmap(QtContactPixmap::ContactVideo);
	if (haveVideo())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) )
			qDebug() << "Video clicked";
	}
	else
		x-=px.width();


	px = spx->getPixmap(QtContactPixmap::ContactCall);		
	if (haveCall())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) )
			qDebug() << "Call clicked";
	}
	else
		x-=px.width();

	px = spx->getPixmap(QtContactPixmap::ContactIM);		
	if (haveIM())
	{
		x-=px.width();
		if ( (pos.x()>=x) && (pos.x()<=x+px.width()) )
			qDebug() << "IM clicked";
	}
	else
		x-=px.width();
}
void QtUser::setButton(const Qt::MouseButton button)
{
	_mouseButton = button;
}

 Qt::MouseButton QtUser::getButton()
{
	return _mouseButton;
}

void QtUser::setOpenStatus(bool value)
{
	_openStatus = value;
}