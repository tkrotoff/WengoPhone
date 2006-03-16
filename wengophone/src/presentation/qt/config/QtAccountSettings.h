#ifndef QTACCOUNTSETTINGS_H
#define QTACCOUNTSETTINGS_H
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>

#include <WidgetFactory.h>
#include "QtProtocolSettings.h"

class QtAccountSettings : public QWidget {
		Q_OBJECT

	public:
		QtAccountSettings ( QWidget * parent = 0, Qt::WFlags f = 0 );

	protected:

		QWidget * _widget;

		QPushButton * _imAddPushButton;

		QPushButton * _imModifyPushButton;

		QPushButton * _imDeletePushButton;

		QPushButton * _sipAddPushButton;

		QPushButton * _sipModifyPushButton;

		QPushButton * _sipDeletePushButton;

	public Q_SLOTS:
		void editProfileClicked();

	};


#endif
