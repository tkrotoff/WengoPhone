#ifndef QTEVENTWIDGET_H
#define QTEVENTWIDGET_H

#include <QtGui>
#include "QtClickableLabel.h"


class QtEventWidget : public QWidget
{
	Q_OBJECT

public:

	typedef QList<QtClickableLabel *> QtClickableLabelList;

	QtEventWidget (QWidget * parent = 0, Qt::WFlags f = 0);



protected:

	QtClickableLabel * _missedCallLabel;

	QtClickableLabel * _newMessagesLabel;

	QtClickableLabelList _missedCallLabelList;

	QtClickableLabelList _messagesLabelList;

	void addMissedCall(const QString & nickName);

	void addMessage(const QString & nickName);

};

#endif
