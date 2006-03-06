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
#include <QtGui>
#include <QSize>

#include "QtTreeViewDelegate.h"
#include "QtUserWidgetEventFilter.h"
#include "QtUserList.h"
#include "QtContactPixmap.h"
/*
#define MENUICON_WIDTH  8
#define MENUICON_HEIGHT 8
*/
QtTreeViewDelegate::QtTreeViewDelegate(QObject *parent) : QItemDelegate(parent)
{
}
void QtTreeViewDelegate::setParent(QWidget * parent) 
{
    _parent = parent;
}
QWidget * QtTreeViewDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem & ,
                                     const QModelIndex &index) const
{
    QtUserWidget * widget  = new QtUserWidget(parent);
	QtUserList * ul = QtUserList::getInstance();
	QWidget * w = widget->findChild<QWidget *>("UserWidget");
	w->installEventFilter(new QtUserWidgetEventFilter((QObject *)this,w,ul->getUser(index.data().toString() )));
    return (QWidget *)widget;
}

void QtTreeViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QtUserWidget *widget = qobject_cast<QtUserWidget *>(editor);
    if (!widget)
        return;
    widget->setText(index.model()->data(index).toString());
}

void QtTreeViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    QtUserWidget *widget = qobject_cast<QtUserWidget *>(editor);
    if (!widget)
        return;
    model->setData(index, widget->text());
}

void QtTreeViewDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	if ( ! index.parent().isValid() )
		drawGroup(painter,option,index);
	else
	{
		QtUserList * ul = QtUserList::getInstance();
		if (ul)
		{
			ul->paintUser(painter,option,index);
		}
	}
}

QSize QtTreeViewDelegate::sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QSize orig = QItemDelegate::sizeHint(option,index);

	QtUserList * ul =  QtUserList::getInstance();
	QtUser * user = ul->getUser(index.data().toString());
	if (user)
		return QSize(orig.width(),ul->getUser(index.data().toString())->getHeight());
	else 
		if ( !index.parent().isValid())
			return (QSize(orig.width(),QtUser::UserSize));
	return orig;
}

void QtTreeViewDelegate::drawGroup( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QRect r;
	QtContactPixmap * spx;
	QPixmap px;
	int x;
	spx = QtContactPixmap::getInstance();
	painter->setPen(option.palette.text().color() );
	r = option.rect;

	QLinearGradient lg( QPointF(1,option.rect.top()), QPointF(1,option.rect.bottom() ));
	lg.setColorAt ( .8, QColor(212,208,200));
	lg.setColorAt ( 0, QColor(255, 255, 255));
	painter->fillRect(option.rect,QBrush(lg));
	
	QFont f = option.font;
	f.setBold(true);
	painter->setFont(f);
	
	if ( option.state & QStyle::State_Open)
		px = spx->getPixmap(QtContactPixmap::ContactGroupOpen);
	else
		px = spx->getPixmap(QtContactPixmap::ContactGroupClose);

	x = option.rect.left();
	//painter->drawPixmap (x,r.top(),px);
	//x+=px.width()+5;
	r.setLeft(x);
	QFont font = painter->font();
	int y = ((r.bottom()-r.top()) - QFontMetrics(font).height() ) / 2;
	r.setTop(y+r.top());
	r.setLeft(r.left()+10);
	painter->drawText(r,Qt::AlignLeft,index.data().toString(),0);
	/*painter->drawLine(option.rect.left(),option.rect.bottom(),
					  option.rect.width(),option.rect.bottom()); */
}
