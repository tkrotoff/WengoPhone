
#include "WebcamClient.h"
#include "WebcamDriver.h"
#include <stdio.h>

wcClient *Webcam::RegisterClient(int format, int fps, QObject *eventWin)
{
    wcClient *client = new wcClient;

    if (fps == 0)
    {
        fps = 10;
    }

    client->eventWindow = eventWin;
    client->fps = fps;
    client->actualFps = fps;
    client->interframeTime = 1000/fps;
    client->timeLastCapture = QTime::currentTime();
    client->framesDelivered = 0;

    switch (format)
    {
    case VIDEO_PALETTE_RGB24:   client->frameSize = RGB24_LEN(WCWIDTH, WCHEIGHT);   client->format = PIX_FMT_BGR24;      break;
    case VIDEO_PALETTE_RGB32:   client->frameSize = RGB32_LEN(WCWIDTH, WCHEIGHT);   client->format = PIX_FMT_RGBA32;     break;
    case VIDEO_PALETTE_YUV420P: client->frameSize = YUV420P_LEN(WCWIDTH, WCHEIGHT); client->format = PIX_FMT_YUV420P;    break;
    case VIDEO_PALETTE_YUV422P: client->frameSize = YUV422P_LEN(WCWIDTH, WCHEIGHT); client->format = PIX_FMT_YUV422P;    break;
    default:
        delete client;
        return 0;
    }

    // Create some buffers for the client
    for (int i=0; i<WC_CLIENT_BUFFERS; i++)
        client->BufferList.append(new unsigned char[client->frameSize]);

    WebcamLock.lock();
    wcClientList.append(client);
    WebcamLock.unlock();

    return client;
}

void Webcam::UnregisterClient(wcClient *client)
{
    WebcamLock.lock();
    wcClientList.remove(client);
    WebcamLock.unlock();

    // Delete client buffers
    unsigned char *it;
    while ((it=client->BufferList.first()) != 0)
    {
        client->BufferList.remove(it);
        delete it;
    }

    // Delete client buffers in the FULL queue
    while ((it=client->FullBufferList.first()) != 0)
    {
        client->FullBufferList.remove(it);
        delete it;
    }

    delete client;
}
