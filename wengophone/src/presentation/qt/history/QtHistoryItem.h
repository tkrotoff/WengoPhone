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

#ifndef QTHISTORYITEM_H
#define QTHISTORYITEM_H

#include <QtGui>

class QtHistoryItem : public QTreeWidgetItem
{


	public:

		enum HistoryType{ Sms, OutGoingCall, IncomingCall,MissedCall, RejectedCall, Chat,  };

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
