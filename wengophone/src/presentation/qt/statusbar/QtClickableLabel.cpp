#include "QtClickableLabel.h"

QtClickableLabel::QtClickableLabel( QWidget * parent , Qt::WFlags f ) : QLabel( parent, f ) {
	_mousePressed = false;
}

void QtClickableLabel::mousePressEvent ( QMouseEvent * ) {
	_mousePressed = true;
}

void QtClickableLabel::mouseReleaseEvent ( QMouseEvent * e ) {
	/* get the widget rect */
	const QRect rect = this->rect();
	/* get the mouse position relative to this widget */
	QPoint mousePosition = e->pos();
	if ( ( mousePosition.x() >= rect.x() ) && ( mousePosition.x() <= rect.width() ) ){
		if ( ( mousePosition.y() >= rect.y() ) && ( mousePosition.y() <= rect.height() ) ){
			clicked();
		}
	}
}
