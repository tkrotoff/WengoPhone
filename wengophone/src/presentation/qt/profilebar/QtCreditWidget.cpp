#include <control/CWengoPhone.h>

#include "QtCreditWidget.h"

QtCreditWidget::QtCreditWidget(QWidget * parent , Qt::WFlags f ) : QWidget(parent,f){

/*
	QPalette p = palette();
	p.setColor(QPalette::Active,QPalette::Window,Qt::white);
	setPalette(p);
	setAutoFillBackground(true);
*/
	QGridLayout * gridLayout = new QGridLayout(this);

	QtClickableLabel * buy = new QtClickableLabel(this);
	//QtClickableLabel * subscribe = new QtClickableLabel(this);

	buy->setText( tr ("Buy call-out credits") );
	//subscribe->setText( tr ("Subscribe to the PSTN unlimited") );

	gridLayout->addWidget(buy,0,0);
	//gridLayout->addWidget(subscribe,1,0);

	connect ( buy, SIGNAL ( clicked() ), SLOT ( buyOutClicked() ) );

}

void QtCreditWidget::buyOutClicked(){
	_cWengoPhone->showWengoBuyWengos();
}

void QtCreditWidget::setCWengoPhone(CWengoPhone * cwengophone){
	_cWengoPhone = cwengophone;
}
