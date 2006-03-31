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
		openPopup( QPoint( e-> globalPos() ) );
	}
}
