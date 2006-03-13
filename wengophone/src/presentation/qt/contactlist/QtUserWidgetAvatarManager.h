#ifndef QTUSERWIDGETAVATARMANAGER_H
#define QTUSERWIDGETAVATARMANAGER_H

#include <QtGui>
#include <QtSvg>

class QtUserWidgetAvatarManager : public QObject
{
	Q_OBJECT
public:

	QtUserWidgetAvatarManager(QObject * parent, QFrame * target);
	
	void setAvatar(QByteArray svg);
	
	QByteArray getSvg();
	
	void loadAvatar(QString path);

protected:

	bool eventFilter(QObject *obj, QEvent *event);
	
	void paintEvent(QPaintEvent * event);
	
	QFrame * _target;
	
	QByteArray _svg;
	
	QSvgRenderer _svgRenderer;
	
	bool	_isSet;
};

#endif // QTUSERWIDGETAVATARMANAGER_H
