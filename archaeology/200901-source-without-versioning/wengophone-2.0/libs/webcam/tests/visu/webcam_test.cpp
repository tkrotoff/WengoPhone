/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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
#include <webcam_test.h>

#include <util/Logger.h>

#include <iostream>
#include <fstream>
using namespace std;

const unsigned DEST_W = 176;
const unsigned DEST_H = 144;


void WebcamTest::frameCapturedEventHandler(IWebcamDriver *sender, piximage *image) {
	memcpy(_image->bits(), image->data, image->width * image->height * 4);
	update();
}


void WebcamTest::paintEvent(QPaintEvent *) {
	QPainter painter(this);
	
	if (_image)
		painter.drawImage(0, 0, *_image, 0, 0, _image->width(), _image->height());
}

WebcamTest::WebcamTest()
: _image(NULL) {
	driver = WebcamDriver::getInstance();

	LOG_INFO("Device list:");		
	StringList deviceList = driver->getDeviceList();
	for (register unsigned i = 0 ; i < deviceList.size() ; i++) {
		LOG_INFO("- " + deviceList[i]);
	}

	string device = driver->getDefaultDevice();
	//string device = deviceList[1];

	string title = "Webcam test: using " + device;
	LOG_INFO(title);
	
	driver->frameCapturedEvent +=  boost::bind(&WebcamTest::frameCapturedEventHandler, this, _1, _2);
	driver->setDevice(device);
	driver->setPalette(PIX_OSI_RGB32);
	driver->setResolution(DEST_W, DEST_H);

	setWindowTitle(title.c_str());
	cout << "Using width: " << driver->getWidth() << ", height: " << driver->getHeight() 
		<< " FPS: " << driver->getFPS() << endl;
	setFixedSize(driver->getWidth(), driver->getHeight());
	show();


	if (driver->isOpened()) {
		_image = new QImage(QSize(driver->getWidth(), driver->getHeight()), QImage::Format_ARGB32);
		if (!_image) {
			QMessageBox::critical(0, "Webcam test", "error while creating _image");
		}
		cout << "**Starting capture..." << endl;
		driver->startCapture();
	} else {
		QMessageBox::critical(0, "Webcam test", "No webcam found");
	}
}

WebcamTest::~WebcamTest() {
	delete _image;
	delete driver;
}

int main(int argc, char **argv) {
	QApplication app(argc, argv);

	new WebcamTest();

	return app.exec();
}
