#ifndef QTCALLFORWARDSETTINGS_H
#define QTCALLFORWARDSETTINGS_H

#include <QtGui>

class QtCallForwardSettings : public QWidget {
		Q_OBJECT

	public:

		QtCallForwardSettings( QWidget * parent = 0, Qt::WFlags f = 0 );

		void saveData();

	protected:

		void setupChilds();

		void readConfigData();

		QWidget * _widget;

		QCheckBox * _forwardCallCheckBox;

		QCheckBox * _forwardAllCallCheckBox;

		QLineEdit * _phoneNumber1Edit;

		QLineEdit * _phoneNumber2Edit;

		QLineEdit * _phoneNumber3Edit;

		QCheckBox * _forwardCallMobilCheckBox;

	private:



	public Q_SLOTS:

		void forwardCallStateChanged(int state);
		void forwardAllCallToStateChanged(int state);

	};

#endif
