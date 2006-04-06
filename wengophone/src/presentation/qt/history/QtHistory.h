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

#ifndef OW_QTHISTORY_H
#define OW_QTHISTORY_H

#include <presentation/PHistory.h>
#include <control/history/CHistory.h>

class QtHistoryWidget;

#include <qtutil/QObjectThreadSafe.h>

/**
 * Qt Presentation component for history.
 *
 * @author Mathieu Stute
 */
class QtHistory : public QObjectThreadSafe, public PHistory {
	Q_OBJECT
public:

	QtHistory ( CHistory & cHistory );

	void updatePresentation ();
	
	void addHistoryMemento(std::string type, std::string date,
		std::string time, int duration, std::string name, unsigned id);

	void removeHistoryMemento(int id);
	
	void clearAllEntries();
	
	void clearSmsEntries();
	
	void clearChatEntries();
	
	void clearIncomingCallEntries();
	
	void clearOutgoingCallEntries();

	void clearMissedCallEntries();
	
	void clearRejectedCallEntries();

	QWidget * getWidget();

private:
	
	void initThreadSafe();

	void updatePresentationThreadSafe();
	
	void historyLoadedEventHandler(CHistory &);
	
	void mementoAddedEventHandler(CHistory &, int id);

	void mementoUpdatedEventHandler(CHistory &, int id);

	QtHistoryWidget * _historyWidget;
	
	CHistory & _cHistory;
};

#endif	//OW_QTHISTORY_H
