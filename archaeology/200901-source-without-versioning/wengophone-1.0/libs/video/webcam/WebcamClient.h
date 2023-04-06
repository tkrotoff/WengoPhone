#ifndef WEBCAMCLIENT_H_
#define WEBCAMCLIENT_H_

#include <sys/types.h>
#include <qobject.h>
#include <qptrlist.h>
#include <qdatetime.h>

#define WC_CLIENT_BUFFERS   1024

struct wcClient
{
    QObject *eventWindow;
    int format;
    int frameSize;
    int fps;
    int actualFps;
    int interframeTime;
    int framesDelivered;
    QPtrList<unsigned char> BufferList;
    QPtrList<unsigned char> FullBufferList;
    QTime timeLastCapture;

};

#endif
