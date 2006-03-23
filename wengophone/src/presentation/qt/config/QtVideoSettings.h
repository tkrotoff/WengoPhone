#ifndef QTVIDEOSETTINGS_H
#define QTVIDEOSETTINGS_H


#include <QtGui>



class QtVideoSettings : public QWidget {
		Q_OBJECT

	public:
		QtVideoSettings ( QWidget * parent = 0, Qt::WFlags f = 0 );

		void saveData();

	protected:

		void setupChilds();

		void readConfigData();

		QWidget * _widget;

		QComboBox * _webcamDeviceComboBox;

		QPushButton * _webcamPreviewPushButton;

		QFrame * _webcamPreviewFrame;

		QTreeWidget * _videoQualityTreeWidget;

		QGroupBox * _videoEnabled;

		QPushButton * _makeTestVideoCallPushButton;

	};



#endif
