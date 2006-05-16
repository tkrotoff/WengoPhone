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

#include <util/Logger.h>

HistoryTreeEventManager::HistoryTreeEventManager(QTreeWidget * target, QtHistoryWidget * historyWidget)
	: QObject(target), _historyWidget(historyWidget) {
	target->viewport()->installEventFilter(this);
}

bool HistoryTreeEventManager::eventFilter(QObject *obj, QEvent *event) {
	if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);

		if( mouseEvent->button() == Qt::RightButton) {
			_historyWidget->showPopupMenu(mouseEvent->pos());
			return QObject::eventFilter(obj, event);
		}
	}
	return QObject::eventFilter(obj, event);
}

QtHistoryWidget::QtHistoryWidget ( QWidget * parent , Qt::WFlags f ) : QWidget(parent,f)
{
	QGridLayout * gridLayout = new QGridLayout(this);

	_treeWidget = new QTreeWidget(this);
	_treeWidget->setRootIsDecorated ( false );
	_treeWidget->setIndentation(0);

	_historyTreeEventManager = new HistoryTreeEventManager(_treeWidget, this);

	QStringList headerLabels;

	_header = _treeWidget->header();
	_header->setClickable(true);

	headerLabels << tr("Type");
	headerLabels << tr("Name/ID");
	headerLabels << tr("Date");
	headerLabels << tr("Duration");

	_treeWidget->setHeaderLabels(headerLabels);
	_treeWidget->setSortingEnabled ( false );

	_header->resizeSection (0, 150);
	_header->setStretchLastSection(true);

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

	connect(_treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *,int)), SLOT(itemClicked()));
	
	connect (_header,SIGNAL(sectionClicked (int)),SLOT(headerClicked(int)) );


	_popupMenu = new QMenu();

	action = _popupMenu->addAction( tr ("Erase entry") );
	connect (action, SIGNAL( triggered(bool) ),SLOT( eraseEntry() ));

	action = _popupMenu->addAction( tr ("Replay entry") );
	connect (action, SIGNAL( triggered(bool) ),SLOT( replayEntry() ));
}

QtHistoryWidget::~QtHistoryWidget() {
	delete _historyTreeEventManager;
}

