#include "QtUserWidgetAvatarManager.h"

QtUserWidgetAvatarManager::QtUserWidgetAvatarManager(QObject * parent, QFrame * target) : QObject(parent)
{
	_target = target;
	_isSet = false;
}

void QtUserWidgetAvatarManager::setAvatar(QByteArray svg){
	_svg = svg;
	_svgRenderer.load(_svg);
	_isSet = true;
}

QByteArray QtUserWidgetAvatarManager::getSvg()
{
	return _svg;
}

bool QtUserWidgetAvatarManager::eventFilter(QObject *obj, QEvent *event)
{
	switch (event->type())
    {
        case QEvent::Paint:
            paintEvent(dynamic_cast<QPaintEvent *>(event));
            return true;
        default:
            return QObject::eventFilter(obj, event);
    }
}

void QtUserWidgetAvatarManager::paintEvent(QPaintEvent * event)
{

	if (_isSet)
	{
		// Get a painter
		QPainter painter(_target);
		_svgRenderer.render(&painter);
	}
}

void QtUserWidgetAvatarManager::loadAvatar(QString path)
{
	QFile file(path);
	QByteArray tmp;
	if (file.open(QIODevice::ReadOnly))
	{
		tmp = file.readAll ();
		if ( !tmp.isEmpty() )
		{
			setAvatar(tmp);
		}
		file.close();
	}
}
