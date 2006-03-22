#ifndef QTAUDIOSETTINGS_H
#define QTAUDIOSETTINGS_H


#include <QtGui>

#include <WidgetFactory.h>


class QtAudioSettings : public QWidget
{
    Q_OBJECT

public:
    QtAudioSettings(QWidget * parent = 0, Qt::WFlags f=0);

	void saveData();

protected:
    QWidget     *   _widget;

	void setupChilds();

	void readConfigData();

	QComboBox * _inputDeviceComboBox;

	QComboBox * _outputDeviceComboBox;

	QComboBox * _ringingDeviceComboBox;

	QGroupBox * _personalAudioConfig;

	QPushButton * _makeTestCallPushButton;

};


#endif
