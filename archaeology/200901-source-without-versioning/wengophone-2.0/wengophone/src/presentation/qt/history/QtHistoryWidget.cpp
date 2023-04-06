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
#include "QtHistoryItem.h"

#include "ui_HistoryWidget.h"

#include <model/history/HistoryMemento.h>
#include <control/history/CHistory.h>
#include <presentation/qt/QtToolBar.h>

#include <qtutil/SafeConnect.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <QtGui/QtGui>

HistoryTreeEventManager::HistoryTreeEventManager(QTreeWidget * target, QtHistoryWidget * historyWidget)
	: QObject(target),
	_historyWidget(historyWidget) {

	target->viewport()->installEventFilter(this);
}

bool HistoryTreeEventManager::eventFilter(QObject * object, QEvent * event) {
	if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent * mouseEvent = (QMouseEvent *) event;

		if (mouseEvent->button() == Qt::RightButton) {
			_historyWidget->showPopupMenu(mouseEvent->pos());
			return QObject::eventFilter(object, event);
		}
	}
	return QObject::eventFilter(object, event);
}


QtHistoryWidget::QtHistoryWidget(QWidget * parent)
	: QObject(parent) {

	_historyWidget = new QWidget(parent);

	_ui = new Ui::HistoryWidget();
	_ui->setupUi(_historyWidget);

#if defined(OS_MACOSX)
	_historyWidget->layout()->setMargin(10);
#endif

	//treeWidget
	SAFE_CONNECT(_ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));
	SAFE_CONNECT(_ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *,int)), SLOT(itemClicked()));

	//header
	_ui->treeWidget->header()->setClickable(true);
	_ui->treeWidget->setSortingEnabled(false);
	_ui->treeWidget->header()->resizeSection(0, 150);
	_ui->treeWidget->header()->setStretchLastSection(true);
	SAFE_CONNECT(_ui->treeWidget->header(), SIGNAL(sectionClicked(int)), SLOT(headerClicked(int)));

	_historyTreeEventManager = new HistoryTreeEventManager(_ui->treeWidget, this);


	_menu = new QMenu();

	QAction * action = _menu->addAction(tr("Outgoing SMS"));
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(showSMSCall(bool)));

	action = _menu->addAction(tr("Outgoing call"));
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(showOutGoingCall(bool)));

	action = _menu->addAction(tr("Missed call"));
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(showMissedCall(bool)));

	action = _menu->addAction(tr("Incoming call"));
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(showIncomingCall(bool)));

	action = _menu->addAction(tr("Rejected call"));
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(showRejectedCall(bool)));

	// TODO: uncomment when chat will be historized
	/*
	action = _menu->addAction(tr("Chat"));
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(showChat(bool)));
	*/

	action = _menu->addAction(tr("All"));
	SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(showAll(bool)));

	showAll(true);

	_popupMenu = new QMenu();
}

QtHistoryWidget::~QtHistoryWidget() {
	OWSAFE_DELETE(_popupMenu);
	OWSAFE_DELETE(_menu);
	OWSAFE_DELETE(_historyTreeEventManager);
	OWSAFE_DELETE(_ui);
	OWSAFE_DELETE(_historyWidget);
}

QWidget * QtHistoryWidget::getWidget() const {
	return _historyWidget;
}

void QtHistoryWidget::addItem(const QString & text, const QIcon & icon, const QDate & date, const QTime & time,
	const QTime & duration, const QString & name, unsigned id, QtHistoryItem::HistoryType type) {

	QtHistoryItem * item = new QtHistoryItem(_ui->treeWidget);

	item->setText(QtHistoryItem::COLUMN_TYPE, text);
	item->setIcon(QtHistoryItem::COLUMN_TYPE, icon);
	item->setText(QtHistoryItem::COLUMN_DATE, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)));
	item->setText(QtHistoryItem::COLUMN_DURATION, duration.toString(Qt::TextDate));
	item->setText(QtHistoryItem::COLUMN_PEERS, name);
	item->setId(id);
	item->setItemType(type);
	resizeColumns();
	sortHistory();
}

