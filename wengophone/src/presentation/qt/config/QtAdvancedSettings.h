#ifndef ADVANCEDSETTINGS_H
#define ADVANCEDSETTINGS_H

#include <QtGui>

#include <WidgetFactory.h>


class QtAdvancedSettings : public QWidget {
		Q_OBJECT

	public:
		QtAdvancedSettings ( QWidget * parent = 0, Qt::WFlags f = 0 );

	protected:

		void setupChild();

		QWidget * _widget;

		QCheckBox * _callAutomaticallyAnswerCheckBox;

		QSpinBox * _callAnswerDelaySpinBox;

		QCheckBox * _chatPopUpCheckBox;

		QCheckBox * _forwardAllCallsToCellPhoneCheckBox;



	};

#endif
