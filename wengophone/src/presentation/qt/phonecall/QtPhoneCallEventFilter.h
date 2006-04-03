#ifndef QTPHONECALLEVENTFILTER_H
#define QTPHONECALLEVENTFILTER_H

#include <QtGui>

class QtPhoneCallEventFilter : public QObject
{
	Q_OBJECT

public:

	QtPhoneCallEventFilter(QObject * parent = 0);

Q_SIGNALS:

	void openPopup( const QPoint * pos );


protected:

	bool eventFilter(QObject *obj, QEvent *event);

	void mouseEvent(QMouseEvent * e);

};

#endif // QTPHONECALLEVENTFILTER_H