void QtHistoryWidget::addSMSItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned int id) {

	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText(QtHistoryItem::COLUMN_TYPE, text);
	item->setIcon(QtHistoryItem::COLUMN_TYPE, QIcon(QPixmap(":/pics/history/sms_send.png")));
	item->setText(QtHistoryItem::COLUMN_DATE, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText(QtHistoryItem::COLUMN_DURATION, duration.toString(Qt::TextDate) );
	item->setText(QtHistoryItem::COLUMN_PEERS, name);
	item->setId(id);
	item->setItemType(QtHistoryItem::Sms);

	sortHistory();
}


void QtHistoryWidget::addOutGoingCallItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned int id) {

	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText(QtHistoryItem::COLUMN_TYPE, text );
	item->setIcon(QtHistoryItem::COLUMN_TYPE, QIcon(QPixmap(":/pics/history/call_outgoing.png")));
	item->setText(QtHistoryItem::COLUMN_DATE, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText(QtHistoryItem::COLUMN_DURATION, duration.toString(Qt::TextDate) );
	item->setText(QtHistoryItem::COLUMN_PEERS, name);
	item->setId( id );
	item->setItemType(QtHistoryItem::OutGoingCall);

	sortHistory();
}

void QtHistoryWidget::addIncomingCallItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned int id){

	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText(QtHistoryItem::COLUMN_TYPE, text );
	item->setIcon(QtHistoryItem::COLUMN_TYPE, QIcon(QPixmap(":/pics/history/call_incoming.png")));
	item->setText(QtHistoryItem::COLUMN_DATE, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText(QtHistoryItem::COLUMN_DURATION, duration.toString(Qt::TextDate) );
	item->setText(QtHistoryItem::COLUMN_PEERS, name);
	item->setId( id );
	item->setItemType(QtHistoryItem::IncomingCall);

	sortHistory();
}

void QtHistoryWidget::addChatItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned int id) {

	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText(QtHistoryItem::COLUMN_TYPE, text );
	item->setText(QtHistoryItem::COLUMN_DATE, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText(QtHistoryItem::COLUMN_DURATION, duration.toString(Qt::TextDate) );
	item->setText(QtHistoryItem::COLUMN_PEERS, name);
	item->setId( id );
	item->setItemType(QtHistoryItem::Chat);

	sortHistory();
}

void QtHistoryWidget::addMissedCallItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned int id) {
	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText(QtHistoryItem::COLUMN_TYPE, text );
	item->setIcon(QtHistoryItem::COLUMN_TYPE, QIcon(QPixmap(":/pics/history/call_missed.png")));
	item->setText(QtHistoryItem::COLUMN_DATE, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText(QtHistoryItem::COLUMN_DURATION, duration.toString(Qt::TextDate) );
	item->setText(QtHistoryItem::COLUMN_PEERS, name);
	item->setId( id );
	item->setItemType(QtHistoryItem::MissedCall);

	sortHistory();
}


void QtHistoryWidget::addRejectedCallItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, int unsigned id) {
	QtHistoryItem * item = new QtHistoryItem(_treeWidget);

	item->setText(QtHistoryItem::COLUMN_TYPE, text );
	item->setIcon(QtHistoryItem::COLUMN_TYPE, QIcon(QPixmap(":/pics/history/call_missed.png")));
	item->setText(QtHistoryItem::COLUMN_DATE, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
	item->setText(QtHistoryItem::COLUMN_DURATION, duration.toString(Qt::TextDate) );
	item->setText(QtHistoryItem::COLUMN_PEERS, name);
	item->setId( id );
	item->setItemType(QtHistoryItem::RejectedCall);

	sortHistory();
}

void QtHistoryWidget::editItem(const QString & text,const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned int id) {

	QList<QTreeWidgetItem *>  itemList = _treeWidget->findItems("*", Qt::MatchWildcard);

	QList<QTreeWidgetItem *>::iterator iter;

	for ( iter = itemList.begin(); iter != itemList.end(); iter++){
		QtHistoryItem * item = dynamic_cast<QtHistoryItem *> ( (*iter) );
		if ( item->getId() == id ){
			item->setText(QtHistoryItem::COLUMN_TYPE, text );
			item->setText(QtHistoryItem::COLUMN_DATE, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)) );
			item->setText(QtHistoryItem::COLUMN_DURATION, duration.toString(Qt::TextDate) );
			item->setText(QtHistoryItem::COLUMN_PEERS, name);
			break;
		}
	}
	_treeWidget->viewport()->update();
}

void QtHistoryWidget::itemDoubleClicked ( QTreeWidgetItem * item, int column ){
	QtHistoryItem * hItem = dynamic_cast<QtHistoryItem *>(item);
	replayItem(hItem);
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
void QtHistoryWidget::clearHistory() {
	_treeWidget->clear();
}

void QtHistoryWidget::showPopupMenu(const QPoint & pos) {
	QTreeWidgetItem * item = _treeWidget->itemAt(pos);
	QtHistoryItem * hItem = dynamic_cast<QtHistoryItem *>(item);
	if( hItem ) {
		_currentItem = hItem;
		QCursor cursor;
		_popupMenu->popup(cursor.pos());
	}
}

void QtHistoryWidget::eraseEntry() {
	if( _currentItem ) {
		removeItem(_currentItem->getId());
	}
}

void QtHistoryWidget::replayEntry() {
	if( _currentItem ) {
		replayItem(_currentItem);
	}
}

void QtHistoryWidget::sortHistory() {
	_treeWidget->sortItems(QtHistoryItem::COLUMN_DATE, Qt::DescendingOrder);
}

void QtHistoryWidget::itemClicked() {
	missedCallsSeen();
}
