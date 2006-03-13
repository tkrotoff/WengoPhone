#include <webcam.h>

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

	void frameCaptured(piximage *image);
private:

	void paintEvent(QPaintEvent *);

	QImage *_image;
	
	webcam *_webcam;
};
