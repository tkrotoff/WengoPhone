#ifndef QTCREDITWIDGET_H
#define QTCREDITWIDGET_H

#include <QtGui>
#include "QtClickableLabel.h"

class CWengoPhone;

class QtCreditWidget : public QWidget
{
	Q_OBJECT

	public:

	QtCreditWidget(QWidget * parent =0, Qt::WFlags f = 0);

	void setCWengoPhone(CWengoPhone * cwengophone);


public Q_SLOTS:

	void buyOutClicked();


	protected:

	CWengoPhone * _cWengoPhone;
};


#endif;

