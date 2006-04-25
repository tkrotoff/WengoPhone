#include <webcam/WebcamDriver.h>

#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

class WebcamTest : public QMainWindow {
	Q_OBJECT
public:
	WebcamTest();
	~WebcamTest();

	void frameCapturedEventHandler(IWebcamDriver *sender, piximage *image);
private:

	void paintEvent(QPaintEvent *);

	IWebcamDriver *driver;

	QImage *_image;
};
