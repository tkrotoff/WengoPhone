/*
* WengoPhone, a voice over Internet phone
* Copyright (C) 2004-2006  Wengo
 *
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
//#include "QtProfileLabel.h"
#include "QtCallBar.h"

#include <qtutil/QtWengoStyleLabel.h>

QtCallBar::QtCallBar(QWidget * parent , Qt::WFlags f) :
QFrame(parent,f)
{
	setFrameShape(QFrame::NoFrame);
	_callBarButton = new QtWengoStyleLabel(this);
	_callBarButton->setPixmaps(
	                  QPixmap(), // Start
					  QPixmap(":/pics/callbar/call_bar_button.png"), // End
					  QPixmap(), // Fill

					  QPixmap(), // Start
					  QPixmap(":/pics/callbar/call_bar_button.png"), // End
					  QPixmap() // Fill
					  );

	_callBarButtonVideo= new QtWengoStyleLabel(this);
	_callBarButtonVideo->setPixmaps(
	                  QPixmap(":/pics/callbar/call_bar_button_video.png"), // Start
					  QPixmap(), // End
					  QPixmap(), // Fill
					  QPixmap(":/pics/callbar/call_bar_button_video.png"), // Start
					  QPixmap(), // End
					  QPixmap() // Fill
					  );

	_callBarButtonOff = new QtWengoStyleLabel (this);
	_callBarButtonOff->setPixmaps(
					  QPixmap(":/pics/callbar/call_bar_button_off.png"), // Start
					  QPixmap(), // End
					  QPixmap(), // Fill
					  QPixmap(":/pics/callbar/call_bar_button_off.png"), // Start
					  QPixmap(), // End
					  QPixmap() // Fill
					  );

	_callBarComboContainer = new QtWengoStyleLabel (this);
	_callBarComboContainer->setPixmaps(
					  QPixmap(), // Start
					  QPixmap(), // End
					  QPixmap(":/pics/callbar/call_bar_fill.png"), // Fill
					  QPixmap(), // Start
					  QPixmap(), // End
					  QPixmap() // Fill
					  );



	QGridLayout * comboContLayout = new QGridLayout(_callBarComboContainer);
	_phoneComboBox = new QComboBox(_callBarComboContainer);
	_phoneComboBox->setEditable(true);
	_phoneComboBox->setMaximumSize(QSize(10000,22));
	comboContLayout->addWidget(_phoneComboBox);

	QGridLayout *  layout = new QGridLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	layout->addWidget(_callBarComboContainer,0,0);
	layout->addWidget(_callBarButtonVideo,0,1);
	layout->addWidget(_callBarButtonOff,0,2);
	layout->addWidget(_callBarButton,0,3);

	_callBarButton->setMaximumSize(QSize(45,65));
	_callBarButtonOff->setMaximumSize(QSize(20,65));
	_callBarButtonVideo->setMaximumSize(QSize(27,65));

	connect (_callBarButton, SIGNAL(clicked()),SLOT(callBarButtonClicked()));
	connect (_callBarButtonOff, SIGNAL(clicked()),SLOT(callBarButtonOffClicked()));
	connect (_callBarButtonVideo, SIGNAL(clicked()),SLOT(callBarButtonVideoClicked()));

}
void QtCallBar::callBarButtonVideoClicked(){
	VideoClicked();
}
void QtCallBar::callBarButtonOffClicked(){
	OffClicked();
}
void QtCallBar::callBarButtonClicked(){
	ButtonClicked();
}
QComboBox * QtCallBar::getComboBox(){
	return _phoneComboBox;

}