void QtHistoryWidget::addSMSItem(const QString & text, const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned id) {
	addItem(text, QIcon(QPixmap(":/pics/history/sms_sent.png")), date, time, duration, name, id, QtHistoryItem::Sms);
}

void QtHistoryWidget::addOutGoingCallItem(const QString & text, const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned id) {
	addItem(text, QIcon(QPixmap(":/pics/history/call_outgoing.png")), date, time, duration, name, id, QtHistoryItem::OutGoingCall);
}

void QtHistoryWidget::addIncomingCallItem(const QString & text, const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned id) {
	addItem(text, QIcon(QPixmap(":/pics/history/call_incoming.png")), date, time, duration, name, id, QtHistoryItem::IncomingCall);
}

void QtHistoryWidget::addChatItem(const QString & text, const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned id) {
	addItem(text, QIcon(QPixmap(":/pics/history/chat.png")), date, time, duration, name, id, QtHistoryItem::Chat);
}

void QtHistoryWidget::addMissedCallItem(const QString & text, const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned id) {
	addItem(text, QIcon(QPixmap(":/pics/history/call_missed.png")), date, time, duration, name, id, QtHistoryItem::MissedCall);
}

void QtHistoryWidget::addRejectedCallItem(const QString & text, const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned id) {
	addItem(text, QIcon(QPixmap(":/pics/history/call_missed.png")), date, time, duration, name, id, QtHistoryItem::RejectedCall);
}

void QtHistoryWidget::editItem(const QString & text, const QDate & date, const QTime & time, const QTime & duration, const QString & name, unsigned id) {

	QList<QTreeWidgetItem *> itemList = _ui->treeWidget->findItems("*", Qt::MatchWildcard);

	QList<QTreeWidgetItem *>::iterator it;

	for (it = itemList.begin(); it != itemList.end(); it++) {
		QtHistoryItem * item = (QtHistoryItem *) *it;
		if (item->getId() == id) {
			item->setText(QtHistoryItem::COLUMN_TYPE, text);
			item->setText(QtHistoryItem::COLUMN_DATE, date.toString("yyyy-MM-dd") + QString(" %1").arg(time.toString(Qt::TextDate)));
			item->setText(QtHistoryItem::COLUMN_DURATION, duration.toString(Qt::TextDate));
			item->setText(QtHistoryItem::COLUMN_PEERS, name);
			resizeColumns();
			break;
		}
	}
	_ui->treeWidget->viewport()->update();
}

void QtHistoryWidget::itemDoubleClicked(QTreeWidgetItem * item, int column) {
	QtHistoryItem * hItem = (QtHistoryItem *) item;
	replayItem(hItem);
}

void QtHistoryWidget::headerClicked(int logicalIndex) {
	if (!logicalIndex == 0) {
		return;
	}
	QCursor cursor;
	_menu->popup(cursor.pos());
}

void QtHistoryWidget::showItem(const QString & text) {
	QAbstractItemModel * model = _ui->treeWidget->header()->model();
	model->setHeaderData(0, Qt::Horizontal, QVariant(text), Qt::DisplayRole);
}


void QtHistoryWidget::showSMSCall(bool) {
	showItem(tr("SMS"));
	showOnlyItemOfType((int)HistoryMemento::OutgoingSmsOk);
}

void QtHistoryWidget::showOutGoingCall(bool) {
	showItem(tr("Outgoing call"));
	showOnlyItemOfType((int)HistoryMemento::OutgoingCall);
}

void QtHistoryWidget::showIncomingCall(bool) {
	showItem(tr("Incoming call"));
	showOnlyItemOfType((int)HistoryMemento::IncomingCall);
}

void QtHistoryWidget::showChat(bool) {
	showItem(tr("Chat"));
	showOnlyItemOfType((int)HistoryMemento::ChatSession);
}

void QtHistoryWidget::showMissedCall(bool) {
	showItem(tr("Missed call"));
	showOnlyItemOfType((int)HistoryMemento::MissedCall);
}

void QtHistoryWidget::showRejectedCall(bool checked) {
	showItem(tr("Rejected call"));
	showOnlyItemOfType((int)HistoryMemento::RejectedCall);
}

