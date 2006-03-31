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

#include "QtHistoryWidget.h"

QtHistoryWidget::QtHistoryWidget ( QWidget * parent , Qt::WFlags f  ) : QWidget(parent,f)
{
	QGridLayout * gridLayout = new QGridLayout(this);

	_treeWidget = new QTreeWidget(this);
	_treeWidget->setRootIsDecorated ( false );
	_treeWidget->setIndentation(0);

	QStringList headerLabels;

	_header = _treeWidget->header();
	_header->setClickable ( true );

	headerLabels << tr("Type");
	headerLabels << tr("Date");
	headerLabels << tr("Duration");
	headerLabels << tr("Name/ID");

	_treeWidget->setHeaderLabels(headerLabels);
	_treeWidget->setSortingEnabled ( false );

	_header->resizeSection ( 0,200);

	gridLayout->addWidget(_treeWidget,0,0);

	_menu = new QMenu();

	QAction * action;

	action = _menu->addAction( tr ("Outgoing SMS") );
	connect (action, SIGNAL( triggered(bool) ),SLOT( showSMSCall(bool) ));

	action = _menu->addAction( tr ("Outgoing call") );
	connect (action, SIGNAL( triggered(bool) ),SLOT( showOutGoingCall(bool) ));

	action = _menu->addAction( tr ("Missed call") );
	connect (action, SIGNAL( triggered(bool) ),SLOT( showMissedCall (bool) ));

	action = _menu->addAction( tr ("Incoming call") );
	connect (action, SIGNAL( triggered(bool) ),SLOT( showIncoming(bool) ));

	action = _menu->addAction( tr ("Chat") );
	connect (action, SIGNAL( triggered(bool) ),SLOT( showChat(bool) ));

	action = _menu->addAction( tr ("All") );
	connect (action, SIGNAL( triggered(bool) ),SLOT( showAll(bool) ));

	connect (_treeWidget, SIGNAL( itemDoubleClicked ( QTreeWidgetItem *, int ) ),SLOT (itemDoubleClicked ( QTreeWidgetItem *, int) ) );

	connect (_header,SIGNAL(sectionClicked (int)),SLOT(headerClicked(int)) );
}

void QtHistoryWidget::addSMSItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, int id) {

	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText(0,text);
	item->setText(1,date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText(2,name);
	item->setId(id);
	item->setItemType(QtHistoryItem::Sms);

}


void QtHistoryWidget::addOutGoingCallItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, int id) {

	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText( 0, text );
	item->setText( 1, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText( 2, duration.toString(Qt::TextDate) );
	item->setText( 3, name);
	item->setId( id );
	item->setItemType( QtHistoryItem::OutGoingCall);
}

void QtHistoryWidget::addIncomingCallItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, int id){

	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText( 0, text );
	item->setText( 1, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText( 2, duration.toString(Qt::TextDate) );
	item->setText( 3, name);
	item->setId( id );
	item->setItemType(QtHistoryItem::IncomingCall);
}

void QtHistoryWidget::addChatItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, int id) {

	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText( 0, text );
	item->setText( 1, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText( 2, duration.toString(Qt::TextDate) );
	item->setText( 3, name);
	item->setId( id );
	item->setItemType(QtHistoryItem::Chat);
}

void QtHistoryWidget::addMissedCallItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, int id) {
	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText( 0, text );
	item->setText( 1, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText( 2, duration.toString(Qt::TextDate) );
	item->setText( 3, name);
	item->setId( id );
	item->setItemType(QtHistoryItem::MissedCall);
}


void QtHistoryWidget::addRejectedCallItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, int id) {
	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText( 0, text );
	item->setText( 1, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText( 2, duration.toString(Qt::TextDate) );
	item->setText( 3, name);
	item->setId( id );
	item->setItemType(QtHistoryItem::RejectedCall);
}

void QtHistoryWidget::editItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, int id) {

	QList<QTreeWidgetItem *>  itemList = _treeWidget->findItems("*", Qt::MatchWildcard);

	QList<QTreeWidgetItem *>::iterator iter;

	for ( iter = itemList.begin(); iter != itemList.end(); iter++){
		QtHistoryItem * item = dynamic_cast<QtHistoryItem *> ( (*iter) );
		if ( item->getId() == id ){
			item->setText( 0, text );
			item->setText( 1, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
			item->setText( 2, duration.toString(Qt::TextDate) );
			item->setText( 3, name);
			break;
		}
	}
	_treeWidget->viewport()->update();
}

void QtHistoryWidget::itemDoubleClicked ( QTreeWidgetItem * item, int column ){

	QtHistoryItem * hItem = dynamic_cast<QtHistoryItem *>(item);

	switch ( hItem->getItemType() )
	{
		case QtHistoryItem::Sms:
			break;

		case QtHistoryItem::OutGoingCall:
			break;

		case QtHistoryItem::IncomingCall:
			break;

		case QtHistoryItem::MissedCall:
			break;

		case QtHistoryItem::Chat:
			break;

		default:
			break;
	}
}

void QtHistoryWidget::headerClicked(int logicalIndex){
	if ( !logicalIndex == 0)
		return;
	QCursor cursor;
	_menu->popup(cursor.pos());

}

void QtHistoryWidget::showSMSCall(bool ){
	QAbstractItemModel * model = _header->model();
	model->setHeaderData(0,Qt::Horizontal,QVariant( tr("SMS") ),Qt::DisplayRole);

}
void QtHistoryWidget::showOutGoingCall(bool){
	QAbstractItemModel * model = _header->model();
	model->setHeaderData(0,Qt::Horizontal,QVariant( tr("Outgoing call") ),Qt::DisplayRole);
}
void QtHistoryWidget::showIncoming(bool){
	QAbstractItemModel * model = _header->model();
	model->setHeaderData(0,Qt::Horizontal,QVariant( tr("Incoming call") ),Qt::DisplayRole);
}
void QtHistoryWidget::showChat(bool){
	QAbstractItemModel * model = _header->model();
	model->setHeaderData(0,Qt::Horizontal,QVariant( tr("Chat") ),Qt::DisplayRole);
}
void QtHistoryWidget::showAll(bool){
	QAbstractItemModel * model = _header->model();
	model->setHeaderData(0,Qt::Horizontal,QVariant( tr("All") ),Qt::DisplayRole);
}
void QtHistoryWidget::showMissedCall(bool){
	QAbstractItemModel * model = _header->model();
	model->setHeaderData(0,Qt::Horizontal,QVariant( tr("Missed call") ),Qt::DisplayRole);
}
void QtHistoryWidget::clearHistory(){
	_treeWidget->clear();
}
