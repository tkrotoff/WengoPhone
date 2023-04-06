/*
* WengoPhone, a voice over Internet phone
* Copyright (C) 2004-2005  Wengo
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

#include "ChatPageWidget.h"
#include "EmoticonsWidget.h"
#include "MyWidgetFactory.h"

#include <qwidget.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qiconset.h>
#include <qtooltip.h>
#include <qurl.h>
#include <qobjectlist.h>
#include <qcursor.h>
#include <qapplication.h>
#include <qdir.h>

#include <iostream>
using namespace std;

EmoticonsWidget::EmoticonsWidget(QObject * parent, bool useTextLabel, int nbcol)
	: QObject(parent) {
	_emoticonsWidget = MyWidgetFactory::create("EmoticonsWidgetForm.ui", this, 0);
#ifdef WIN32
	_emoticonsWidget->recreate(0, Qt::WStyle_Splash, _emoticonsWidget->pos());
#endif
	_layout = (QGridLayout*)_emoticonsWidget->layout();
	_nbcol = nbcol;
	_useTextLabel = useTextLabel;
}

void EmoticonsWidget::setIconset(Iconset iconset) {
	_iconset.clear();
	_iconset = iconset;
	update();
}

void EmoticonsWidget::setIconset(const QString & /*name*/) {
	Iconset iconset;
	iconset.load(qApp->applicationDirPath() + QDir::separator() + "emoticons" + QDir::separator() + "default");
	setIconset(iconset);
}

void EmoticonsWidget::update() {
	erase();
	QPtrListIterator<Icon> it = _iconset.iterator();
	Icon *icon;
	QPixmap pix;
	int count = 0;
	int row = 0;
	while ( (icon = it.current()) != 0 ) {
		++it;
		QPushButton * b = new QPushButton(_emoticonsWidget);
		connect(b, SIGNAL(clicked()), SLOT(clicked()));
		//b->setUsesTextLabel(true);
		b->setMaximumSize(50, 50);
		b->setFlat(true);
		QString temp = regExp2HumanReadable(icon->regExp());
		//b->setTextPosition(QToolButton::BesideIcon);
		b->setName(getFirstShortcut(icon->regExp()));
		if(_useTextLabel) {
			b->setText(getFirstShortcut(QRegExp::escape(temp)));
		}
		b->setIconSet(QIconSet(icon->pixmap()));
		QToolTip::add(b, regExp2HumanReadable(icon->regExp()));
		b->show();
		if( count >= _nbcol) {
			count = 0;
			row++;
		}
		_layout->addWidget((QWidget*)b, row, count);
		count++;
	}
	_emoticonsWidget->adjustSize();
}

QString EmoticonsWidget::regExp2HumanReadable(QRegExp regexp) {
	QString temp = regexp.pattern();
	temp.replace("\\|", "pipereplacement");
	temp.replace("\\\\", "backslashreplacement");
	temp.remove("\\");
	temp.replace("|", tr(" or "));
	temp.replace("pipereplacement", "|");
	temp.replace("backslashreplacement", "\\");
	return temp;
}

QString EmoticonsWidget::getFirstShortcut(QRegExp regexp) {
	QString temp = regExp2HumanReadable(regexp);
	int index = temp.find(tr(" or"));
	temp.remove(index, temp.length() - index);
	return temp;
}

void EmoticonsWidget::erase() {
	const QObjectList * children = ((QObject*)_emoticonsWidget)->queryList("QToolButton");
	for(QObjectList::iterator it = children->begin(); it != children->end(); it++) {
		disconnect(SIGNAL(clicked()), this, SLOT(clicked()));
		delete(*it);
	}
}

void EmoticonsWidget::clicked() {
	QPushButton * button = (QPushButton*)_emoticonsWidget->childAt(_emoticonsWidget->mapFromGlobal(_emoticonsWidget->cursor().pos()));
	if(button) {
		((ChatPageWidget*)parent())->insertImage(button->name());
		((ChatPageWidget*)parent())->giveFocusToTextEdit();
	}
}

QMimeSourceFactory *EmoticonsWidget::getMimeSourceFactory() {
	return _iconset.createMimeSourceFactory();
}
