#ifndef QTCLICKABLELABLE_H
#define QTCLICKABLELABLE_H

#include <QtGui>

class QtClickableLabel : public QLabel {
		Q_OBJECT

	public:

		QtClickableLabel( QWidget * parent = 0, Qt::WFlags f = 0 );

		void setSelected(bool value);

		void setSelectedBgColor(const QColor & bgcolor);

	protected:

		virtual void mousePressEvent ( QMouseEvent * e );

		virtual void mouseReleaseEvent ( QMouseEvent * e );

		QColor _bgcolor;

		bool _mousePressed;

	Q_SIGNALS:

		void clicked();

		void clicked(const QString & text);

		void clicked(QtClickableLabel * instance);


	};

#endif
