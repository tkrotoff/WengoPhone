#include "QtPhoneCallEventFilter.h"


QtPhoneCallEventFilter::QtPhoneCallEventFilter(QObject * parent) : QObject (parent) {
}


bool QtPhoneCallEventFilter::eventFilter(QObject *obj, QEvent *event){
		switch (event->type())
    {
        case QEvent::MouseButtonPress:
            // paintEvent(dynamic_cast<QPaintEvent *>(event));

            mouseEvent(dynamic_cast<QMouseEvent *>(event));

            return true;
            break;
        default:
            return QObject::eventFilter(obj, event);
    }
}

void QtPhoneCallEventFilter::mouseEvent(QMouseEvent * e){

	if ( e->button() == Qt::RightButton ){
		QPoint  p = QPoint(e-> globalPos());
		openPopup( p.x(), p.y() );
	}
}
