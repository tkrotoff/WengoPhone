#ifndef QTVIDEOSETTINGS_H
#define QTVIDEOSETTINGS_H


#include <QtGui>



class QtVideoSettings : public QWidget {
		Q_OBJECT

	public:
		QtVideoSettings ( QWidget * parent = 0, Qt::WFlags f = 0 );

	protected:

		void setupChilds();

		QWidget * _widget;

		QComboBox * _webcamDeviceComboBox;

		QPushButton * _webcamPreviewPushButton;

		QFrame * _webcamPreviewFrame;

		QTreeWidget * _videoQualityTreeWidget;

		QPushButton * _makeTestVideoCallPushButton;

	};



#endif
