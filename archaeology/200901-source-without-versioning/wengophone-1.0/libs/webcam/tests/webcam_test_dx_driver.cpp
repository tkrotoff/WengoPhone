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

#include <webcam/WebcamDirectX.h>
#include <time.h>
#include <qapplication.h>
#include <qmessagebox.h>

class WebcamOpenStartCloseTest {
	public:
		WebcamOpenStartCloseTest(long nb_iter);
		~WebcamOpenStartCloseTest();
		WebcamDirectX *webcam;
	private:
		long _nb_iter;
};

int webcam_frame_callback(void *userdata, int len, uint8_t *sampled_data) {
	return 0;
}

WebcamOpenStartCloseTest::WebcamOpenStartCloseTest(long nb_iter) {
	_nb_iter = nb_iter;

	for(int i = 0; i<= nb_iter; i++) {
		webcam = new WebcamDirectX();
		webcam->setDeviceDefault();
		if( webcam->open() ) {
			webcam->startCapture();
			webcam->stopCapture();
		} else {
			QMessageBox::critical(0, "Webcam test", "No webcam found");
		}
	}
}

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	WebcamOpenStartCloseTest *t1 = new WebcamOpenStartCloseTest(100);
	return 1;
}
