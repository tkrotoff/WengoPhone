#include "QtStatusBar.h"

QtStatusBar::QtStatusBar( QWidget * parent , Qt::WFlags f ) : QWidget (parent,f){

	_statusMenu = NULL;

	_nickNameWidgetVisible = false;
	_eventsWidgetVisible = false;
	_crediWidgetVisible = false;

	_widgetLayout = new QGridLayout(this);
	_widgetLayout->setMargin(0);
	_widgetLayout->setSpacing(0);

	_gridlayout = new QGridLayout();
	_gridlayout->setMargin(0);
	_gridlayout->setSpacing(10);

	_widgetLayout->addLayout(_gridlayout,0,0);

	// The status widget
	_statusLabel = new QtClickableLabel(this);
	_statusLabel->setMinimumSize(QSize(16,16));
	_statusLabel->setMaximumSize(QSize(16,16));

	// Nickname label
	_nicknameLabel = new QtClickableLabel(this);

	// The events label
	_eventsLabel = new QtClickableLabel(this);

	// The credit label
	_creditLabel = new QtClickableLabel(this);

	// Add the labels to the gridlayout
	_gridlayout->addWidget( _statusLabel   , 0, 0 );
	_gridlayout->addWidget( _nicknameLabel , 0, 1 );
	_gridlayout->addWidget( _eventsLabel   , 0, 2 );
	_gridlayout->addWidget( _creditLabel   , 0, 3 );

	_statusLabel->setText("S");
	_nicknameLabel->setText("NickName");
	_eventsLabel->setText("events");
	_creditLabel->setText("E X,XX");

	// Objects connection
	connect(_statusLabel,SIGNAL(clicked()),SLOT(statusClicked()));
	connect(_nicknameLabel,SIGNAL(clicked()),SLOT(nicknameClicked()));
	connect(_eventsLabel,SIGNAL(clicked()),SLOT(eventsClicked()));
	connect(_creditLabel,SIGNAL(clicked()),SLOT(creditClicked()));
}


void QtStatusBar::statusClicked(){
	createStatusMenu();
}

void QtStatusBar::nicknameClicked(){
	if ( _nickNameWidgetVisible ){
		removeNickNameWidget();
	}
	else{
		_nicknameLabel->setSelected(true);
		_eventsLabel->setSelected(false);
		_creditLabel->setSelected(false);
		showNickNameWidget();
	}
}

void QtStatusBar::showNickNameWidget(){

	removeEventsWidget();
	removeCreditWidget();

	if ( ! _nickNameWidgetVisible )
	{
		_nickNameWidget = new QtNickNameWidget( this );
		_widgetLayout->addWidget(_nickNameWidget, 1, 0 );
		_nickNameWidgetVisible=true;
	}
}

void QtStatusBar::removeNickNameWidget(){

	if ( _nickNameWidgetVisible ){
		_widgetLayout->removeWidget ( _nickNameWidget );
		_nickNameWidget->setVisible(false);
		setMinimumSize(QSize(0,0));
		resize(QSize(0,0));
		delete _nickNameWidget;
		_nickNameWidgetVisible=false;
		_nicknameLabel->setSelected(false);
	}
}

void QtStatusBar::eventsClicked(){
	if ( _eventsWidgetVisible )
		removeEventsWidget();
	else
	{
		_nicknameLabel->setSelected(false);
		_eventsLabel->setSelected(true);
		_creditLabel->setSelected(false);
		showEventsWidget();
	}
}

void QtStatusBar::showEventsWidget(){

	removeNickNameWidget();
	removeCreditWidget();
	if ( !_eventsWidgetVisible ){
		_eventWidget = new QtEventWidget(this);
		_widgetLayout->addWidget(_eventWidget, 1, 0);
		_eventsWidgetVisible=true;
	}
}

void QtStatusBar::removeEventsWidget(){
	if ( _eventsWidgetVisible ){
		_widgetLayout->removeWidget( _eventWidget );
		_eventWidget->setVisible(false);
		setMinimumSize(QSize(0,0));
		resize(QSize(0,0));
		delete _eventWidget;
		_eventsWidgetVisible=false;
		_eventsLabel->setSelected(false);
	}
}

void QtStatusBar::creditClicked(){
	if ( _crediWidgetVisible )
		removeCreditWidget();
	else{
		_nicknameLabel->setSelected(false);
		_eventsLabel->setSelected(false);
		_creditLabel->setSelected(true);
		showCreditWidget();
	}
}

void QtStatusBar::showCreditWidget(){
	removeNickNameWidget();
	removeEventsWidget();
	if ( ! _crediWidgetVisible ){
		_creditWidget = new QtCreditWidget(this);
		_widgetLayout->addWidget(_creditWidget, 1, 0);
		_crediWidgetVisible=true;
	}
}

void QtStatusBar::removeCreditWidget(){

	if ( _crediWidgetVisible ){
		_widgetLayout->removeWidget( _creditWidget );
		_creditWidget->setVisible(false);
		setMinimumSize(QSize(0,0));
		resize(QSize(0,0));
		delete _creditWidget;
		_crediWidgetVisible=false;
		_creditLabel->setSelected(false);
	}
}


void QtStatusBar::createStatusMenu(){
	if (_statusMenu)
		delete _statusMenu;
	_statusMenu = new QMenu(this);

	QAction * action;

	action = _statusMenu->addAction( tr( "Online" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( onlineClicked(bool) ) );

	action = _statusMenu->addAction( tr( "DND" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( dndClicked(bool) ) );

	action = _statusMenu->addAction( tr( "Invisible" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( invisibleClicked(bool) ) );

	action = _statusMenu->addAction( tr( "Be right back" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( brbClicked(bool) ) );

	action = _statusMenu->addAction( tr( "Away" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( awayClicked(bool) ) );

	action = _statusMenu->addAction( tr( "Not available" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( notAvailableClicked(bool) ) );

	action = _statusMenu->addAction( tr( "Forward to cellphone" ) );
	connect(action,SIGNAL( triggered (bool) ),SLOT( forwardClicked(bool) ) );

	QPoint p = _statusLabel->pos();

	p.setY( p.y() + _statusLabel->rect().bottom() );

	_statusMenu->setWindowOpacity(0.95);

	_statusMenu->popup(mapToGlobal(p));
}

void QtStatusBar::onlineClicked(bool){

}

void QtStatusBar::dndClicked(bool){

}

void QtStatusBar::invisibleClicked(bool){

}

void QtStatusBar::brbClicked(bool){

}

void QtStatusBar::awayClicked(bool){

}

void QtStatusBar::notAvailableClicked(bool){

}

void QtStatusBar::forwardClicked(bool){

}
