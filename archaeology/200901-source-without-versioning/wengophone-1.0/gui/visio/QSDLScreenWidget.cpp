
#include "visio/QSDLScreenWidget.h"

#include <stdlib.h>
#include <stdlib.h>
#include <SDL.h>

QSDLScreenWidget::QSDLScreenWidget( QWidget *parent, const char *name ) :
    QWidget (parent,name)
{   
    atexit(SDL_Quit);
    
    char windowid[64];
    sprintf(windowid, "SDL_WINDOWID=0x%lx", winId());
    SDL_putenv(windowid);

    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Unable to init SDL:: %s\n", SDL_GetError());
        exit(0);
    }

    screen = SDL_SetVideoMode(width(), height(), 0,
            SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_RESIZABLE);
    SDL_FillRect(screen, NULL, 0);
}

QSDLScreenWidget::~QSDLScreenWidget()
{
    if (screen) {
        SDL_FreeSurface(screen);
    }
}

void QSDLScreenWidget::resizeEvent(QResizeEvent * re) {
    int new_width, new_height;

    new_width = re->size().width();
    new_height = re->size().height();

    screen = SDL_SetVideoMode(new_width, new_height, 0,
            SDL_DOUBLEBUF | SDL_HWSURFACE | SDL_RESIZABLE);
}

void QSDLScreenWidget::paintEvent(QPaintEvent *) {
    SDL_Flip(screen);
}
