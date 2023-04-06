
#ifndef QSDLWIDGET_H
#define QSDLWIDGET_H

#include <qwidget.h>
#include <SDL.h>
#include <SDL_video.h>


#ifdef WIN32
#define NEED_SDL_GETENV
#include <SDL_getenv.h>
#else
#ifndef WIN32
#define SDL_putenv putenv
#endif
#endif


struct SDL_Surface;

class QSDLScreenWidget : public QWidget
{
    Q_OBJECT

public:
    QSDLScreenWidget(QWidget *parent=0, const char *name=0);
    ~QSDLScreenWidget();
    SDL_Surface *screen;

protected:
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *);

};

#endif