void QtHistoryWidget::showAll(bool) {
	showItem(tr("All"));
	showOnlyItemOfType((int)HistoryMemento::Any);
}

void QtHistoryWidget::clearHistory() {
	_ui->treeWidget->clear();
}

void QtHistoryWidget::showPopupMenu(const QPoint & pos) {

	QTreeWidgetItem * item = _ui->treeWidget->itemAt(pos);
	QtHistoryItem * hItem = (QtHistoryItem *) item;

	_popupMenu->clear();
	if (hItem) {
		_currentItem = hItem;

		QAction * action = _popupMenu->addAction(QIcon(":/pics/actions/delete.png"), tr("Erase this entry"));
		SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(eraseEntry()));

		switch (_currentItem->getItemType()) {
			case QtHistoryItem::Sms:
				action = _popupMenu->addAction(QIcon(":/pics/actions/send-sms-16.png"), tr("Open in SMS window"));
				break;
			case QtHistoryItem::OutGoingCall:
			case QtHistoryItem::IncomingCall:
			case QtHistoryItem::MissedCall:
			case QtHistoryItem::RejectedCall:
				action = _popupMenu->addAction(QIcon(":/pics/actions/accept-phone.png"), tr("Call this peer"));
				break;
			case QtHistoryItem::Chat:
			default:
				break;
		}

		SAFE_CONNECT(action, SIGNAL(triggered(bool)), SLOT(replayEntry()));

		QCursor cursor;
		_popupMenu->popup(cursor.pos());

	} else if (_qtToolBar) {

		QAction * action = _popupMenu->addAction(tr("Clear Outgoing Calls"));
		SAFE_CONNECT_RECEIVER(action, SIGNAL(triggered()), _qtToolBar, SLOT(clearHistoryOutgoingCalls()));
		action = _popupMenu->addAction(tr("Clear Incoming Calls"));
		SAFE_CONNECT_RECEIVER(action, SIGNAL(triggered()), _qtToolBar, SLOT(clearHistoryIncomingCalls()));
		action = _popupMenu->addAction(tr("Clear Missed Calls"));
		SAFE_CONNECT_RECEIVER(action, SIGNAL(triggered()), _qtToolBar, SLOT(clearHistoryMissedCalls()));
		action = _popupMenu->addAction(tr("Clear Rejected Calls"));
		SAFE_CONNECT_RECEIVER(action, SIGNAL(triggered()), _qtToolBar, SLOT(clearHistoryRejectedCalls()));
		//action = _popupMenu->addAction(tr("Clear Chat Session"));
		//SAFE_CONNECT_RECEIVER(action, SIGNAL(triggered()), _qtToolBar, SLOT(clearHistoryChatSessions()));
		action = _popupMenu->addAction(tr("Clear SMS"));
		SAFE_CONNECT_RECEIVER(action, SIGNAL(triggered()), _qtToolBar, SLOT(clearHistorySms()));
		action = _popupMenu->addAction(tr("Clear All"));
		SAFE_CONNECT_RECEIVER(action, SIGNAL(triggered()), _qtToolBar, SLOT(clearHistoryAll()));

		QCursor cursor;
		_popupMenu->popup(cursor.pos());
	}
}

void QtHistoryWidget::eraseEntry() {
	if (_currentItem) {
		removeItem(_currentItem->getId());
	}
}

void QtHistoryWidget::replayEntry() {
	if (_currentItem) {
		replayItem(_currentItem);
	}
}

void QtHistoryWidget::sortHistory() {
	_ui->treeWidget->sortItems(QtHistoryItem::COLUMN_DATE, Qt::DescendingOrder);
}

void QtHistoryWidget::itemClicked() {
	missedCallsSeen();
}

void QtHistoryWidget::resizeColumns() {
	_ui->treeWidget->resizeColumnToContents(QtHistoryItem::COLUMN_TYPE);
	_ui->treeWidget->resizeColumnToContents(QtHistoryItem::COLUMN_DATE);
	_ui->treeWidget->resizeColumnToContents(QtHistoryItem::COLUMN_DURATION);
	_ui->treeWidget->resizeColumnToContents(QtHistoryItem::COLUMN_PEERS);
}
