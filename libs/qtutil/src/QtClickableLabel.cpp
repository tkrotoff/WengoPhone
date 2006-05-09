#include <qtutil/QtClickableLabel.h>

QtClickableLabel::QtClickableLabel( QWidget * parent , Qt::WFlags f ) : QLabel( parent, f ) {
	_mousePressed = false;
//	setAutoFillBackground(true);
	_bgcolor = Qt::white;
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
			clicked(text());
			clicked(this);
		}
	}
}

void QtClickableLabel::setSelectedBgColor(const QColor & bgcolor){
	_bgcolor = bgcolor;
}

void QtClickableLabel::setSelected(bool value){

	if ( value ){
		QPalette p = palette();
		p.setColor(QPalette::Active,QPalette::Window,Qt::white);
		setPalette(p);
		update();
	}
	else
	{
		QPalette p = palette();
		p.setColor(QPalette::Active,QPalette::Window,qApp->palette().window().color());
		setPalette(p);
		update();
	}

}
