
#include <webcam_c/webcam_c.h>
#include <time.h>
#include <qapplication.h>
#include <qimage.h>
#include <qmainwindow.h>

class WebcamTest {
    public:
        WebcamTest();
        ~WebcamTest();

        struct webcam *webcam;
        QMainWindow *mainWindow;
};


int webcam_frame_callback(void *userdata, int len, uint8_t *sampled_data) {
    WebcamTest *wt = (WebcamTest *) userdata;
    QImage image((uint8_t *)sampled_data, webcamGetCurrentWidth(wt->webcam),
            webcamGetCurrentHeight(wt->webcam), 32, (QRgb *) 0, 0, QImage::LittleEndian);
    bitBlt(wt->mainWindow, 0, 0, &image);
    return 0;
}

WebcamTest::WebcamTest() {
    mainWindow = new QMainWindow();
    webcam = webcamInitialize();
    webcamSetCallback(webcam, webcam_frame_callback);
    webcamSetUserData(webcam, (void *) this);

    webcamOpen(webcam);
    webcamSetPalette(webcam, WEBCAM_PALETTE_RGB32);
    webcamStartCapture(webcam);
    mainWindow->show();
}

int main(int argc, char **argv) {
    WebcamDirectX *webcam;
    QApplication app(argc, argv);

    WebcamTest *t = new WebcamTest();
    return app.exec();
}
