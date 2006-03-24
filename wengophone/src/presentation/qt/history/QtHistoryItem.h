#ifndef QTHISTORYITEM_H
#define QTHISTORYITEM_H

#include <QtGui>

class QtHistoryItem : public QTreeWidgetItem
{


	public:

		enum HistoryType{ Sms, OutGoingCall, IncomingCall,MissedCall, Chat };

		QtHistoryItem(int type = Type);
		QtHistoryItem ( const QStringList & strings, int type = Type );
		QtHistoryItem ( QTreeWidget * parent, int type = Type );
		QtHistoryItem ( QTreeWidget * parent, const QStringList & strings, int type = Type );
		QtHistoryItem ( QTreeWidget * parent, QtHistoryItem * preceding, int type = Type );
		QtHistoryItem ( QtHistoryItem * parent, int type = Type );
		QtHistoryItem ( QtHistoryItem * parent, const QStringList & strings, int type = Type );
		QtHistoryItem ( QtHistoryItem * parent, QtHistoryItem * preceding, int type = Type );
		QtHistoryItem ( const QtHistoryItem & other );
		virtual ~QtHistoryItem();

		int getId() const;

		void setId(int id);

		void setItemType(QtHistoryItem::HistoryType type);

		QtHistoryItem::HistoryType getItemType() const;



	protected:

		int _id;
		HistoryType _type;

	private:
};

#endif // QTHISTORYITEM_H
