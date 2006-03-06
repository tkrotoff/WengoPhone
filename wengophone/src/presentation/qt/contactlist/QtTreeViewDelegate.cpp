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
//#include <qFramePaintEventFilter.h>

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
                                     const QModelIndex &) const
{
    
    UserWidget * widget  = new UserWidget(parent);
    // widget->getFrame()->installEventFilter(new QFramePaintEventFilter( (QObject *)this) );
    return (QWidget *)widget;
}

void QtTreeViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    UserWidget *widget = qobject_cast<UserWidget *>(editor);
    if (!widget)
        return;
    widget->setText(index.model()->data(index).toString());
}

void QtTreeViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    UserWidget *widget = qobject_cast<UserWidget *>(editor);
    if (!widget)
        return;
    model->setData(index, widget->text());
}

void QtTreeViewDelegate::paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QRect r;
	bool parentItem = false;
	QtContactPixmap * spx;
	QPixmap px;
	int x;
	spx = QtContactPixmap::getInstance();
/*		
    if (option.state & QStyle::State_Selected){
        painter->fillRect(option.rect,option.palette.highlight());
        painter->setPen(option.palette.highlightedText().color() );
    }
	
    else
    {
	*/
        painter->setPen(option.palette.text().color() );
    /* }
	*/
	r = option.rect;
	if ( index.child(0,0).isValid() )
	{
        QFont f = option.font;
        f.setBold(true);
        painter->setFont(f);
		if ( option.state & QStyle::State_Open)
			px = spx->getPixmap(QtContactPixmap::ContactGroupOpen);
		else
			px = spx->getPixmap(QtContactPixmap::ContactGroupClose);
			
		x = option.rect.left();
		painter->drawPixmap (x,r.top(),px);
		x+=px.width()+5;
		r.setLeft(x);
        painter->drawText(r,Qt::AlignLeft,index.data().toString(),0);
		painter->drawLine(option.rect.left(),option.rect.bottom(),
						  option.rect.width(),option.rect.bottom());

        parentItem=true;
	}
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
	// QTreeWidget * p = (QTreeWidget *)(_parent);
	// qDebug() << option;
	//return QSize(-1,QFontMetrics(_parent->font()).height());
	return QItemDelegate::sizeHint(option,index);
}
