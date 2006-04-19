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

#include "QtTreeViewDelegate.h"

#include "QtUserWidgetEventFilter.h"
#include "QtUserWidget.h"
#include "QtUserList.h"
#include "QtContactPixmap.h"

#include <util/Logger.h>
#include <util/StringList.h>

#include <qtutil/Object.h>

#include <QtGui>
#include <QSize>

/*
#define MENUICON_WIDTH  8
#define MENUICON_HEIGHT 8
*/

/*
static int USER_TITLE_FRAME_HEIGHT = 0;
static int USER_WIDGET_FRAME_HEIGHT = 0;
*/
static int GROUP_WIDGET_FRAME_HEIGHT = 22;

QtTreeViewDelegate::QtTreeViewDelegate( QObject *parent ) : QItemDelegate( parent ) {
/*
	QtUserWidget * widget = new QtUserWidget( NULL );
	QWidget * userWidget = Object::findChild<QWidget *>( widget, "UserWidget" );
	QFrame * userTitleFrame = Object::findChild<QFrame *>( userWidget, "userTitleFrame" );
*/
//	USER_TITLE_FRAME_HEIGHT = userTitleFrame->height();
//	USER_WIDGET_FRAME_HEIGHT = userWidget->height();
}

void QtTreeViewDelegate::setParent( QWidget * parent ) {
	_parent = parent;
}

QWidget * QtTreeViewDelegate::createEditor( QWidget *parent,
	const QStyleOptionViewItem & ,
	const QModelIndex &index ) const {

	// USER_TITLE_FRAME_HEIGHT = userTitleFrame->height();
	//LOG_DEBUG( "height= " + String::fromNumber( USER_TITLE_FRAME_HEIGHT ) );

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user = ul->getUser(index.data().toString());

	QtUserWidget * widget = new QtUserWidget(user->getCContact(), parent);
	QWidget * userWidget = Object::findChild<QWidget *>(widget, "UserWidget");
	QFrame * userTitleFrame = Object::findChild<QFrame *>(userWidget, "userTitleFrame");

	userWidget->installEventFilter(new QtUserWidgetEventFilter((QObject *) this, userWidget, user));

	//widget->setAvatar("emoticons/cat.svg");

	return ( QWidget * ) widget;
}

void QtTreeViewDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const {
		QtUserWidget * widget = qobject_cast<QtUserWidget *>( editor );

		if ( !widget ) {
				return ;
			}

		widget->setText( index.model() ->data( index ).toString() );
	}

void QtTreeViewDelegate::setModelData( QWidget *editor, QAbstractItemModel *model,
									   const QModelIndex &index ) const {
		QtUserWidget * widget = qobject_cast<QtUserWidget *>( editor );

		if ( !widget )
			return ;

		model->setData( index, widget->text() );
	}

void QtTreeViewDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const {
		if ( ! index.parent().isValid() )
			drawGroup( painter, option, index );
		else {
				QtUserList * ul = QtUserList::getInstance();

				if ( ul ) {
						ul->paintUser( painter, option, index );
					}
			}
	}

QSize QtTreeViewDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const {
		// LOG_DEBUG( "height= " + String::fromNumber( USER_TITLE_FRAME_HEIGHT ) );

		QSize orig = QItemDelegate::sizeHint( option, index );

		QtUserList * ul = QtUserList::getInstance();
		QtUser * user = ul->getUser( index.data().toString() );

		if ( user ) {
				return QSize( orig.width(), ul->getUser( index.data().toString() ) ->getHeight() );
/*
				if ( user->isOpen() ) {
						return QSize( orig.width(), USER_WIDGET_FRAME_HEIGHT );
					}

				else {
						return QSize( orig.width(), USER_TITLE_FRAME_HEIGHT );
					}
*/
			}

		else {
				if ( !index.parent().isValid() ) {
						return ( QSize( orig.width(), GROUP_WIDGET_FRAME_HEIGHT ) );
					}
			}

		return orig;
	}

void QtTreeViewDelegate::drawGroup( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const {
		QRect r;
		QtContactPixmap * spx;
		QPixmap px;
		int x;
		spx = QtContactPixmap::getInstance();
		painter->setPen( option.palette.text().color() );
		r = option.rect;
/*
		QLinearGradient lg( QPointF( 1, option.rect.top() ), QPointF( 1, option.rect.bottom() ) );
		lg.setColorAt ( .8, QColor( 212, 208, 200 ) );
		lg.setColorAt ( 0, QColor( 255, 255, 255 ) );
		painter->fillRect( option.rect, QBrush( lg ) );
*/
		QFont f = option.font;
		f.setBold( true );
		painter->setFont( f );

		if ( option.state & QStyle::State_Open )
			px = spx->getPixmap( QtContactPixmap::ContactGroupOpen );
		else
			px = spx->getPixmap( QtContactPixmap::ContactGroupClose );

		x = option.rect.left();

		painter->drawPixmap (x,r.top()+5,px);
		x+=px.width()+3;
		r.setLeft( x );

		QFont font = painter->font();

		int y = ( ( r.bottom() - r.top() ) - QFontMetrics( font ).height() ) / 2;

		r.setTop( y + r.top() );

		r.setLeft( r.left() + 10 );

		// Number of child

		int nbchild = index.model()->rowCount(index);

		QString str= QString("%1 (%2)").arg(index.data().toString()).arg(nbchild);

		// painter->drawText( r, Qt::AlignLeft, index.data().toString(), 0 );
		painter->drawText( r, Qt::AlignLeft, str , 0 );

		painter->setPen(QColor(210,210,210));
		painter->drawLine(option.rect.left(),option.rect.bottom(),
						  option.rect.width(),option.rect.bottom());

		painter->setPen(QColor(128,128,128));
		painter->drawLine(option.rect.left(),option.rect.bottom()-1,
						  option.rect.width(),option.rect.bottom()-1);

		painter->setPen(Qt::black);
		painter->drawLine(option.rect.left(),option.rect.bottom()-2,
						  option.rect.width(),option.rect.bottom()-2);

	}
