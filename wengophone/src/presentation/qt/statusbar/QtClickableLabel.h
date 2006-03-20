#ifndef QTCLICKABLELABLE_H
#define QTCLICKABLELABLE_H

#include <QtGui>

class QtClickableLabel : public QLabel {
		Q_OBJECT

	public:

		QtClickableLabel( QWidget * parent = 0, Qt::WFlags f = 0 );

	protected:

		virtual void mousePressEvent ( QMouseEvent * e );
		virtual void mouseReleaseEvent ( QMouseEvent * e );
		bool _mousePressed;

	Q_SIGNALS:

		void clicked();


	};

#endif;
