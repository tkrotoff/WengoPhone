#include "QtEventWidget.h"

QtEventWidget::QtEventWidget( QWidget * parent, Qt::WFlags f ) : QWidget ( parent, f ) {

	QPalette p = palette();
	p.setColor(QPalette::Active,QPalette::Window,Qt::white);
	setPalette(p);
	setAutoFillBackground(true);

	QGridLayout * gridLayout = new QGridLayout(this);

	_missedCallLabel = new QtClickableLabel(this);
	_missedCallLabel->setText( tr ("Missed calls") );

	_newMessagesLabel = new QtClickableLabel(this);
	_newMessagesLabel->setText( tr("New messages") );

	gridLayout->addWidget( _missedCallLabel ,0,0 );
	gridLayout->addWidget( _newMessagesLabel,0,1 );

}

void QtEventWidget::addMissedCall(const QString & nickName){

	QtClickableLabel * label = new QtClickableLabel(this);
	label->setText(nickName);
	QGridLayout * gridLayout = dynamic_cast<QGridLayout *>(layout());
	int row = gridLayout->rowCount ();
	gridLayout->addWidget( label, row, 0);

}

void QtEventWidget::addMessage(const QString & nickName){

	QtClickableLabel * label = new QtClickableLabel(this);
	label->setText(nickName);
	QGridLayout * gridLayout = dynamic_cast<QGridLayout *>(layout());
	int row = gridLayout->rowCount ();
	gridLayout->addWidget( label, row, 1);
}
