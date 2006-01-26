#include <trayicon.h>

#include <qpopupmenu.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qevent.h>

static const char * pixmap[] = {
	"22 22 8 1",
	"  c Gray100",
	". c Gray97",
	"X c #4f504f",
	"o c #00007f",
	"O c Gray0",
	"+ c none",
	"@ c Gray0",
	"# c Gray0",
	"++++++++++++++++++++++",
	"++++++++++++++++++++++",
	"++++++++++++++++++++++",
	"++++++++++++++++++++++",
	"+OOOOOOOOOOOOOOOOOOOO+",
	"OOXXXXXXXXXXXXXXXXXXOO",
	"OXX.          OO OO  O",
	"OX.      oo     O    O",
	"OX.      oo     O   .O",
	"OX  ooo  oooo   O    O",
	"OX    oo oo oo  O    O",
	"OX  oooo oo oo  O    O",
	"OX oo oo oo oo  O    O",
	"OX oo oo oo oo  O    O",
	"OX  oooo oooo   O    O",
	"OX            OO OO  O",
	"OO..................OO",
	"+OOOOOOOOOOOOOOOOOOOO+",
	"++++++++++++++++++++++",
	"++++++++++++++++++++++",
	"++++++++++++++++++++++",
	"++++++++++++++++++++++"
};

class MyMainWindow : public QMainWindow {
protected:

	virtual void closeEvent(QCloseEvent * event) {
		hide();
	}
};

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	MyMainWindow mainWindow;
	app.setMainWidget(&mainWindow);
	mainWindow.show();

	QPopupMenu menu(&mainWindow);
	menu.insertItem("menu item 1");
	menu.insertItem("menu item 2");
	menu.insertItem("quit", &app, SLOT(quit()));

	TrayIcon trayIcon(QPixmap(pixmap), "tooltip",
				&menu, &mainWindow);
	QObject::connect(&trayIcon, SIGNAL(doubleClicked(const QPoint &)),
				&mainWindow, SLOT(show()));
	trayIcon.show();

	return app.exec();
}
