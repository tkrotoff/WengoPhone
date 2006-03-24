#ifndef QTHISTORYWIDGET_H
#define QTHISTORYWIDGET_H

#include <QtGui>

#include "QtHistoryItem.h"

class QtHistoryWidget : public QWidget {

		Q_OBJECT

public:


	QtHistoryWidget ( QWidget * parent = 0, Qt::WFlags f = 0 );

	void addSMSItem(const QString & text,const QDate & date, const QString & name, int id);

	void addOutGoingCallItem(const QString & text,const QDate & date, const QString & name, int id);

	void addIncomingCallItem(const QString & text,const QDate & date, const QString & name, int id);

	void addChatItem(const QString & text,const QDate & date, const QString & name, int id);

public Q_SLOTS:

	void showSMSCall(bool checked);

	void showOutGoingCall(bool checked);

	void showIncoming(bool checked);

	void showChat(bool checked);

	void showMissedCall(bool checked);

	void showAll(bool checked);

	void itemDoubleClicked ( QTreeWidgetItem * item, int column );

	void headerClicked(int logicalIndex);

protected:

	QTreeWidget * _treeWidget;

	QHeaderView * _header;

	QMenu * _menu;
};

#endif
