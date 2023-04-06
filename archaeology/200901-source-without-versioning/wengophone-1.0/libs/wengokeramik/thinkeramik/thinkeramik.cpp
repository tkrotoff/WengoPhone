/* Keramik Style for KDE3
   Copyright (c) 2002 Malte Starostik <malte@kde.org>
             (c) 2002,2003 Maksim Orlovich <mo002j@mail.rochester.edu>

   based on the KDE3 HighColor Style

   Copyright (C) 2001-2002 Karol Szwed      <gallium@kde.org>
             (C) 2001-2002 Fredrik Höglund  <fredrik@kde.org>

   Drawing routines adapted from the KDE2 HCStyle,
   Copyright (C) 2000 Daniel M. Duley       <mosfet@kde.org>
             (C) 2000 Dirk Mueller          <mueller@kde.org>
             (C) 2001 Martijn Klingens      <klingens@kde.org>

    Progressbar code based on KStyle, Copyright (C) 2001-2002 Karol Szwed <gallium@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// $Id: thinkeramik.cpp,v 1.121.2.2 2004/03/25 13:35:08 lukas Exp $

#include <qbitmap.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdrawutil.h>
#include <qframe.h>
#include <qheader.h>
#include <qintdict.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmenubar.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qprogressbar.h>
#include <qpushbutton.h>
#include <qsettings.h>
#include <qslider.h>
#include <qstyleplugin.h>
#include <qtabbar.h>
#include <qtimer.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qtabwidget.h>

#include <kpixmap.h>
#include <kpixmapeffect.h>

#include "moc_thinkeramik.cc"
#include "gradients.h"
#include "colorutil.h"
#include "thinkeramikrc.h"
#include "thinkeramikimage.h"

#include "bitmaps.h"
#include "pixmaploader.h"

#define loader ThinKeramik::PixmapLoader::the()

QColor  sbacolor;
QColor  sbscolor;
QString sbgcolstr, sbafgcolstr;
QString  atcolstr;
QString  mfgcolstr;
QString  mbgcolstr;
QString  stpcolstr;
QString  menuSepColStr;
QString  bordercolstr;
QString  mselfgcolstr;
QString  mselbgcolstr;
QString  mselshdcolstr;
QString  btnprlcolstr;
QString  cbxprlcolstr;
QString  pgbColor;
QString  pglColor1;
QString  pglColor2;
QString  pggColor;
QString  buttonBorderColStr;
QString  comboboxBorderColStr;
QString  statusbarBorderColStr;
bool disableul;
bool enablegrad;
bool customgrad;
bool strong3dmi;
bool strong3dms;
bool enablemi3d;
bool enablems3d;
bool useshadowtxtM;
bool disableEmpShadowM;
bool useshadowtxtMB;
bool useshadowtxtB;
bool useshadowtxtT;
bool usesgb;
bool usesgbv;
bool usesgc;
bool useflatbevel;
bool usepbcbevel;
bool disablebb;
bool useBGCTab;
bool useSingleLineHandles;
bool disableScrollbarBorder;
bool useShadowlessButton;
bool prelightInnerCircumOnly;
bool prelightInnerCircumOnlyCbx;
bool highlightDefaultButton;
bool highlightActiveTask;
bool flattenActiveTab;
bool surroundSunkenPanel;
bool surroundTaskbutton;
bool useLightCombo;
bool useLightNECombo;
bool reduceButtonSize;
bool barBorderFollowsContrast;
bool removeSublineArea;

int defaultStripeWidth = 23;
int currentStripeWidth = 0;
int buttonBorderDarkValue = 150;
int lcbbwidth = 19;

QPixmap menubgimage;
bool scaledmenubgi;

bool combobox = false;
bool lightWeightMode = false;

//TANGUY
static const QString styleName = "WengoKeramik2";

// -- Style Plugin Interface -------------------------
class ThinKeramikStylePlugin : public QStylePlugin
{
public:
	ThinKeramikStylePlugin() {}
	~ThinKeramikStylePlugin() {}

	QStringList keys() const
	{
		if (QPixmap::defaultDepth() > 8)
			return QStringList() << styleName;
		else
			return QStringList();
	}

	QStyle* create( const QString& key )
	{
		if ( key == styleName.lower() ) return new ThinKeramikStyle();
		return 0;
	}
};

Q_EXPORT_PLUGIN( ThinKeramikStylePlugin )
// ---------------------------------------------------


// ### Remove globals
/*
QBitmap lightBmp;
QBitmap grayBmp;
QBitmap dgrayBmp;
QBitmap centerBmp;
QBitmap maskBmp;
QBitmap xBmp;
*/
namespace
{
	const int itemFrame       = 2;
	const int itemHMargin     = 6;
	const int itemVMargin     = 0;
	const int arrowHMargin    = 6;
	const int rightBorder     = 12;
	const char* kdeToolbarWidget = "kde toolbar widget";

	const int smallButMaxW    = 27;
	const int smallButMaxH    = 20;
	const int titleBarH       = 22;
}
// ---------------------------------------------------------------------------

namespace
{
	void drawThinKeramikArrow(QPainter* p, QColorGroup cg, QRect r, QStyle::PrimitiveElement pe, bool down, bool enabled, bool customScrollMode)
	{
		QPointArray a;

		switch(pe)
		 {
			case QStyle::PE_ArrowUp:
				a.setPoints(QCOORDARRLEN(thinkeramik_up_arrow), thinkeramik_up_arrow);
				break;

			case QStyle::PE_ArrowDown:
				a.setPoints(QCOORDARRLEN(thinkeramik_down_arrow), thinkeramik_down_arrow);
				break;

			case QStyle::PE_ArrowLeft:
				a.setPoints(QCOORDARRLEN(thinkeramik_left_arrow), thinkeramik_left_arrow);
				break;

			default:
				a.setPoints(QCOORDARRLEN(thinkeramik_right_arrow), thinkeramik_right_arrow);
		}

		p->save();
		/*if ( down )
			p->translate( pixelMetric( PM_ButtonShiftHorizontal ),
						pixelMetric( PM_ButtonShiftVertical ) );
		*/

		if ( enabled ) {
			//CHECKME: Why is the -1 needed?
			a.translate( r.x() + r.width() / 2 - 1, r.y() + r.height() / 2 );

			QColor sbacol = sbacolor;
			if (customScrollMode)
				sbacol = cg.button();

			QColor sbafgcol = cg.buttonText();
			if (!customScrollMode && !sbafgcolstr.isNull())
			        sbafgcol = QColor(sbafgcolstr);

			if (!down)
				p->setPen( sbafgcol );
			else
				p->setPen ( sbacol );
			p->drawLineSegments( a );
		} else {
			a.translate( r.x() + r.width() / 2, r.y() + r.height() / 2 + 1 );
			p->setPen( cg.light() );
			p->drawLineSegments( a );
			a.translate( -1, -1 );
			p->setPen( cg.mid() );
			p->drawLineSegments( a );
		}
		p->restore();
	}
}

// XXX
/* reimp. */
void ThinKeramikStyle::renderMenuBlendPixmap( KPixmap& pix, const QColorGroup &cg,
		const QPopupMenu *popup ) const
{

	QColor mfgcolor, mbgcolor, stpcolor;
	if (!mfgcolstr.isNull()) {
	      mfgcolor = QColor(mfgcolstr);
	} else {
	      mfgcolor = cg.text();
	}
	if (!mbgcolstr.isNull()) {
	      mbgcolor = QColor(mbgcolstr);
	} else {
	      mbgcolor = cg.background().light(110);
	}
	if (!stpcolstr.isNull()) {
	      stpcolor = QColor(stpcolstr);
	} else {
	      stpcolor = cg.mid();
	}

	int stripewidth = QMAX( defaultStripeWidth, currentStripeWidth );

  	QPainter p( &pix );
	bool revlayout = QApplication::reverseLayout();

        if (menubgimage.isNull()) {
              if (enablegrad) {
#ifdef Q_WS_X11 // Only draw menu gradients on TrueColor, X11 visuals
  	            if ( QPaintDevice::x11AppDepth() >= 24 )
		          if (customgrad) {
	                        KPixmapEffect::gradient( pix, mbgcolor, stpcolor,
				   		         KPixmapEffect::HorizontalGradient );
			  } else {
	                        KPixmapEffect::gradient( pix, mbgcolor.light(120) , mbgcolor.dark(115),
						         KPixmapEffect::HorizontalGradient );
			  }
		    else
#endif
		          pix.fill( mbgcolor );
	      } else {
		    if (revlayout) {
		          p.fillRect( 0, 0, popup->frameRect().width()-stripewidth, pix.height(), mbgcolor );
		    } else {
		          p.fillRect( 0, 0, pix.width(), pix.height(), mbgcolor );
		    }
	      }

	      if (!customgrad) {
		    if (revlayout) {
		          if (enablems3d) {
		                const QRect gr = QRect(popup->frameRect().width()-stripewidth, 0, stripewidth, pix.height());
				ThinKeramik::GradientPainter::renderGradient2( &p, gr, stpcolor.dark(105), false, true, strong3dms);
			  } else {
  	                        p.fillRect( popup->frameRect().width()-stripewidth, 0, stripewidth, pix.height(), stpcolor );
			  }
		    } else {
		          if (enablems3d) {
 		                const QRect gr = QRect(popup->frameRect().left() + 1, 0, stripewidth, pix.height());
				ThinKeramik::GradientPainter::renderGradient2( &p, gr, stpcolor.dark(105), false, true, strong3dms);
			  } else {
			        p.fillRect( popup->frameRect().left() + 1, 0, stripewidth, pix.height(),stpcolor );
			  }
		    }
	      }
	} else {
	      if (!scaledmenubgi) {
  	            p.drawTiledPixmap(popup->frameRect(), menubgimage);
	      } else {
		    p.drawPixmap(popup->frameRect(), menubgimage);
	      }
	}

}

// XXX
QRect ThinKeramikStyle::subRect(SubRect r, const QWidget *widget) const
{
	// We want the focus rect for buttons to be adjusted from
	// the Qt3 defaults to be similar to Qt 2's defaults.
	// -------------------------------------------------------------------
	switch ( r )
	{
		case SR_PushButtonFocusRect:
		{
			const QPushButton* button = (const QPushButton*) widget;
			QRect wrect(widget->rect());

			if ( (button->isDefault() || button->autoDefault()) && highlightDefaultButton )
			{
				return QRect(wrect.x() + 4, wrect.y() + 5, wrect.width() - 10,  wrect.height() - 10);
			}
			else
			{
				return QRect(wrect.x() + 3, wrect.y() + 5, wrect.width() - 8,  wrect.height() - 10);
			}

			break;
		}

		case SR_ComboBoxFocusRect:
		{
			return querySubControlMetrics( CC_ComboBox, widget, SC_ComboBoxEditField );
		}

		case SR_CheckBoxFocusRect:
		{
			const QCheckBox* cb = static_cast<const QCheckBox*>(widget);

			//Only checkbox, no label
			if (cb->text().isEmpty() && (cb->pixmap() == 0) )
			{
				QRect bounding = cb->rect();
				QSize checkDim = loader.size( thinkeramik_checkbox_on);
				int   cw = checkDim.width();
				int   ch = checkDim.height();

				QRect checkbox(bounding.x() + 1, bounding.y() + 1 + (bounding.height() - ch)/2,
								cw - 3, ch - 4);

				return checkbox;
			}

			//Fallthrough intentional
		}

		case SR_ProgressBarGroove:
			return widget->rect();

		default:
			return KStyle::subRect( r, widget );
	}
}


QPixmap ThinKeramikStyle::stylePixmap(StylePixmap stylepixmap,
									const QWidget* widget,
									const QStyleOption& opt) const
{
    switch (stylepixmap) {
		case SP_TitleBarMinButton:
			return ThinKeramik::PixmapLoader::the().pixmap(thinkeramik_title_iconify,
				Qt::black, Qt::black, false, false);
			//return qpixmap_from_bits( iconify_bits, "title-iconify.png" );
		case SP_TitleBarMaxButton:
			return ThinKeramik::PixmapLoader::the().pixmap(thinkeramik_title_maximize,
				Qt::black, Qt::black, false, false);
		case SP_TitleBarCloseButton:
			if (widget && widget->inherits("KDockWidgetHeader"))
				return ThinKeramik::PixmapLoader::the().pixmap(thinkeramik_title_close_tiny,
				Qt::black, Qt::black, false, false);
			else	return ThinKeramik::PixmapLoader::the().pixmap(thinkeramik_title_close,
				Qt::black, Qt::black, false, false);
		case SP_TitleBarNormalButton:
			return ThinKeramik::PixmapLoader::the().pixmap(thinkeramik_title_restore,
				Qt::black, Qt::black, false, false);
		default:
			break;
	}

	return KStyle::stylePixmap(stylepixmap, widget, opt);
}




ThinKeramikStyle::ThinKeramikStyle()
	:KStyle( AllowMenuTransparency | FilledFrameWorkaround, ThreeButtonScrollBar ),
		maskMode(false), formMode(false),
		toolbarBlendWidget(0), titleBarMode(None), flatMode(false), customScrollMode(false), kickerMode(false)
{
	forceSmallMode = false;
	hoverWidget    = 0;
	progAnimShift  = 0;

	QSettings settings;

	//highlightScrollBar = settings.readBoolEntry("/qt/ThinKmk/highlightScrollBar", true);
	highlightScrollBar = true;
	animateProgressBar = settings.readBoolEntry("/qt/ThinKmk/animateProgressBar", false);

	if (animateProgressBar)
	{
		QTimer* timer = new QTimer( this );
		timer->start(50, false);
		connect(timer, SIGNAL(timeout()), this, SLOT(updateProgressPos()));
	}

}

void ThinKeramikStyle::updateProgressPos()
{
	progAnimShift++;
	if (progAnimShift == 28)
		progAnimShift = 0;
		
	//Update the registered progressbars.
	QMap<QWidget*, bool>::iterator iter;
	for (iter = progAnimWidgets.begin(); iter != progAnimWidgets.end(); iter++)
		iter.key()->update();
}

ThinKeramikStyle::~ThinKeramikStyle()
{
	ThinKeramik::PixmapLoader::release();
	ThinKeramik::GradientPainter::releaseCache();
	ThinKeramikDbCleanup();
}

void ThinKeramikStyle::polish(QApplication* app)
{
	//TANGUY
	//style 0 - WengoKeramik blue
	//app->setPalette(QPalette(QColor(158, 185, 226), QColor(158, 185, 226)), true);
	//style 1
	//app->setPalette(QPalette(QColor(159, 156, 169), QColor(224, 223, 227)), true);
	//style 2
	app->setPalette(QPalette(QColor(210, 208, 215), QColor(234, 233, 235)), true);
	//style 3
	//app->setPalette(QPalette(QColor(205, 203, 209), QColor(224, 223, 227)), true);
	//style 4
	//app->setPalette(QPalette(QColor(255, 255, 255), QColor(158, 185, 226)), true);
	//style 5
	//app->setPalette(QPalette(QColor(54, 74, 102), QColor(158, 185, 226)), true);

	if (!qstrcmp(app->argv()[0], "kicker"))
		kickerMode = true;
}

void ThinKeramikStyle::polish(QWidget* widget)
{
	// Put in order of highest occurrence to maximise hit rate
	if ( widget->inherits( "QPushButton" )  || widget->inherits( "QComboBox" ) || widget->inherits("QToolButton") )
	{
		widget->installEventFilter(this);
		if ( widget->inherits( "QComboBox" ) )
			widget->setBackgroundMode( NoBackground );
	}
	else if ( widget->inherits( "QMenuBar" ) || widget->inherits( "QPopupMenu" ) )
		widget->setBackgroundMode( NoBackground );

 	else if ( widget->parentWidget() &&
			  ( ( widget->inherits( "QListBox" ) && widget->parentWidget()->inherits( "QComboBox" ) ) ||
	            widget->inherits( "KCompletionBox" ) ) ) {
		QListBox* listbox = (QListBox*) widget;
		listbox->setLineWidth( 4 );
		listbox->setBackgroundMode( NoBackground );
		widget->installEventFilter( this );

	} else if (widget->inherits("QToolBarExtensionWidget")) {
		widget->installEventFilter(this);
		//widget->setBackgroundMode( NoBackground );
 	}
	else if ( !qstrcmp( widget->name(), kdeToolbarWidget ) ) {
		widget->setBackgroundMode( NoBackground );
		widget->installEventFilter(this);
	}

	if (animateProgressBar && ::qt_cast<QProgressBar*>(widget))
	{
		progAnimWidgets[widget] = true;
		connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(progressBarDestroyed(QObject*)));
	}

	KStyle::polish(widget);
}

void ThinKeramikStyle::unPolish(QWidget* widget)
{
	//### TODO: This needs major cleanup (and so does polish() )
	if ( widget->inherits( "QPushButton" ) || widget->inherits( "QComboBox"  ) )
	{
		if ( widget->inherits( "QComboBox" ) )
			widget->setBackgroundMode( PaletteButton );
		widget->removeEventFilter(this);
	}
	else if ( widget->inherits( "QMenuBar" ) || widget->inherits( "QPopupMenu" ) )
		widget->setBackgroundMode( PaletteBackground );

 	else if ( widget->parentWidget() &&
			  ( ( widget->inherits( "QListBox" ) && widget->parentWidget()->inherits( "QComboBox" ) ) ||
	            widget->inherits( "KCompletionBox" ) ) ) {
		QListBox* listbox = (QListBox*) widget;
		listbox->setLineWidth( 1 );
		listbox->setBackgroundMode( PaletteBackground );
		widget->removeEventFilter( this );
		widget->clearMask();
	} else if (widget->inherits("QToolBarExtensionWidget")) {
		widget->removeEventFilter(this);
	}
	else if ( !qstrcmp( widget->name(), kdeToolbarWidget ) ) {
		widget->setBackgroundMode( PaletteBackground );
		widget->removeEventFilter(this);
	}
	else if ( ::qt_cast<QProgressBar*>(widget) )
	{
		progAnimWidgets.remove(widget);
	}

	KStyle::unPolish(widget);
}

void ThinKeramikStyle::progressBarDestroyed(QObject* obj)
{
	progAnimWidgets.remove(static_cast<QWidget*>(obj));
}


void ThinKeramikStyle::polish( QPalette& )
{
	loader.clear();

	QSettings settings;

 	QString sbscolstr = settings.readEntry("/qt/ThinKmk/Scrollbar");
 	if (!sbscolstr.isNull()) {
 	      sbscolor = QColor(sbscolstr);
 	} else {
 	      sbscolor = QColor(settings.readEntry("/qt/KWinPalette/inactiveBackground", "#d8d8d8"));
 	}
 	QString sbacolstr = settings.readEntry("/qt/ThinKmk/ScrollArrowbtn");
 	if (!sbacolstr.isNull()) {
 	      sbacolor = QColor(sbacolstr);
 	} else {
 	      sbacolor = sbscolor;
 	}
 	sbgcolstr = settings.readEntry("/qt/ThinKmk/ScrollGroove");
 	sbafgcolstr = settings.readEntry("/qt/ThinKmk/ScrollArrowFG");
 
 	disableul = settings.readBoolEntry("/qt/ThinKmk/DisableUpperLines");
 	enablegrad = settings.readBoolEntry("/qt/ThinKmk/EnableGradation");
 	customgrad = settings.readBoolEntry("/qt/ThinKmk/CustomGradation");
 	strong3dmi = settings.readBoolEntry("/qt/ThinKmk/Strong3DMI");
 	strong3dms = settings.readBoolEntry("/qt/ThinKmk/Strong3DMS");
 	enablemi3d = settings.readBoolEntry("/qt/ThinKmk/EnableMenuItem3D");
 	enablems3d = settings.readBoolEntry("/qt/ThinKmk/EnableStripe3D");
 	atcolstr = settings.readEntry("/qt/ThinKmk/ActiveTab");
 	mfgcolstr = settings.readEntry("/qt/ThinKmk/MenuFG");
 	mbgcolstr = settings.readEntry("/qt/ThinKmk/MenuBG");
 	stpcolstr = settings.readEntry("/qt/ThinKmk/MenuStripe");
 	menuSepColStr = settings.readEntry("/qt/ThinKmk/MenuSeparator");
 	mselfgcolstr = settings.readEntry("/qt/ThinKmk/MenuSelectedFG");
 	mselbgcolstr = settings.readEntry("/qt/ThinKmk/MenuSelectedBG");
 	mselshdcolstr = settings.readEntry("/qt/ThinKmk/MenuSelectedShadow");
 	bordercolstr = settings.readEntry("/qt/ThinKmk/MenuBorder");
 	btnprlcolstr = settings.readEntry("/qt/ThinKmk/ButtonPrelight");
 	cbxprlcolstr = settings.readEntry("/qt/ThinKmk/ComboboxPrelight");
 	prelightInnerCircumOnly = settings.readBoolEntry("/qt/ThinKmk/PrelightInnerCircumOnly");
 	prelightInnerCircumOnlyCbx = settings.readBoolEntry("/qt/ThinKmk/PrelightInnerCircumOnlyCombobox");
 	pgbColor  = settings.readEntry("/qt/ThinKmk/ProgressbarColor");
 	pglColor1 = settings.readEntry("/qt/ThinKmk/ProgressbarLabelColor1");
 	pglColor2 = settings.readEntry("/qt/ThinKmk/ProgressbarLabelColor2");
 	pggColor  = settings.readEntry("/qt/ThinKmk/ProgressbarGrooveColor");
 	buttonBorderColStr = settings.readEntry("/qt/ThinKmk/ButtonBorderColor");
 	comboboxBorderColStr = settings.readEntry("/qt/ThinKmk/ComboboxBorderColor");
	statusbarBorderColStr = settings.readEntry("/qt/ThinKmk/StatusbarBorderColor");
 	useshadowtxtB = settings.readBoolEntry("/qt/ThinKmk/UseShadowTextButton");
 	useshadowtxtM = settings.readBoolEntry("/qt/ThinKmk/UseShadowTextMenu");
 	disableEmpShadowM = settings.readBoolEntry("/qt/ThinKmk/DisableShadowEmphasis");
 	useshadowtxtMB = settings.readBoolEntry("/qt/ThinKmk/UseShadowTextMenuBar");
 	useshadowtxtT = settings.readBoolEntry("/qt/ThinKmk/UseShadowTextTab");
 	usesgb = settings.readBoolEntry("/qt/ThinKmk/UseSGButton");
 	usesgbv = settings.readBoolEntry("/qt/ThinKmk/UseSGBevel", usesgb);
 	usesgc = settings.readBoolEntry("/qt/ThinKmk/UseSGCombobox");
 	useflatbevel = settings.readBoolEntry("/qt/ThinKmk/UseFlatBevel");
 	usepbcbevel = settings.readBoolEntry("/qt/ThinKmk/UsePBCBevel");

	//TANGUY
	//disablebb = settings.readBoolEntry("/qt/ThinKmk/DisableBarBorder");
	disablebb = true;

 	useBGCTab = settings.readBoolEntry("/qt/ThinKmk/UseBGColorTab");
 	useSingleLineHandles = settings.readBoolEntry("/qt/ThinKmk/UseSingleLineHandles");
 	disableScrollbarBorder = settings.readBoolEntry("/qt/ThinKmk/DisableScrollbarBorder");
 	useShadowlessButton = settings.readBoolEntry("/qt/ThinKmk/UseShadowlessButton");
 	highlightDefaultButton = settings.readBoolEntry("/qt/ThinKmk/HighlightDefaultButton");
  	highlightActiveTask = settings.readBoolEntry("/qt/ThinKmk/HighlightActiveTask");
  	flattenActiveTab = settings.readBoolEntry("/qt/ThinKmk/FlattenActiveTab");
  	surroundSunkenPanel = settings.readBoolEntry("/qt/ThinKmk/SurroundSunkenPanel");
 	surroundTaskbutton = settings.readBoolEntry("/qt/ThinKmk/SurroundTaskbutton");
 	useLightCombo = settings.readBoolEntry("/qt/ThinKmk/UseLightWeightCombobox");
 	useLightNECombo = settings.readBoolEntry("/qt/ThinKmk/UseLightWeightStdCombobox");
  	reduceButtonSize = settings.readBoolEntry("/qt/ThinKmk/ReduceButtonSize");
 	barBorderFollowsContrast = settings.readBoolEntry("/qt/ThinKmk/BarBorderFollowsContrast");
	removeSublineArea = settings.readBoolEntry("/qt/ThinKmk/RemoveSublineArea");
 	QString mimgfile = settings.readEntry("/qt/ThinKmk/MenuBGImage");

	if (mimgfile) {
	      menubgimage = QPixmap(mimgfile);
	} else {
	      menubgimage = QPixmap();
	}
 	scaledmenubgi = settings.readBoolEntry("/qt/ThinKmk/ScaledMenuBGImage");
}

QColor ThinKeramikStyle::getBlendColor(const QColor &col1, const QColor& col2,
				    int col1Ratio) const
{
      /*
	col1Raiot: Ratio of color1 (0-100)
      */

	int col2Ratio = 100 - col1Ratio;
        if (col1Ratio < 0) {
		col1Ratio = 0;
		col2Ratio = 100;
	} else if (col1Ratio > 100) {
		col1Ratio = 100;
		col2Ratio = 0;
	}

	QColor blendColor = QColor( (col1.red() * col1Ratio / 100   + col2.red() * col2Ratio / 100),
				    (col1.green() * col1Ratio / 100 + col2.green() * col2Ratio / 100),
				    (col1.blue() * col1Ratio / 100  + col2.blue() * col2Ratio / 100) );
	return blendColor;
}

QRegion ThinKeramikStyle::getUnPrlRegion(const QRect &r) const
{
        int x,y,w,h, x1,x2,y1,y2;
	r.rect(&x, &y, &w, &h);

	x1 = x+4;
	y1 = y+5;
	x2 = x1 + w - 11;
	y2 = y1 + h - 11;

	QRegion unPrlClip;
	unPrlClip |= QRect(x1, y1, w-10, h-10);
	unPrlClip -= QRect(x1, y1, 1, 1);
	unPrlClip -= QRect(x1, y2, 1, 1);
	unPrlClip -= QRect(x2, y1, 1, 1);
	unPrlClip -= QRect(x2, y2, 1, 1);

	return unPrlClip;
}

void ThinKeramikStyle::drawButtonBorder(QPainter *p, const QRect &r, const QColor& col,
				    int buttonType) const
{
      /*
	buttonType
	1: Smallbutton
	2: Pushbutton
	3: Combobox
      */

        int x,y,w,h, x1,x2,y1,y2;
	r.rect(&x, &y, &w, &h);

	if (buttonType == 1) {
	        x1 = x;
		y1 = y;
		x2 = x + w - 1;
		y2 = y + h - 1;
	} else {
	        x1 = x;
		y1 = y+2;
		x2 = x + w - 3;
		y2 = y + h - 3;
	}

	p->save();

	p->setPen(col);
	p->drawLine(x1+2, y1, x2-2, y1);
	p->drawLine(x1, y1+2, x1, y2-2);
	p->drawLine(x2, y1+2, x2, y2-2);
	p->drawLine(x1+2, y2, x2-2, y2);

	//p->setPen(col.light(110));
	p->drawPoint(x1+1, y1+1);
	p->drawPoint(x2-1, y1+1);
	p->drawPoint(x1+1, y2-1);
	p->drawPoint(x2-1, y2-1);

	p->restore();
}

/** 
 Draws gradient background for toolbar buttons, handles and spacers
*/
static void renderToolbarEntryBackground(QPainter* paint,
		 const QToolBar* parent, QRect r, const QColorGroup& cg, bool horiz)
{
	int  toolWidth, toolHeight;
	
	//Do we have a parent toolbar to use?
	if (parent)
	{
		//Calculate the toolbar geometry.
		//The initial guess is the size of the parent widget
		toolWidth  = parent->width();
		toolHeight = parent->height();
					
		//If we're floating, however, wee need to fiddle
		//with height to skip the titlebar
		if (parent->place() == QDockWindow::OutsideDock)
		{
			toolHeight = toolHeight - titleBarH - 2*parent->frameWidth() + 2;
			//2 at the end = the 2 pixels of border of a "regular"
			//toolbar we normally paint over.
		}
	}
	else
	{
		//No info, make a guess.
		//We take the advantage of the fact that the non-major
		//sizing direction parameter is ignored
		toolWidth  = r.width () + 2;
		toolHeight = r.height() + 2;
	}

	//Calculate where inside the toolbar we're
	int xoff = 0, yoff = 0;
	if (horiz)
		yoff = (toolHeight - r.height())/2;
	else
		xoff = (toolWidth - r.width())/2;
						
	ThinKeramik::GradientPainter::renderGradient( paint, r, cg.button(),
		horiz, false /*Not a menubar*/,
		xoff, yoff,
		toolWidth, toolHeight);
}

static void renderToolbarWidgetBackground(QPainter* painter, const QWidget* widget)
{
	// Draw a gradient background for custom widgets in the toolbar
	// that have specified a "kde toolbar widget" name, or
	// are caught as toolbar widgets otherwise

	// Find the top-level toolbar of this widget, since it may be nested in other
	// widgets that are on the toolbar.
	QWidget *parent = static_cast<QWidget*>(widget->parentWidget());
	int x_offset = widget->x(), y_offset = widget->y();
	while (parent && parent->parent() && !qstrcmp( parent->name(), kdeToolbarWidget ) )
	{
		x_offset += parent->x();
		y_offset += parent->y();
		parent = static_cast<QWidget*>(parent->parent());
	}

	QRect pr = parent->rect();
	bool horiz_grad = pr.width() > pr.height();

	int  toolHeight = parent->height();
	int  toolWidth  = parent->width ();

	// Check if the parent is a QToolbar, and use its orientation, else guess.
	//Also, get the height to use in the gradient from it.
	QToolBar* tb = dynamic_cast<QToolBar*>(parent);
	if (tb)
	{
		horiz_grad = tb->orientation() == Qt::Horizontal;

		//If floating, we need to skip the titlebar.
		if (tb->place() == QDockWindow::OutsideDock)
		{
			toolHeight = tb->height() - titleBarH - 2*tb->frameWidth() + 2;
			//Calculate offset here by looking at the bottom edge difference, and height.
			//First, calculate by how much the widget needs to be extended to touch
			//the bottom edge, minus the frame (except we use the current y_offset
			// to map to parent coordinates)
			int needToTouchBottom = tb->height() - tb->frameWidth() -
									(widget->rect().bottom() + y_offset);

			//Now, with that, we can see which portion to skip in the full-height
			//gradient -- which is the stuff other than the extended
			//widget
			y_offset = toolHeight - (widget->height() + needToTouchBottom) - 1;
		}
	}

	if (painter)
	{
	        //ThinKeramik::GradientPainter::renderGradient( painter, widget->rect(),
		//	 widget->colorGroup().button(), horiz_grad, false,
		//	 x_offset, y_offset, toolWidth, toolHeight);
		painter->fillRect(QRect(0, 0, pr.width(), pr.height()), parent->colorGroup().background());
	}
	else
	{
		QPainter p( widget );
		//ThinKeramik::GradientPainter::renderGradient( &p, widget->rect(),
		//	 widget->colorGroup().button(), horiz_grad, false,
		//	 x_offset, y_offset, toolWidth, toolHeight);
		p.fillRect(QRect(0, 0, pr.width(), pr.height()), parent->colorGroup().background());
	}
}

// This function draws primitive elements as well as their masks.
void ThinKeramikStyle::drawPrimitive( PrimitiveElement pe,
									QPainter *p,
									const QRect &r,
									const QColorGroup &cg,
									SFlags flags,
									const QStyleOption& opt ) const
{
	bool down = flags & Style_Down;
	bool on   = flags & Style_On;
	bool disabled = ( flags & Style_Enabled ) == 0;
	int  x, y, w, h;
	r.rect(&x, &y, &w, &h);

	int x2 = x+w-1;
	int y2 = y+h-1;

	switch(pe)
	{
		// BUTTONS
		// -------------------------------------------------------------------
		case PE_ButtonDefault:
		{
			bool sunken = on || down;
			bool hotButton = false;

			QColor btncolor = cg.button();

			int id;
			if ( sunken ) id  = thinkeramik_pushbutton_pressed;
			        else id = thinkeramik_pushbutton;

			if (flags & Style_MouseOver && id == thinkeramik_pushbutton ) {
			        if (!btnprlcolstr.isNull()) {
				        btncolor = QColor(btnprlcolstr);
				} else {
				        btncolor = cg.button().light(107);
				}
				hotButton = true;
			}

			if (usesgb && id == thinkeramik_pushbutton) {
			        id = thinkeramik_pushbutton_sg;
			}

			if (useShadowlessButton) {
			        if (id == thinkeramik_pushbutton)
				        id = thinkeramik_pushbutton_shadowless;
				else if  (id == thinkeramik_pushbutton_sg)
				        id = thinkeramik_pushbutton_sg_shadowless;
			}

			//p->fillRect( r, cg.background() );
			if (highlightDefaultButton && !disabled && !hotButton &&!sunken) {
			        QColor buttonHighlightColor = getBlendColor(cg.highlight(), cg.button(), 70);
			        ThinKeramik::RectTilePainter( id, false ).draw(p, r, buttonHighlightColor, cg.background(), disabled,  pmode() );
				p->save();
				p->setClipping( false );
				QRegion unPrlClip = getUnPrlRegion(r);
				p->setClipRegion( unPrlClip );
				ThinKeramik::RectTilePainter( id, false ).draw(p, r, cg.button(), cg.background(), disabled,  pmode() );
				p->setClipping( false );
				p->restore();
			} else {
			        ThinKeramik::RectTilePainter( id, false ).draw(p, r, btncolor, cg.background(), disabled,  pmode() );
			}

			if (!disabled) {
			        QColor buttonBorderColor = cg.button().dark(buttonBorderDarkValue);
				if (!buttonBorderColStr.isNull() ) {
			                buttonBorderColor = QColor(buttonBorderColStr);
				}
				drawButtonBorder(p, r, buttonBorderColor, 2); // Pushbutton Border

				if (prelightInnerCircumOnly && hotButton) {
				        p->save();
					p->setClipping( false );
					QRegion unPrlClip = getUnPrlRegion(r);
					p->setClipRegion( unPrlClip );
					ThinKeramik::RectTilePainter( id, false ).draw(p, r, cg.button(), cg.background(), disabled,  pmode() );
					p->setClipping( false );
					p->restore();
				}
			}

			break;
		}

		case PE_ButtonDropDown:
		case PE_ButtonTool:
		{
			if (titleBarMode)
			{
				QRect nr;
				if (titleBarMode == Maximized)
				{
					//### What should we draw at sides?
					nr = QRect(r.x(), r.y(), r.width()-1, r.height() );
				}
				else
				{
					int offX = (r.width() - 15)/2;
					int offY = (r.height() - 15)/2;

					if (flags & Style_Down)
						offY += 1;

					nr = QRect(r.x()+offX, r.y()+offY, 15, 15);
				}

				ThinKeramik::ScaledPainter(flags & Style_Down ? thinkeramik_titlebutton_pressed : thinkeramik_titlebutton,
										ThinKeramik::ScaledPainter::Both).draw( p, nr, cg.button(), cg.background());
				return;
			}

			if (on)
			{
				ThinKeramik::RectTilePainter(thinkeramik_toolbar_clk).draw(p, r, cg.button(), cg.background());
				p->setPen(cg.dark());
				p->drawLine(x, y, x2, y);
				p->drawLine(x, y, x, y2);
			}
			else if (down)
			{
				ThinKeramik::RectTilePainter(thinkeramik_toolbar_clk).draw(p, r, cg.button(), cg.background());
			}
			else {
				if (flags & Style_MouseOver)
				{
				ThinKeramik::GradientPainter::renderGradient( p,
					QRect(r.x(), 0, r.width(), r.height()),
					ThinKeramik::ColorUtil::lighten(cg.button(), 115), flags & Style_Horizontal, false );
				}
				else
					ThinKeramik::GradientPainter::renderGradient( p,
						QRect(r.x(), 0, r.width(), r.height()),
						cg.button(), flags & Style_Horizontal, false );

				p->setPen(cg.button().light(70));
				p->drawLine(x, y, x2-1, y);
				p->drawLine(x, y, x, y2-1);
				p->drawLine(x+2, y2-1, x2-1, y2-1);
				p->drawLine(x2-1, y+2, x2-1, y2-2);

				p->setPen(ThinKeramik::ColorUtil::lighten(cg.button(), 115) );
				p->drawLine(x+1, y+1, x2-1, y+1);
				p->drawLine(x+1, y+1, x+1, y2);
				p->drawLine(x, y2, x2, y2);
				p->drawLine(x2, y, x2, y2);
			}

			break;
		}

		// PUSH BUTTON
		// -------------------------------------------------------------------
		case PE_ButtonCommand:
		{
			bool sunken = on || down;
			bool useSmall = false;
			bool noFlat = false;
			bool hotButton = false;
			QColor btncolor = cg.button();

			int  name;

			if ( ( w <= smallButMaxW || h <= smallButMaxH || forceSmallMode) && !lightWeightMode )
			{
				if (sunken)
					name = thinkeramik_pushbutton_small_pressed;
				else
					name =  thinkeramik_pushbutton_small;
				forceSmallMode = false;
				useSmall = true;
			}
			else
			{
				if (sunken)
					name = thinkeramik_pushbutton_pressed;
				else
					name =  thinkeramik_pushbutton;
			}

			if (flags & Style_MouseOver && name == thinkeramik_pushbutton ) {
			      btncolor = cg.button().light(107);
			      if (!combobox && !btnprlcolstr.isNull()) {
				    btncolor = QColor(btnprlcolstr);
			      } else if (combobox && !cbxprlcolstr.isNull()) {
				    btncolor = QColor(cbxprlcolstr);
			      }
			      hotButton = true;
			}

			if ( !lightWeightMode
			   && ( (!combobox && usesgb && name == thinkeramik_pushbutton )
			      || (combobox && usesgc && name == thinkeramik_pushbutton ) ) ) {
			      name = thinkeramik_pushbutton_sg;
			}

			if (useShadowlessButton || lightWeightMode) {
			        if (name == thinkeramik_pushbutton)
				        name = thinkeramik_pushbutton_shadowless;
				else if  (name == thinkeramik_pushbutton_sg)
				        name = thinkeramik_pushbutton_sg_shadowless;
			}

			if (toolbarBlendWidget && !flatMode )
			{
				//Render the toolbar gradient.
				renderToolbarWidgetBackground(p, toolbarBlendWidget);

				//Draw and blend the actual bevel..
				ThinKeramik::RectTilePainter( name, false ).draw(p, r, btncolor, cg.background(),
					disabled, ThinKeramik::TilePainter::PaintFullBlend  );
				noFlat = true;
			}
			else if (!flatMode) {
				ThinKeramik::RectTilePainter( name, false ).draw(p, r, btncolor, cg.background(), disabled, pmode() );
				noFlat = true;
			} else {
				ThinKeramik::ScaledPainter( name + ThinKeramikTileCC, ThinKeramik::ScaledPainter::Vertical).draw(
					p, r, btncolor, cg.background(), disabled, pmode() );

				p->setPen(cg.button().light(75));

				p->drawLine(x, y, x2, y);
				p->drawLine(x, y, x, y2);
				p->drawLine(x, y2, x2, y2);
				p->drawLine(x2, y, x2, y2);
				flatMode = false;
			}

			
			if (!disabled && noFlat) {
			        QColor buttonBorderColor = cg.button().dark(buttonBorderDarkValue);
				if (useSmall) {
				        //buttonBorderColor = cg.button().dark(buttonBorderDarkValue-15);
				        //drawButtonBorder(p, r, buttonBorderColor, 1); // Smallbutton Border
				} else if (combobox) {
				        if (lightWeightMode) {
					        buttonBorderColor = getBlendColor(cg.background().dark(135), cg.dark(), 50);
					} else {
					        if (!comboboxBorderColStr.isNull()) {
						        buttonBorderColor = QColor(comboboxBorderColStr);
						}
					}
					drawButtonBorder(p, r, buttonBorderColor, 3); // Combobox Border
				} else {
				        if (!buttonBorderColStr.isNull()) {
					        buttonBorderColor = QColor(buttonBorderColStr);
					}
					drawButtonBorder(p, r, buttonBorderColor, 2); // Pushbutton Border
				}

				if (!useSmall && hotButton) {
					if ( (prelightInnerCircumOnly && !combobox)
					    || (prelightInnerCircumOnlyCbx && combobox) ){
					        p->save();
						p->setClipping( false );
						QRegion unPrlClip = getUnPrlRegion(r);
						p->setClipRegion( unPrlClip );
						ThinKeramik::RectTilePainter( name, false ).draw(p, r, cg.button(), cg.background(), disabled, pmode() );
						p->setClipping( false );
						p->restore();
					}

				}
			} else if (disabled && noFlat && combobox && lightWeightMode) {
			        QColor buttonBorderColor = getBlendColor(cg.background().dark(135), cg.dark(), 50);
				drawButtonBorder(p, r, buttonBorderColor, 3); // Combobox Border
			}

			combobox = false;

			break;

		}

		// BEVELS
		// -------------------------------------------------------------------
		case PE_ButtonBevel:
		{
			int x,y,w,h;
			r.rect(&x, &y, &w, &h);
			bool sunken = on || down;
			int x2 = x+w-1;
			int y2 = y+h-1;

			// Outer frame
			p->setPen(cg.shadow());
			p->drawRect(r);

			// Bevel
			p->setPen(sunken ? cg.mid() : cg.light());
			p->drawLine(x+1, y+1, x2-1, y+1);
			p->drawLine(x+1, y+1, x+1, y2-1);
			p->setPen(sunken ? cg.light() : cg.mid());
			p->drawLine(x+2, y2-1, x2-1, y2-1);
			p->drawLine(x2-1, y+2, x2-1, y2-1);

			if (w > 4 && h > 4)
			{
				if (sunken)
					p->fillRect(x+2, y+2, w-4, h-4, cg.button());
				else
					ThinKeramik::GradientPainter::renderGradient( p, QRect(x+2, y+2, w-4, h-4),
							cg.button(), flags & Style_Horizontal );
			}
			break;
		}


			// FOCUS RECT
			// -------------------------------------------------------------------
		case PE_FocusRect:
			//Qt may pass the background color to use for the focus indicator
			//here. This particularly happens within the iconview.
			//If that happens, pass it on to drawWinFocusRect() so it can
			//honor it
			if ( opt.isDefault() )
				p->drawWinFocusRect( r );
			else
				p->drawWinFocusRect( r, opt.color() );
			break;

			// HEADER SECTION
			// -------------------------------------------------------------------
		case PE_HeaderSection:
                {
		        QColor hsc      = cg.background().light(102);
			QColor hsc_down = cg.background().dark(102);
			if (usepbcbevel) {
			       hsc      = cg.button();
			       hsc_down = cg.button().dark(105);
			}

			if ( flags & Style_Down ) {
			        QColor atcolor;
			        if (kickerMode && highlightActiveTask) {
				        if (!atcolstr.isNull()) {
					        atcolor = QColor(atcolstr);
					} else {
					        atcolor = cg.highlight();
					}
					hsc_down = getBlendColor(atcolor, hsc_down, 15);
				}

				ThinKeramik::RectTilePainter( thinkeramik_listview_pressed, false ).draw( p, r, hsc_down, cg.background() );
				if (kickerMode && highlightActiveTask) {
					QRegion highlightClip;
					highlightClip |= QRect(x, y, w, 4);
					highlightClip |= QRect(x, y+h-4, w, 4);
					p->save();
					p->setClipping( false );
					p->setClipRegion( highlightClip );
					ThinKeramik::RectTilePainter( thinkeramik_listview_pressed, false ).draw( p, r, atcolor, cg.background() );
					p->setClipping( false );
					p->restore();
				}
			}
			else {
			      if (useflatbevel) {
				    ThinKeramik::RectTilePainter( thinkeramik_listview_flat, false ).draw( p, r, hsc, cg.background() );
			      } else if (usesgbv) {
				    ThinKeramik::RectTilePainter( thinkeramik_listview_sg, false ).draw( p, r, hsc, cg.background() );
			      } else {
				    ThinKeramik::RectTilePainter( thinkeramik_listview, false ).draw( p, r, hsc, cg.background() );
			      }
			}

			QColor borderColor1, borderColor2;
			if (kickerMode && surroundTaskbutton) {
			        borderColor2 = getBlendColor(cg.mid(), cg.dark(), 50);
				borderColor1 = borderColor2.light(105);
				p->save();
			        p->setPen(borderColor1);
				p->drawLine(x+1, y, x+w-2, y);
				p->drawLine(x, y+1, x, y+h-2);
				p->setPen(borderColor2);
				p->drawLine(x+w-1, y+1, x+w-1, y+h-2);
				p->drawLine(x+1, y+h-1, x+w-2, y+h-1);
				p->restore();
			} else {
			  /*
			        borderColor2 = getBlendColor(hsc.dark(135), cg.dark(), 50);
				p->save();
				p->setPen(borderColor2);
				p->drawLine(x+w-1, y+1, x+w-1, y+h-2);
				p->drawLine(x+1, y+h-1, x+w-2, y+h-1);
				p->restore();
			  */
			}


			break;
		}

		case PE_HeaderArrow:
			if ( flags & Style_Up )
				drawPrimitive( PE_ArrowUp, p, r, cg, Style_Enabled );
			else drawPrimitive( PE_ArrowDown, p, r, cg, Style_Enabled );
			break;


		// 	// SCROLLBAR
		// -------------------------------------------------------------------

		case PE_ScrollBarSlider:
		{
			bool horizontal = flags & Style_Horizontal;
			bool active = ( flags & Style_Active ) || ( flags & Style_Down );
			int name = ThinKeramikSlider1;
			unsigned int count = 3;

			int x,y,w,h;
			r.rect(&x, &y, &w, &h);

			if ( horizontal )
			{
				if ( w > ( loader.size( thinkeramik_scrollbar_hbar+ThinKeramikSlider1 ).width() +
				           loader.size( thinkeramik_scrollbar_hbar+ThinKeramikSlider4 ).width() +
				           loader.size( thinkeramik_scrollbar_hbar+ThinKeramikSlider3 ).width() + 2 ) )
					count = 5;
			}
			else if ( h > ( loader.size( thinkeramik_scrollbar_vbar+ThinKeramikSlider1 ).height() +
			                loader.size( thinkeramik_scrollbar_vbar+ThinKeramikSlider4 ).height() +
			                loader.size( thinkeramik_scrollbar_vbar+ThinKeramikSlider3 ).height() + 2 ) )
					count = 5;

			QColor col = sbscolor;
			if (customScrollMode || !highlightScrollBar)
				col = cg.button();

			if (!( flags & Style_Enabled ))
				ThinKeramik::ScrollBarPainter( name, count, horizontal ).draw( p, r, sbacolor.dark(135), cg.background(), false, pmode() );
			else if (!active)
				ThinKeramik::ScrollBarPainter( name, count, horizontal ).draw( p, r, col, cg.background(), false, pmode() );
			else
				ThinKeramik::ScrollBarPainter( name, count, horizontal ).draw( p, r, ThinKeramik::ColorUtil::lighten(col ,110), cg.background(), false, pmode() );

			break;
		}

		case PE_ScrollBarAddLine:
		{
			bool down = flags & Style_Down;

			QColor sbacol = sbacolor;
			if (customScrollMode)
				sbacol = cg.button();

			QColor sbafgcol = cg.buttonText();
			if (!customScrollMode && !sbafgcolstr.isNull())
			        sbafgcol = QColor(sbafgcolstr);

			if ( flags & Style_Horizontal )
			{
			        ThinKeramik::CenteredPainter painter(removeSublineArea ? thinkeramik_scrollbar_hbar_arrow2s : thinkeramik_scrollbar_hbar_arrow2 );
				painter.draw( p, r, down? sbafgcol : sbacol, cg.background(), disabled, pmode() );

				p->setPen( sbafgcol );
				if (removeSublineArea) {
				        QRect r2 = QRect(x+1, y, w-1, h);
				        drawThinKeramikArrow(p, cg, r2, PE_ArrowRight, down, !disabled, customScrollMode);
				} else {
				        p->drawLine(r.x()+r.width()/2 - 1, r.y() + r.height()/2 - 3,
						    r.x()+r.width()/2 - 1, r.y() + r.height()/2 + 3);

					drawThinKeramikArrow(p, cg, QRect(r.x(), r.y(), r.width()/2, r.height()), PE_ArrowLeft, down, !disabled, customScrollMode);

					drawThinKeramikArrow(p, cg, QRect(r.x()+r.width()/2, r.y(), r.width() - r.width()/2, r.height()),
							 PE_ArrowRight, down, !disabled, customScrollMode);
				}
			}
			else
			{
			        ThinKeramik::CenteredPainter painter(removeSublineArea ? thinkeramik_scrollbar_vbar_arrow2s : thinkeramik_scrollbar_vbar_arrow2 );
				painter.draw( p, r, down? sbafgcol : sbacol, cg.background(), disabled, pmode() );

				p->setPen( sbafgcol );
				if (removeSublineArea) {
				        drawThinKeramikArrow(p, cg, r, PE_ArrowDown, down, !disabled, customScrollMode);
				} else {
				        p->drawLine(r.x()+r.width()/2 - 4, r.y()+r.height()/2,
						    r.x()+r.width()/2 + 2, r.y()+r.height()/2);

					drawThinKeramikArrow(p, cg, QRect(r.x(), r.y(), r.width(), r.height()/2), PE_ArrowUp, down, !disabled, customScrollMode);

					drawThinKeramikArrow(p, cg, QRect(r.x(), r.y()+r.height()/2, r.width(), r.height() - r.height()/2),
							 PE_ArrowDown, down, !disabled, customScrollMode);
				}
				//drawThinKeramikArrow(p, cg, r, PE_ArrowUp, down, !disabled);
			}


			break;
		}

		case PE_ScrollBarSubLine:
		{
			bool down = flags & Style_Down;

			QColor sbacol = sbacolor;
			if (customScrollMode)
				sbacol = cg.button();

			QColor sbafgcol = cg.buttonText();
			if (!customScrollMode && !sbafgcolstr.isNull())
			        sbafgcol = QColor(sbafgcolstr);

			if ( flags & Style_Horizontal )
			{
				ThinKeramik::CenteredPainter painter(thinkeramik_scrollbar_hbar_arrow1 );
				painter.draw( p, r, down? sbafgcol : sbacol, cg.background(), disabled, pmode() );
				drawThinKeramikArrow(p, cg, r, PE_ArrowLeft, down, !disabled, customScrollMode);

			}
			else
			{
				ThinKeramik::CenteredPainter painter( thinkeramik_scrollbar_vbar_arrow1 );
				painter.draw( p, r, down? sbafgcol : sbacol, cg.background(), disabled, pmode() );
				drawThinKeramikArrow(p, cg, r, PE_ArrowUp, down, !disabled, customScrollMode);

			}
			break;
		}
		
		// CHECKBOX (indicator)
		// -------------------------------------------------------------------
		case PE_Indicator:
		case PE_IndicatorMask:

			if (flags & Style_On)
				ThinKeramik::ScaledPainter( thinkeramik_checkbox_on ).draw( p, r, cg.button(), cg.background(), disabled, pmode() );
			else if (flags & Style_Off)
				ThinKeramik::ScaledPainter( thinkeramik_checkbox_off ).draw( p, r, cg.button(), cg.background(), disabled, pmode() );
			else
				ThinKeramik::ScaledPainter( thinkeramik_checkbox_tri ).draw( p, r, cg.button(), cg.background(), disabled, pmode() );

			break;

			// RADIOBUTTON (exclusive indicator)
			// -------------------------------------------------------------------
		case PE_ExclusiveIndicator:
		case PE_ExclusiveIndicatorMask:
		{

			ThinKeramik::ScaledPainter( on ? thinkeramik_radiobutton_on : thinkeramik_radiobutton_off ).draw( p, r, cg.button(), cg.background(), disabled, pmode() );
			break;
		}

			// line edit frame
		case PE_PanelLineEdit:
		{

		        QColor frameMid, frameBG;
			if (combobox) {
			        frameMid = cg.button().dark(135);
				frameBG  = cg.button();
			} else {
			      /*
				frameMid = getBlendColor(cg.dark().light(117), cg.background().dark(135) , 50);
				frameMid = getBlendColor(frameMid, cg.highlight(), 50);
			      */
			        frameMid = getBlendColor(cg.background().dark(135), cg.highlight(), 50);
				frameBG  = cg.background();
			}
			p->setPen( frameMid );
			p->drawRect( r );
			if (opt.isDefault() || opt.lineWidth() != 1) {
			        p->setPen( cg.base() );
				p->drawRect(x + 1, y + 1, w - 2, h - 2);
			}
			p->setPen( getBlendColor( frameMid, frameBG, 50 ) );
			p->drawPoint(x ,y);
			p->drawPoint(x ,y + h - 1);
			p->drawPoint(x + w - 1, y);
			p->drawPoint(x + w - 1, y + h - 1);

			combobox = false;
			break;
		}

			// SPLITTER/DOCKWINDOW HANDLES
			// -------------------------------------------------------------------
		case PE_DockWindowResizeHandle:
		case PE_Splitter:
		{
			int x,y,w,h;
			r.rect(&x, &y, &w, &h);
			int x2 = x+w-1;
			int y2 = y+h-1;

			p->setPen(cg.background().dark(135));
			p->drawRect( r );
			p->setPen(cg.background());
			p->drawPoint(x, y);
			p->drawPoint(x2, y);
			p->drawPoint(x, y2);
			p->drawPoint(x2, y2);
			p->setPen(cg.background().light(110));
			p->drawLine(x+1, y+1, x+1, y2-1);
			p->drawLine(x+1, y+1, x2-1, y+1);
			p->setPen(cg.background().dark(110));
			p->drawLine(x2-1, y+1, x2-1, y2-1);
			p->drawLine(x+1, y2-1, x2-1, y2-1);
 			p->fillRect(x+2, y+2, w-4, h-4, cg.brush(QColorGroup::Background));
			break;
		}

 		case PE_PanelPopup: 
 		{
 		        if (!bordercolstr.isNull()) {
 			      QColor bordercolor = QColor(bordercolstr);
 			      p->setPen(bordercolor);
 			      p->drawRect(r);
 		        } else {
 			      QColor mbgcolor;
 			      if (!mbgcolstr.isNull()) {
 				    mbgcolor = QColor(mbgcolstr);
 			      } else {
 				    mbgcolor = cg.background().light(110);
 			      }
 			      int bdx, bdy, bdw, bdh;
 			      r.rect(&bdx, &bdy, &bdw, &bdh);
 			      p->setPen (mbgcolor.light(110) );
 			      p->drawRect(r);
 			      p->setPen (mbgcolor.dark(200) );
 			      p->drawLine(bdx, bdy + bdh - 1, bdx + bdw - 1, bdy + bdh - 1);
 			      p->drawLine(bdx + bdw - 1, bdy, bdx + bdw - 1, bdy + bdh - 1);
 			}
 			break;
 		}
 
	        case PE_StatusBarSection: {
 		        KStyle::drawPrimitive(pe, p, r, cg, flags, opt);
 			if (surroundSunkenPanel) {
 			        QColor panelBorderColor = getBlendColor(cg.background().dark(145), cg.dark(), 50);
 				if (!statusbarBorderColStr.isNull()) {
 				        panelBorderColor = QColor(statusbarBorderColStr);
 				}
 			        p->setPen(panelBorderColor);
 				p->drawRect(x, y, r.width(), r.height());
 				p->setPen(getBlendColor(panelBorderColor, cg.background(), 50) );
 				p->drawPoint(x ,y);
 				p->drawPoint(x ,y + h - 1);
 				p->drawPoint(x + w - 1, y);
 				p->drawPoint(x + w - 1, y + h - 1);
 			}
 		        break;
 		}

			// GENERAL PANELS
			// -------------------------------------------------------------------
	        case PE_PanelTabWidget:
		case PE_Panel:
		{
			if (kickerMode)
			{
				if (p->device() && p->device()->devType() == QInternal::Widget &&
											 QCString(static_cast<QWidget*>(p->device())->className()) == "FittsLawFrame" )
				{
					int x2 = x + r.width() - 1;
					int y2 = y + r.height() - 1;
					p->setPen(cg.dark());
					p->drawLine(x+1,y2,x2-1,y2);
					p->drawLine(x2,y+1,x2,y2);

					p->setPen( cg.light() );
					p->drawLine(x, y, x2, y);
					p->drawLine(x, y, x, y2);


					return;
				}
			}
			KStyle::drawPrimitive(pe, p, r, cg, flags, opt);

 			if (!kickerMode || pe == PE_PanelTabWidget) {
 			        QPen oldPen = p->pen();
 				if (flags & Style_Sunken) {
 				        if (surroundSunkenPanel) {
 					        p->setPen( cg.dark().light(105) ); // Edge
 						p->drawRect(x, y, r.width(), r.height());
 						p->setPen( cg.light() );
 						p->drawRect(x+1, y+1, r.width()-2, r.height()-2);
 						p->setPen(getBlendColor(cg.background(), cg.dark(), 70) );
 						p->drawLine(x+1, y+1, x+r.width()-3, y+1);
 						p->drawLine(x+1, y+1, x+1, y+r.height()-3);
 					}
 				} else {
 				        p->setPen( cg.dark().light(105) ); // Edge
 				        p->drawLine(x, y, x+r.width()-1, y);
 					p->drawLine(x, y, x, y+r.height()-1);
 					p->setPen( cg.background() );
 				        p->drawLine(x, y+r.height()-1,
 						    x+r.width()-1, y+r.height()-1);
 					p->drawLine(x+r.width()-1, y,
 						    x+r.width()-1, y+r.height()-1);
 
 					if (pe == PE_PanelTabWidget) {
 				            p->drawLine(x+1, y+1,
 							x+r.width()-3, y+1);
 					    p->drawLine(x+1, y+1,
 							x+1, y+r.height()-3);
 					}
 
 				}
 				p->setPen(oldPen);
 			}

			break;
		}
		case PE_WindowFrame:
		{
			bool sunken  = flags & Style_Sunken;
			int lw = opt.isDefault() ? pixelMetric(PM_DefaultFrameWidth)
				: opt.lineWidth();
			if (lw == 2)
			{
				QPen oldPen = p->pen();
				int x,y,w,h;
				r.rect(&x, &y, &w, &h);
				int x2 = x+w-1;
				int y2 = y+h-1;
				p->setPen(sunken ? cg.light() : cg.dark());
				p->drawLine(x, y2, x2, y2);
				p->drawLine(x2, y, x2, y2);
				p->setPen(sunken ? cg.mid() : cg.light());
				p->drawLine(x, y, x2, y);
				p->drawLine(x, y, x, y2);
				p->setPen(sunken ? cg.midlight() : cg.mid());
				p->drawLine(x+1, y2-1, x2-1, y2-1);
				p->drawLine(x2-1, y+1, x2-1, y2-1);
				p->setPen(sunken ? cg.dark() : cg.midlight());
				p->drawLine(x+1, y+1, x2-1, y+1);
				p->drawLine(x+1, y+1, x+1, y2-1);
				p->setPen(oldPen);
			} else
				KStyle::drawPrimitive(pe, p, r, cg, flags, opt);

			break;
		}


			// MENU / TOOLBAR PANEL
			// -------------------------------------------------------------------
		case PE_PanelMenuBar: 			// Menu
		case PE_PanelDockWindow:		// Toolbar
		{
 			int x,y,w,h;
 			r.rect(&x, &y, &w, &h);
 			int x2 = r.right();
 			int y2 = r.bottom();
 			if (w>h) {
 			      p->fillRect(r, cg.background());
 			      if (!disablebb) {
 			            p->setPen(cg.light());
 			            p->drawLine(x, y, x2, y);
 				    if (barBorderFollowsContrast) {
 				            p->setPen(getBlendColor(cg.mid(),cg.dark(),50));
 				    } else {
 				            p->setPen(cg.mid());
 				    }
 			            p->drawLine(x, y2, x2, y2);
 			      }
 			} else {
 			      p->fillRect(r, cg.background());
 			      if (!disablebb) {
 				    p->setPen(cg.light());
 				    p->drawLine(x, y, x, y2);
 				    if (barBorderFollowsContrast) {
 				            p->setPen(getBlendColor(cg.mid(),cg.dark(),50));
 				    } else {
 				            p->setPen(cg.mid());
 				    }
 				    p->drawLine(x2, y, x2, y2);
 			      }
 			}                     
			break;
		}

		// TOOLBAR SEPARATOR
		// -------------------------------------------------------------------
		case PE_DockWindowSeparator:
		{
		        p->fillRect(r, cg.background());

			if ( !(flags & Style_Horizontal) )
			{
				p->setPen(cg.mid());
				p->drawLine(4, r.height()/2, r.width()-5, r.height()/2);
				p->setPen(cg.light());
				p->drawLine(4, r.height()/2+1, r.width()-5, r.height()/2+1);
			}
			else
			{
				p->setPen(cg.mid());
				p->drawLine(r.width()/2, 4, r.width()/2, r.height()-5);
				p->setPen(cg.light());
				p->drawLine(r.width()/2+1, 4, r.width()/2+1, r.height()-5);
			}
			break;
		}

		default:
		{
			// ARROWS
			// -------------------------------------------------------------------
			if (pe >= PE_ArrowUp && pe <= PE_ArrowLeft)
			{
				QPointArray a;

				switch(pe)
				{
					case PE_ArrowUp:
						a.setPoints(QCOORDARRLEN(u_arrow), u_arrow);
						break;

					case PE_ArrowDown:
						a.setPoints(QCOORDARRLEN(d_arrow), d_arrow);
						break;

					case PE_ArrowLeft:
						a.setPoints(QCOORDARRLEN(l_arrow), l_arrow);
						break;

					default:
						a.setPoints(QCOORDARRLEN(r_arrow), r_arrow);
				}

				p->save();
				if ( flags & Style_Down )
					p->translate( pixelMetric( PM_ButtonShiftHorizontal ),
							pixelMetric( PM_ButtonShiftVertical ) );

				if ( flags & Style_Enabled )
				{
					a.translate( r.x() + r.width() / 2, r.y() + r.height() / 2 );
					p->setPen( cg.buttonText() );
					p->drawLineSegments( a );
				}
				else
				{
					a.translate( r.x() + r.width() / 2 + 1, r.y() + r.height() / 2 + 1 );
					p->setPen( cg.light() );
					p->drawLineSegments( a );
					a.translate( -1, -1 );
					p->setPen( cg.mid() );
					p->drawLineSegments( a );
				}
				p->restore();

			}
			else
				KStyle::drawPrimitive( pe, p, r, cg, flags, opt );
		}
	}
}

void ThinKeramikStyle::drawKStylePrimitive( KStylePrimitive kpe,
										  QPainter* p,
										  const QWidget* widget,
										  const QRect &r,
										  const QColorGroup &cg,
										  SFlags flags,
										  const QStyleOption &opt ) const
{
	bool disabled = ( flags & Style_Enabled ) == 0;
	int x, y, w, h;
	r.rect( &x, &y, &w, &h );

	switch ( kpe )
	{
		// SLIDER GROOVE
		// -------------------------------------------------------------------
		case KPE_SliderGroove:
		{
			const QSlider* slider = static_cast< const QSlider* >( widget );
			bool horizontal = slider->orientation() == Horizontal;

			if ( horizontal )
				ThinKeramik::RectTilePainter( thinkeramik_slider_hgroove, false ).draw(p, r, cg.button(), cg.background(), disabled);
			else
				ThinKeramik::RectTilePainter( thinkeramik_slider_vgroove, true, false ).draw( p, r, cg.button(), cg.background(), disabled);

			break;
		}

		// SLIDER HANDLE
		// -------------------------------------------------------------------
		case KPE_SliderHandle:
			{
				const QSlider* slider = static_cast< const QSlider* >( widget );
				bool horizontal = slider->orientation() == Horizontal;

				QColor hl = sbscolor;
				if (!disabled && flags & Style_Active)
					hl = ThinKeramik::ColorUtil::lighten(sbscolor,110);

				if (horizontal)
					ThinKeramik::ScaledPainter( thinkeramik_slider ).draw( p, r, disabled ? cg.button() : hl,
						Qt::black,  disabled, ThinKeramik::TilePainter::PaintFullBlend );
				else
					ThinKeramik::ScaledPainter( thinkeramik_vslider ).draw( p, r, disabled ? cg.button() : hl,
						Qt::black,  disabled, ThinKeramik::TilePainter::PaintFullBlend );
				break;
			}

		// TOOLBAR/GENERAL/KICKER HANDLE
		// -------------------------------------------------------------------
	        case KPE_ToolBarHandle:                 // TOOLBAR
	        case KPE_GeneralHandle:                 // GENERAL/KICKER
		{
			if (kpe == KPE_ToolBarHandle) {
				p->fillRect(r, cg.background());
			}

		        int x, y, w, h, k, l;
			r.rect(&x, &y, &w, &h);

			int baseOffset = 0;
			if (useSingleLineHandles) {
			        if (kpe == KPE_ToolBarHandle) {
				        baseOffset = (QApplication::reverseLayout() ? 2 : 0);
				} else {
				        baseOffset = 1;
				}
			}
			int dotOffset, loopOffset;

			if (flags & Style_Horizontal) {
 
			        for (l = 1; l <= (useSingleLineHandles ? 1 : 2); l++) {

				        if ( l == 1) {
						dotOffset  = baseOffset;
						loopOffset = 0;
					} else {
						dotOffset  = 3;
						loopOffset = 3;
					}

				        for (k = (h%6+3)/2+loopOffset; k <= h-(4+loopOffset); k = k+6) {
					        p->setPen(cg.mid().dark(105));
						p->drawLine(x+2+dotOffset, y+k, x+3+dotOffset, y+k);
						p->drawPoint(x+2+dotOffset, y+k+1);
						p->setPen(cg.mid().light(120));
						p->drawPoint(x+3+dotOffset, y+k+1);
						p->setPen(cg.mid().light(125));
						p->drawPoint(x+4+dotOffset, y+k+1);
						p->drawLine(x+3+dotOffset, y+k+2, x+4+dotOffset, y+k+2);
					}
				}
 
 			} else {
 
			        for (l = 1; l <= (useSingleLineHandles ? 1 : 2); l++) {

				        if ( l == 1) {
						dotOffset  = baseOffset;
						loopOffset = 0;
					} else {
						dotOffset  = 3;
						loopOffset = 3;
					}

					for (k = (w%6+3)/2+loopOffset; k <= w-(4+loopOffset); k = k+6) {
					        p->setPen(cg.mid().dark(105));
						p->drawLine(x+k, y+2+dotOffset, x+k, y+3+dotOffset);
						p->drawPoint(x+k+1, y+2+dotOffset);
						p->setPen(cg.mid().light(120));
						p->drawPoint(x+k+1, y+3+dotOffset);
						p->setPen(cg.mid().light(125));
						p->drawPoint(x+k+1, y+4+dotOffset);
						p->drawLine(x+k+2, y+3+dotOffset, x+k+2, y+4+dotOffset);
					}
				}

 			}

			break;
		}


		default:
			KStyle::drawKStylePrimitive( kpe, p, widget, r, cg, flags, opt);
	}
}

bool ThinKeramikStyle::isFormWidget(const QWidget* widget) const
{
	//Form widgets are in the KHTMLView, but that has 2 further inner levels
	//of widgets - QClipperWidget, and outside of that, QViewportWidget
	QWidget* potentialClipPort = widget->parentWidget();
	if ( !potentialClipPort || potentialClipPort->isTopLevel() )
	return false;

	QWidget* potentialViewPort = potentialClipPort->parentWidget();
	if (!potentialViewPort || potentialViewPort->isTopLevel() ||
			qstrcmp(potentialViewPort->name(), "qt_viewport") )
		return false;

	QWidget* potentialKHTML  = potentialViewPort->parentWidget();
	if (!potentialKHTML || potentialKHTML->isTopLevel() ||
			qstrcmp(potentialKHTML->className(), "KHTMLView") )
		return false;


	return true;
}

void ThinKeramikStyle::drawControl( ControlElement element,
								  QPainter *p,
								  const QWidget *widget,
								  const QRect &r,
								  const QColorGroup &cg,
								  SFlags flags,
								  const QStyleOption& opt ) const
{
	bool disabled = ( flags & Style_Enabled ) == 0;
	int x, y, w, h;
	r.rect( &x, &y, &w, &h );

	switch (element)
	{
		// PUSHBUTTON
		// -------------------------------------------------------------------
		case CE_PushButton:
		{
			const QPushButton* btn = static_cast< const QPushButton* >( widget );

			if (isFormWidget(btn))
				formMode = true;

			if ( widget == hoverWidget )
				flags |= Style_MouseOver;

			if ( btn->isFlat( ) )
				flatMode = true;

			if ( btn->isDefault( ) && !flatMode )
			{
				drawPrimitive( PE_ButtonDefault, p, r, cg, flags );
			}
			else
			{
				if (widget->parent() && widget->parent()->inherits("QToolBar"))
					toolbarBlendWidget = widget;

				combobox = false;
				drawPrimitive( PE_ButtonCommand, p, r, cg, flags );
				toolbarBlendWidget = 0;
			}

			formMode = false;
			break;
		}


		// PUSHBUTTON LABEL
		// -------------------------------------------------------------------
		case CE_PushButtonLabel:
		{
			const QPushButton* button = static_cast<const QPushButton *>( widget );
			bool active = button->isOn() || button->isDown();
			bool cornArrow = false;

			// Shift button contents if pushed.
			if ( active )
			{
				x += pixelMetric(PM_ButtonShiftHorizontal, widget);
				y += pixelMetric(PM_ButtonShiftVertical, widget);
				flags |= Style_Sunken;
			}

			// Does the button have a popup menu?
			if ( button->isMenuButton() )
			{
				int dx = pixelMetric( PM_MenuButtonIndicator, widget );
				if ( button->iconSet() && !button->iconSet()->isNull()  &&
					(dx + button->iconSet()->pixmap (QIconSet::Small, QIconSet::Normal, QIconSet::Off ).width()) >= w )
				{
					cornArrow = true; //To little room. Draw the arrow in the corner, don't adjust the widget
				}
				else
				{
					drawPrimitive( PE_ArrowDown, p, visualRect( QRect(x + w - dx - 8, y + 2, dx, h - 4), r ),
							   cg, flags, opt );
					w -= dx;
				}
			}

			// Draw the icon if there is one
			if ( button->iconSet() && !button->iconSet()->isNull() )
			{
				QIconSet::Mode  mode  = QIconSet::Disabled;
				QIconSet::State state = QIconSet::Off;

				if (button->isEnabled())
					mode = button->hasFocus() ? QIconSet::Active : QIconSet::Normal;
				if (button->isToggleButton() && button->isOn())
					state = QIconSet::On;

				QPixmap pixmap = button->iconSet()->pixmap( QIconSet::Small, mode, state );

				if (button->text().isEmpty() && !button->pixmap())
					p->drawPixmap( x + w/2 - pixmap.width()/2, y + h / 2 - pixmap.height() / 2,
									pixmap );
				else
					p->drawPixmap( x + 4, y + h / 2 - pixmap.height() / 2, pixmap );

				if (cornArrow) //Draw over the icon
					drawPrimitive( PE_ArrowDown, p, visualRect( QRect(x + w - 6, x + h - 6, 7, 7), r ),
							   cg, flags, opt );


				int  pw = pixmap.width();
				x += pw + 4;
				w -= pw + 4;
			}

			// Make the label indicate if the button is a default button or not
			if (useshadowtxtB && button->isEnabled()) {
			      drawItem( p, QRect(x+1, y+1, w, h), AlignCenter | ShowPrefix, button->colorGroup(),
				  		      button->isEnabled(), button->pixmap(), button->text(), -1,
						      &button->colorGroup().button().dark(130) );
			}
			drawItem( p, QRect(x, y, w, h), AlignCenter | ShowPrefix, button->colorGroup(),
						button->isEnabled(), button->pixmap(), button->text(), -1,
						&button->colorGroup().buttonText() );



			if ( flags & Style_HasFocus )
				drawPrimitive( PE_FocusRect, p,
				               visualRect( subRect( SR_PushButtonFocusRect, widget ), widget ),
				               cg, flags );
			break;
		}

		case CE_ToolButtonLabel:
		{
		    //const QToolButton *toolbutton = static_cast<const QToolButton * >(widget);
			bool onToolbar = widget->parentWidget() && widget->parentWidget()->inherits( "QToolBar" );
			QRect nr = r;

			if (!onToolbar)
			{
				if (widget->parentWidget() &&
				 !qstrcmp(widget->parentWidget()->name(),"qt_maxcontrols" ) )
				{
					//Make sure we don't cut into the endline
					if (!qstrcmp(widget->name(), "close"))
					{
						nr.addCoords(0,0,-1,0);
						p->setPen(cg.dark());
						p->drawLine(r.x() + r.width() - 1, r.y(),
								r.x() + r.width() - 1, r.y() + r.height() - 1 );
					}
				}
				//else if ( w > smallButMaxW && h > smallButMaxH )
				//	nr.setWidth(r.width()-2); //Account for shadow
			}

			KStyle::drawControl(element, p, widget, nr, cg, flags, opt);
			break;
		}

		case CE_TabBarTab:
		{
			const QTabBar* tabBar = static_cast< const QTabBar* >( widget );

			bool bottom = tabBar->shape() == QTabBar::RoundedBelow ||
			              tabBar->shape() == QTabBar::TriangularBelow;

			int notCornerAdj = 0;
			if( tabBar->parent()->inherits("QTabWidget") ) {
			        const QTabWidget *tw = (const QTabWidget*)tabBar->parent();
				// is there a corner widget in the (top) left edge?
				QWidget *cw = tw->cornerWidget(Qt::TopLeft);
				if(cw)
				        notCornerAdj = 1;
			}

			QColor atcolor;
			if (!atcolstr.isNull()) {
			      atcolor = QColor(atcolstr);
			} else {
			      atcolor = cg.highlight();
			}

			if ( flags & Style_Selected )
			{
			        QColor tabColor;
				if (useBGCTab) {
				        tabColor = cg.background().light(103);
				} else {
				        tabColor = cg.button().light(110);
				}
				QRect tabRect = r;
				//If not the last tab, readjust the painting to be one pixel wider
				//to avoid a doubled line
				if (  ( (tabBar->indexOf( opt.tab()->identifier() ) != (tabBar->count() - 1) )
					 && !QApplication::reverseLayout() )
				   || ( (tabBar->indexOf( opt.tab()->identifier() ) != 0 )
					 && QApplication::reverseLayout() )  ) {
					tabRect.setWidth( tabRect.width() + 1);
				} else if ( (tabBar->indexOf( opt.tab()->identifier() ) == 0 )
					    && QApplication::reverseLayout() ) {
					tabRect.setWidth( tabRect.width() - 1);
				}

				tabRect.rect( &x, &y, &w, &h );

				ThinKeramik::ActiveTabPainter( bottom ).draw( p, tabRect, tabColor, cg.background(), !tabBar->isEnabled(), pmode());
				if ( !bottom ) {
				        if (flattenActiveTab) {
					      p->fillRect(x+2, y+3, w-3, h-3, cg.background());
					      p->setPen( cg.background() );
					      p->drawLine( x + 2, y + 2, x + w - 3, y + 2 );
					      p->drawLine( x + 3, y + 1, x + w - 4, y + 1 );
					      p->setPen( getBlendColor(cg.dark(), cg.light(), 50 ) );
					      p->drawLine( x+w-2, y+3, x+w-2, y+h-3);
					}

					if ( !disableul ) {
					      p->setPen( atcolor );
					      p->drawLine( x + 3, y + 1, x + w - 4, y + 1 );
					      p->setPen( atcolor.light(120) );
					      p->drawLine( x + 2, y + 2, x + w - 3, y + 2 );
					}
					p->setPen( cg.background() );
					p->drawLine( x, y + h - 1, x + w - 1, y + h - 1 );

					p->setPen( cg.dark().light(105) ); // Edge
					if (tabBar->indexOf( opt.tab()->identifier() ) == 0) {
					      if (QApplication::reverseLayout()) {
						    p->drawLine(x, y+3, x, y+h-2);
						    p->drawLine(x+w-1, y+3, x+w-1, y+h-1-notCornerAdj);
					      } else {
						    p->drawLine(x, y+3, x, y+h-1-notCornerAdj);
						    p->drawLine(x+w-1, y+3, x+w-1, y+h-2);
					      }
					} else {
					      p->drawLine(x, y+3, x, y+h-2);
					      p->drawLine(x+w-1, y+3, x+w-1, y+h-2);
					}
					p->drawLine(x+3, y, x+w-4, y);
					p->drawPoint(x+2, y+1);
					p->drawPoint(x+w-3, y+1);
					p->drawPoint(x+1, y+2);
					p->drawPoint(x+w-2, y+2);
				} else {
					p->setPen( cg.dark() );
					p->drawLine(x, y, x, y+h-4);
					p->drawLine(x+w-1, y, x+w-1, y+h-4);
					p->drawLine(x+3, y+h-1, x+w-4, y+h-1);
					p->drawPoint(x+2, y+h-2);
					p->drawPoint(x+w-3, y+h-2);
					p->drawPoint(x+1, y+h-3);
					p->drawPoint(x+w-2, y+h-3);
				}
			}
			else
			{
			        QRect tabRect = r;
		 	        if ( (tabBar->indexOf( opt.tab()->identifier() ) == 0 )
				     && QApplication::reverseLayout() ) {
					tabRect.setWidth( tabRect.width() - 1);
				}
				tabRect.rect( &x, &y, &w, &h );

			        QColor tabColor;
				if (useBGCTab) {
				        tabColor = cg.background().dark(107);
				} else {
				        tabColor = cg.button();
				}

				ThinKeramik::InactiveTabPainter::Mode mode;
				int index = tabBar->indexOf( opt.tab()->identifier() );
				if ( index == 0 ) mode = ThinKeramik::InactiveTabPainter::First;
				else if ( index == tabBar->count() - 1 ) mode = ThinKeramik::InactiveTabPainter::Last;
				else mode = ThinKeramik::InactiveTabPainter::Middle;

				if ( bottom )
				{
					ThinKeramik::InactiveTabPainter( mode, bottom ).draw( p, x, y, w, h - 3, tabColor, cg.background(), disabled, pmode() );
					p->setPen( cg.dark() );
					p->drawLine( x, y, x + w - 1, y );
					if (QApplication::reverseLayout() ) {
					      if (mode == ThinKeramik::InactiveTabPainter::First) {
						    p->drawLine( x, y, x, y+h-4 );
						    p->drawLine( x+w-1, y, x+w-1, y+h-8 );
						    p->drawLine( x, y+h-4, x+w-4 , y+h-4);
						    p->drawPoint( x+w-3, y+h-5);
						    p->drawLine( x+w-2, y+h-6, x+w-2, y+h-7);
					      } else if (mode == ThinKeramik::InactiveTabPainter::Middle) {
						    p->drawLine( x, y+h-4, x+w-1 , y+h-4);
						    p->drawLine( x, y, x, y+h-4 );
					      } else {
						    p->drawLine( x, y, x, y+h-6);
						    p->drawLine( x+2, y+h-4, x+w-1 , y+h-4);
						    p->drawPoint( x+1, y+h-5);
					      }
					} else {
					      if (mode == ThinKeramik::InactiveTabPainter::First) {
						    p->drawLine( x, y, x, y+h-6);
						    p->drawLine( x+2, y+h-4, x+w-1 , y+h-4);
						    p->drawPoint( x+1, y+h-5);
					      } else if (mode == ThinKeramik::InactiveTabPainter::Middle) {
						    p->drawLine( x, y+h-4, x+w-1 , y+h-4);
						    p->drawLine( x, y, x, y+h-4 );
					      } else {
						    p->drawLine( x, y, x, y+h-4 );
						    p->drawLine( x+w-1, y, x+w-1, y+h-8 );
						    p->drawLine( x, y+h-4, x+w-4 , y+h-4);
						    p->drawPoint( x+w-3, y+h-5);
						    p->drawLine( x+w-2, y+h-6, x+w-2, y+h-7);
					      }
					}
				}
				else
				{
					ThinKeramik::InactiveTabPainter( mode, bottom ).draw( p, x, y + 3, w, h - 3, tabColor, cg.background(), disabled, pmode() );

					p->setPen( cg.background() );
					p->drawLine( x, y + h - 1, x + w - 1, y + h - 1 );

					p->setPen( cg.dark().light(105) ); // Edge
					p->drawLine( x, y + h - 2, x + w - 1, y + h - 2 );
					if (QApplication::reverseLayout() ) {
					      if (mode == ThinKeramik::InactiveTabPainter::First) {
						    p->drawLine( x, y+3, x, y+h-2 );
						    p->drawLine( x+w-1, y+7, x+w-1, y+h-1-notCornerAdj );
						    p->drawLine( x, y+3, x+w-4 , y+3);
						    p->drawPoint( x+w-3, y+4);
						    p->drawLine( x+w-2, y+5, x+w-2, y+6);
					      } else if (mode == ThinKeramik::InactiveTabPainter::Middle) {
						    p->drawLine( x, y+3, x+w-1 , y+3);
						    p->drawLine( x, y+3, x, y+h-2 );
					      } else {
						    p->drawLine( x, y+5, x, y+h-2 );
						    p->drawLine( x+2, y+3, x+w-1 , y+3);
						    p->drawPoint( x+1, y+4);
					      }
					} else {
					      if (mode == ThinKeramik::InactiveTabPainter::First) {
						    p->drawLine( x, y+5, x, y+h-1-notCornerAdj );
						    p->drawLine( x+2, y+3, x+w-1 , y+3);
						    p->drawPoint( x+1, y+4);
					      } else if (mode == ThinKeramik::InactiveTabPainter::Middle) {
						    p->drawLine( x, y+3, x+w-1 , y+3);
						    p->drawLine( x, y+3, x, y+h-2 );
					      } else {
						    p->drawLine( x, y+3, x, y+h-2 );
						    p->drawLine( x+w-1, y+7, x+w-1, y+h-2 );
						    p->drawLine( x, y+3, x+w-4 , y+3);
						    p->drawPoint( x+w-3, y+4);
						    p->drawLine( x+w-2, y+5, x+w-2, y+6);
					      }
					}

				}
			}

			break;
		}
	        case CE_TabBarLabel:{
		      if(opt.isDefault())
			    return;
		      QTab *t = opt.tab();
		      if(useshadowtxtT && !t->text().isEmpty()){
			    const QColor tmp(cg.button().dark(130));
			    drawItem(p, QRect(r.x()+1, r.y()+1, r.width(), r.height()),
				     AlignCenter | ShowPrefix, cg,
				     flags & Style_Enabled, 0, t->text(), -1, &tmp);
		      }
		      drawItem(p, r, AlignCenter | ShowPrefix, cg, flags & Style_Enabled,
			       0, t->text());
		      break;
		}

		case CE_DockWindowEmptyArea:
		case CE_MenuBarEmptyArea:
		{
		        p->fillRect(r, cg.background());
			break;
		}
		// MENUBAR ITEM (sunken panel on mouse over)
		// -------------------------------------------------------------------
		case CE_MenuBarItem:
		{
			QMenuBar  *mb = (QMenuBar*)widget;
			QMenuItem *mi = opt.menuItem();
			QRect      pr = mb->rect();

			bool active  = flags & Style_Active;
			bool focused = flags & Style_HasFocus;

			if ( active && focused ) {
			        int x, y, w, h;
				r.rect(&x, &y, &w, &h);
			        int x2 = x+w-1;
				int y2 = y+h-1;
				QColor mi_color = cg.background().dark(160);
				p->setPen(mi_color);
				p->drawRect(x, y, w, h);

				p->setPen(mi_color.light(115));
				p->drawPoint(x+1, y);
				p->drawPoint(x, y+1);
				p->drawPoint(x2-1, y);
				p->drawPoint(x2, y+1);
				p->drawPoint(x2, y2-1);
				p->drawPoint(x2-1, y2);
				p->drawPoint(x, y2-1);
				p->drawPoint(x+1, y2);

				p->setPen(cg.background());
				p->drawPoint(x, y);
				p->drawPoint(x2, y);
				p->drawPoint(x2, y2);
				p->drawPoint(x, y2);

				p->fillRect(QRect(x+1, y+1, w-2, h-2),cg.background().dark(110));
			} else {
			        p->fillRect(pr, cg.background());
			}

			if (useshadowtxtMB) {
                             const QColor shadowColor(cg.background().dark(130));
			      drawItem( p, QRect(r.x()+1, r.y()+1, r.width(), r.height()), AlignCenter | AlignVCenter | ShowPrefix
				              | DontClip | SingleLine, cg, flags & Style_Enabled,
					      mi->pixmap(), mi->text(), -1, &shadowColor);
			}

			drawItem( p, r, AlignCenter | AlignVCenter | ShowPrefix
					| DontClip | SingleLine, cg, flags & Style_Enabled,
					mi->pixmap(), mi->text() );

			break;
		}


		// POPUPMENU ITEM
		// -------------------------------------------------------------------
		case CE_PopupMenuItem: {
			const QPopupMenu *popupmenu = static_cast< const QPopupMenu * >( widget );
			QRect main = r;

			QMenuItem *mi = opt.menuItem();

			if ( !mi )
			{
				// Don't leave blank holes if we set NoBackground for the QPopupMenu.
				// This only happens when the popupMenu spans more than one column.
				if (! ( widget->erasePixmap() && !widget->erasePixmap()->isNull() ) )
					p->fillRect( r, cg.background().light( 105 ) );

				break;
			}
			int  tab        = opt.tabWidth();
			int  checkcol   = opt.maxIconWidth();
			bool enabled    = mi->isEnabled();
			bool checkable  = popupmenu->isCheckable();
			bool active     = flags & Style_Active;
			bool etchtext   = styleHint( SH_EtchDisabledText );
			bool reverse    = QApplication::reverseLayout();
			if ( checkable )
				checkcol = QMAX( checkcol, 20 );
			int x,  y, w, h;
			r.rect(&x, &y, &w, &h);

			// Draw the menu item background
			// Custom color
			QColor mfgcolor, mbgcolor, stpcolor;
			if (!mfgcolstr.isNull()) {
				    mfgcolor = QColor(mfgcolstr);
			} else {
			            mfgcolor = cg.text();
			}
			if (!mbgcolstr.isNull()) {
				    mbgcolor = QColor(mbgcolstr);
			} else {
				    mbgcolor = cg.background().light(110);
			}
			if (!stpcolstr.isNull()) {
				    stpcolor = QColor(stpcolstr);
			} else {
				    stpcolor = cg.mid();
			}

			int stripewidth = QMAX( defaultStripeWidth, currentStripeWidth );

			// Custom color for selected menu item
			QColor mselfgcolor, mselbgcolor, mselshdcolor;
			if (!mselfgcolstr.isNull()) {
				    mselfgcolor = QColor(mselfgcolstr);
			} else {
			            mselfgcolor = cg.highlightedText();
			}
			if (!mselbgcolstr.isNull()) {
				    mselbgcolor = QColor(mselbgcolstr);
			} else {
			            mselbgcolor = cg.highlight();
			}
			if (!mselshdcolstr.isNull()) {
				    mselshdcolor = QColor(mselshdcolstr);
			} else {
			            mselshdcolor = cg.highlight().dark(130);
			}

			// Draw the transparency pixmap
			if ( widget->erasePixmap() && !widget->erasePixmap()->isNull() ) {
			      p->drawPixmap( main.topLeft(), *widget->erasePixmap(), main );
			}

			// Draw a solid background
			else {
			      if (!reverse) {
				    if (enablems3d) {
				          ThinKeramik::GradientPainter::renderGradient2( p,
						   QRect(x, y, stripewidth, h), stpcolor.dark(105), false, true, strong3dms);
				    } else {
				          p->fillRect( x, y, stripewidth, h, stpcolor );
				    }
				    p->fillRect( x + stripewidth, y, w - stripewidth, h, mbgcolor );
			      } else { // i wonder why exactly +1 (diego)
				    if (enablems3d) {
				          ThinKeramik::GradientPainter::renderGradient2( p,
                                                   QRect(w - stripewidth, y, stripewidth + 1, h), stpcolor.dark(105), false, true, strong3dms);
				    } else {
				          p->fillRect( w - stripewidth, y, stripewidth + 1, h, stpcolor );
				    }
				    p->fillRect( x, y, w - stripewidth, h, mbgcolor );
			      }

			}
			if (active) {
			      if (enabled) {
				    p->setPen(mselbgcolor.dark(150));
				    p->drawRect( QRect(x + 1, y + 1, w - 2, h - 2) );
				    if (enablemi3d) {
				          ThinKeramik::GradientPainter::renderGradient2( p,
                                                   QRect(x + 2, y + 2, w - 4, h - 4), mselbgcolor.dark(105), true, false, strong3dmi);
				    } else {
				          p->fillRect( QRect(x + 2, y + 2, w - 4, h - 4), mselbgcolor);
				    }
			      } else {
				    p->drawWinFocusRect( r );
			      }
			}

			// Are we a menu item separator?

			if ( mi->isSeparator() )
			{
			        if (menuSepColStr.isNull()) {
				        p->setPen( mbgcolor.dark(133) );
				} else {
				        p->setPen( QColor(menuSepColStr) );
				}
				p->drawLine( main.x()+ stripewidth + 5, main.y() + 1, main.width()-10, main.y() + 1 );
				break;
			}

			QRect cr = visualRect( QRect( x + 2, y + 2, checkcol - 1, h - 4 ), r );
			// Do we have an icon?
			if ( mi->iconSet() )
			{
				QIconSet::Mode mode;



				// Select the correct icon from the iconset
				if ( active )
					mode = enabled ? QIconSet::Active : QIconSet::Disabled;
				else
					mode = enabled ? QIconSet::Normal : QIconSet::Disabled;

				// Do we have an icon and are checked at the same time?
				// Then draw a "pressed" background behind the icon
				if ( checkable && /*!active &&*/ mi->isChecked() )
					qDrawShadePanel( p, cr.x(), cr.y(), cr.width(), cr.height(),
									 cg, true, 1, &cg.brush(QColorGroup::Midlight) );
				// Draw the icon
				QPixmap pixmap = mi->iconSet()->pixmap( QIconSet::Small, mode );
				QRect pmr( 0, 0, pixmap.width(), pixmap.height() );
				pmr.moveCenter( cr.center() );
				p->drawPixmap( pmr.topLeft(), pixmap );
			}

			// Are we checked? (This time without an icon)
			else if ( checkable && mi->isChecked() )
			{
				// We only have to draw the background if the menu item is inactive -
				// if it's active the "pressed" background is already drawn
			//	if ( ! active )
					qDrawShadePanel( p, cr.x(), cr.y(), cr.width(), cr.height(), cg, true, 1,
					                 &cg.brush(QColorGroup::Midlight) );

				// Draw the checkmark
				SFlags cflags = Style_Default;
				cflags |= active ? Style_Enabled : Style_On;

				drawPrimitive( PE_CheckMark, p, cr, cg, cflags );
			}

			// Time to draw the menu item label...
			int xm = itemFrame + checkcol + itemHMargin; // X position margin

			int xp = reverse ? // X position
					x + tab + rightBorder + itemHMargin + itemFrame - 1 :
					x + xm;

			int offset = reverse ? -1 : 1;	// Shadow offset for etched text

			// Label width (minus the width of the accelerator portion)
			int tw = w - xm - tab - arrowHMargin - itemHMargin * 3 - itemFrame + 1;

			// Set the color for enabled and disabled text
			// (used for both active and inactive menu items)
			p->setPen( enabled ? cg.buttonText() : cg.mid() );

			// This color will be used instead of the above if the menu item
			// is active and disabled at the same time. (etched text)
			QColor discol = cg.mid();

			// Does the menu item draw it's own label?
			if ( mi->custom() ) {
				int m = itemVMargin;
				// Save the painter state in case the custom
				// paint method changes it in some way
				p->save();

				// Draw etched text if we're inactive and the menu item is disabled
				if ( etchtext && !enabled && !active ) {
					p->setPen( cg.light() );
					mi->custom()->paint( p, cg, active, enabled, xp+offset, y+m+1, tw, h-2*m );
					p->setPen( discol );
				}
				mi->custom()->paint( p, cg, active, enabled, xp, y+m, tw, h-2*m );
				p->restore();
			}
			else {
				// The menu item doesn't draw it's own label
				QString s = mi->text();
				// Does the menu item have a text label?
				if ( !s.isNull() ) {
					int t = s.find( '\t' );
					int m = itemVMargin;
					int text_flags = AlignVCenter | ShowPrefix | DontClip | SingleLine;
					text_flags |= reverse ? AlignRight : AlignLeft;

					//QColor draw = cg.text();
					QColor draw = (active && enabled) ? mselfgcolor : mfgcolor;
					p->setPen(draw);


					// Does the menu item have a tabstop? (for the accelerator text)
					if ( t >= 0 ) {
						int tabx = reverse ? x + rightBorder + itemHMargin + itemFrame :
							x + w - tab - rightBorder - itemHMargin - itemFrame;

						// Draw the right part of the label (accelerator text)
						if ( etchtext && !enabled ) {
							// Draw etched text if we're inactive and the menu item is disabled
							p->setPen( cg.light() );
							p->drawText( tabx+offset, y+m+1, tab, h-2*m, text_flags, s.mid( t+1 ) );
							p->setPen( discol );
						}

						if (useshadowtxtM && enabled) {
						      if (active)
						            p->setPen(mselshdcolor);
						      else
						            p->setPen(cg.background().dark(130));
						      p->drawText( tabx+1, y+m+1, tab, h-2*m, text_flags, s.mid( t+1 ) );
						      p->setPen(draw);
						}
						
						p->drawText( tabx, y+m, tab, h-2*m, text_flags, s.mid( t+1 ) );
						s = s.left( t );
					}

					// Draw the left part of the label (or the whole label
					// if there's no accelerator)
					if ( etchtext && !enabled ) {
						// Etched text again for inactive disabled menu items...
						p->setPen( cg.light() );
						p->drawText( xp+offset, y+m+1, tw, h-2*m, text_flags, s, t );
						p->setPen( discol );
					}

					if (useshadowtxtM && enabled) {
					      if (active) {
					            p->setPen(mselshdcolor);
						    if (!disableEmpShadowM) {
						          p->drawText( xp+2, y+m+1, tw, h-2*m, text_flags, s, t );
						    }
						    p->drawText( xp+1, y+m+1, tw, h-2*m, text_flags, s, t );
					      } else {
						    p->setPen(cg.background().dark(130));
						    p->drawText( xp+1, y+m+1, tw, h-2*m, text_flags, s, t );
					      }
					      
					      p->setPen(draw);
					}

					p->drawText( xp, y+m, tw, h-2*m, text_flags, s, t );

					p->setPen(cg.text());

				}

				// The menu item doesn't have a text label
				// Check if it has a pixmap instead
				else if ( mi->pixmap() ) {
					QPixmap *pixmap = mi->pixmap();

					// Draw the pixmap
					if ( pixmap->depth() == 1 )
						p->setBackgroundMode( OpaqueMode );

					int diffw = ( ( w - pixmap->width() ) / 2 )
									+ ( ( w - pixmap->width() ) % 2 );
					p->drawPixmap( x+diffw, y+itemFrame, *pixmap );

					if ( pixmap->depth() == 1 )
						p->setBackgroundMode( TransparentMode );
				}
			}

			// Does the menu item have a submenu?
			if ( mi->popup() ) {
				PrimitiveElement arrow = reverse ? PE_ArrowLeft : PE_ArrowRight;
				int dim = pixelMetric(PM_MenuButtonIndicator) - itemFrame;
				QRect vr = visualRect( QRect( x + w - arrowHMargin - itemFrame - dim,
							y + h / 2 - dim / 2, dim, dim), r );

				// Draw an arrow at the far end of the menu item
				if ( active ) {
					if ( enabled )
						discol = cg.buttonText();

					QColorGroup g2( discol, cg.highlight(), white, white,
									enabled ? white : discol, discol, white );

					drawPrimitive( arrow, p, vr, g2, Style_Enabled );
				} else
					drawPrimitive( arrow, p, vr, cg,
							enabled ? Style_Enabled : Style_Default );
			}
			break;
		}

		// PROGRESSBAR
		// ------------------------------------------------------------------------
		case CE_ProgressBarGroove: {
			QRect fr = subRect(SR_ProgressBarGroove, widget);
			QPen oldPen = p->pen();

			int x, y, w, h;
			fr.rect(&x, &y, &w, &h);
			int x2 = x+w-1;
			int y2 = y+h-1;
			QColor pbg_color = getBlendColor(cg.background().dark(145), cg.dark(), 50);
			p->setPen(pbg_color);
			p->drawLine(x, y+1, x, y2-1);
			p->drawLine(x+1, y, x2-1, y);
			p->drawLine(x2, y+1, x2, y2-1);
			p->drawLine(x+1, y2, x2-1, y2);

			p->setPen(pbg_color.light(115));
			p->drawPoint(x+1, y);
			p->drawPoint(x, y+1);
			p->drawPoint(x2-1, y);
			p->drawPoint(x2, y+1);
			p->drawPoint(x2, y2-1);
			p->drawPoint(x2-1, y2);
			p->drawPoint(x, y2-1);
			p->drawPoint(x+1, y2);

			if (!pggColor.isNull() && (flags & Style_Enabled) ) {
			        QColor proggrooveColor = QColor(pggColor);
			        p->fillRect(QRect(x+1, y+1, w-2, h-2),proggrooveColor);
				p->setPen(getBlendColor(proggrooveColor.light(115), cg.light(), 50));
			} else {
			        p->setPen(cg.light());
			}

			p->drawLine(x2-1, y+1, x2-1, y2-1);
			p->drawLine(x+1, y2-1, x2-1, y2-1);

			p->setPen(oldPen);

			break;
		}

		case CE_ProgressBarContents: {
			const QProgressBar* pb = (const QProgressBar*)widget;
			QRect cr = subRect(SR_ProgressBarContents, widget);
			double progress = pb->progress();
			bool reverse = QApplication::reverseLayout();
			int steps = pb->totalSteps();

			QColor progbarColor = cg.highlight();
			if (!pgbColor.isNull()) {
			        progbarColor = QColor(pgbColor);
			}

			if (!(flags & Style_Enabled)) {
			        progbarColor = progbarColor.dark(120);
			}

			if (!cr.isValid())
				return;

			// Draw progress bar
			if (progress > 0 || steps == 0) {
				double pg = (steps == 0) ? 0.1 : progress / steps;
				int width = QMIN(cr.width(), (int)(pg * cr.width()));
				if (steps == 0)
					width = QMIN(width,20); //Don't cross squares

				if (steps == 0) { //Busy indicator

					if (width < 1) width = 1; //A busy indicator with width 0 is kind of useless

					int remWidth = cr.width() - width; //Never disappear completely
					if (remWidth <= 0) remWidth = 1; //Do something non-crashy when too small...

					int pstep =  int(progress) % ( 2 *  remWidth );

					if ( pstep > remWidth ) {
						//Bounce about.. We're remWidth + some delta, we want to be remWidth - delta...
						// - ( (remWidth + some delta) - 2* remWidth )  = - (some deleta - remWidth) = remWidth - some delta..
						pstep = - (pstep - 2 * remWidth );
					}

					//Store the progress rect.
					QRect progressRect;
					if (reverse)
						progressRect = QRect(cr.x() + cr.width() - width - pstep, cr.y(),
										width, cr.height());
					else
						progressRect = QRect(cr.x() + pstep, cr.y(), width, cr.height());

					ThinKeramik::RowPainter(thinkeramik_progressbar).draw(p, progressRect,
									 progbarColor, cg.background() );
					return;
				}

				QRect progressRect;

				if (reverse)
					progressRect = QRect(cr.x()+(cr.width()-width), cr.y(), width, cr.height());
				else
					progressRect = QRect(cr.x(), cr.y(), width, cr.height());

				//Apply the animation rectangle.
				//////////////////////////////////////
				if (animateProgressBar)
				{
					if (reverse)
					{
						//Here, we can't simply shift, as the painter code calculates everything based
						//on the left corner, so we need to draw the 2 portions ourselves.

						//Start off by checking the geometry of the end pixmap - it introduces a bit of an offset
						QSize   endDim = loader.size(thinkeramik_progressbar + 3); //3 = 3*1 + 0 = (1,0) = cl

						//We might not have anything to animate at all, though, if the ender is the only thing to paint
						if (endDim.width() < progressRect.width())
						{
							//OK, so we do have some stripes.
							// Render the endline now - the clip region below will protect it from getting overwriten
							QPixmap endline = loader.scale(thinkeramik_progressbar + 3, endDim.width(), progressRect.height(),
															progbarColor, cg.background());
							p->drawPixmap(progressRect.x(), progressRect.y(), endline);

							//Now, calculate where the stripes should be, and set a clip region to that
							progressRect.setLeft(progressRect.x() + endline.width());
							p->setClipRect(progressRect, QPainter::CoordPainter);

							//Expand the paint region slightly to get the animation offset.
							progressRect.setLeft(progressRect.x() - progAnimShift);

							//Paint the stripes.
							QPixmap stripe = loader.scale(thinkeramik_progressbar + 4, 28, progressRect.height(),
														  progbarColor, cg.background());
														  //4 = 3*1 + 1 = (1,1) = cc

							p->drawTiledPixmap(progressRect,  stripe);
							//Exit out here to skip the regular paint path
							return;
						}
					}
					else
					{
						//Clip to the old rectangle
						p->setClipRect(progressRect, QPainter::CoordPainter);
						//Expand the paint region
						progressRect.setLeft(progressRect.x() - 28 + progAnimShift);
					}
				}

				ThinKeramik::ProgressBarPainter(thinkeramik_progressbar, reverse).draw( p,
							progressRect , progbarColor, cg.background());
			}
			break;
		}

		case CE_ProgressBarLabel: {
			const QProgressBar* pb = (const QProgressBar*)widget;
			QRect cr = subRect(SR_ProgressBarContents, widget);
			double progress = pb->progress();
			bool reverse = QApplication::reverseLayout();
			int steps = pb->totalSteps();

			if (!cr.isValid())
				return;

			QColor proglabelColor1 = cg.text();
			QColor proglabelColor2 = cg.highlightedText();
			if (!(flags & Style_Enabled)) {
			        proglabelColor1 = cg.mid();
				proglabelColor2 = cg.mid();
			} else if (!pglColor1.isNull()) {
			        proglabelColor1 = QColor(pglColor1);
				proglabelColor2 = QColor(pglColor2);
			}

			QFont font = p->font();
			font.setBold(true);
			p->setFont(font);

			// Draw label
			if (progress > 0 || steps == 0) {
				double pg = (steps == 0) ? 1.0 : progress / steps;
				int width = QMIN(cr.width(), (int)(pg * cr.width()));
				QRect crect;
				if (reverse)
					crect.setRect(cr.x()+(cr.width()-width), cr.y(), cr.width(), cr.height());
				else
					crect.setRect(cr.x()+width, cr.y(), cr.width(), cr.height());

				p->save();
				p->setPen(pb->isEnabled() ? (reverse ? proglabelColor1 : proglabelColor2) : proglabelColor1);
				p->drawText(r, AlignCenter, pb->progressString());
				p->setClipRect(crect);
				p->setPen(reverse ? proglabelColor2 : proglabelColor1);
				p->drawText(r, AlignCenter, pb->progressString());
				p->restore();

			} else {
				p->setPen(proglabelColor1);
				p->drawText(r, AlignCenter, pb->progressString());
			}

			break;
		}


		default:
			KStyle::drawControl(element, p, widget, r, cg, flags, opt);
	}
}

void ThinKeramikStyle::drawControlMask( ControlElement element,
								    QPainter *p,
								    const QWidget *widget,
								    const QRect &r,
								    const QStyleOption& opt ) const
{
	p->fillRect(r, color1);
	maskMode = true;
	drawControl( element, p, widget, r, QApplication::palette().active(), QStyle::Style_Default, opt);
	maskMode = false;
}

bool ThinKeramikStyle::isSizeConstrainedCombo(const QComboBox* combo) const
{
	if (combo->width() >= 80)
		return false;
	int suggestedWidth = combo->sizeHint().width();
	
	if (combo->width() - suggestedWidth < -5)
		return true;

	return false;
}

void ThinKeramikStyle::drawComplexControl( ComplexControl control,
                                         QPainter *p,
                                         const QWidget *widget,
                                         const QRect &r,
                                         const QColorGroup &cg,
                                         SFlags flags,
									     SCFlags controls,
									     SCFlags active,
                                         const QStyleOption& opt ) const
{
	bool disabled = ( flags & Style_Enabled ) == 0;
	switch(control)
	{
		// COMBOBOX
		// -------------------------------------------------------------------
		case CC_ComboBox:
		{
			bool             toolbarMode = false;
			const QComboBox* cb          = static_cast< const QComboBox* >( widget );
			bool             compact     = isSizeConstrainedCombo(cb);
			bool             revlayout = QApplication::reverseLayout();

			if (isFormWidget(cb))
				formMode = true;

			QPixmap * buf = 0;
			QPainter* p2 = p;

			QRect br = r;

			lightWeightMode = false;
			if ( cb->editable() && useLightCombo ) {
			        lightWeightMode = true;
			} else if (!cb->editable() && useLightNECombo ) {
			        lightWeightMode = true;
			}

			if (controls == SC_All)
			{
				//Double-buffer only when we are in the slower full-blend mode
				if (widget->parent() &&
						( widget->parent()->inherits("QToolBar")|| !qstrcmp(widget->parent()->name(), kdeToolbarWidget) ) )
				{
					buf = new QPixmap( r.width(), r.height() );
					br.setX(0);
					br.setY(0);
					p2 = new QPainter(buf);
					
					//Ensure that we have clipping on, and have a sane base.
					//If need be, Qt will shrink us to the paint region.
					p->setClipping(true);
					p->setClipRect(r);
					toolbarMode = true;
				}
			}


			if ( br.width() >= 28 && br.height() > 20 && !compact && !lightWeightMode)
				br.addCoords( 0, -2, 0, 0 );
				
			//When in compact mode, we force the shadow-less bevel mode,
			//but that also alters height and not just width.
			//readjust height to fake the other metrics (plus clear 
			//the other areas, as appropriate). The automasker
			//will take care of the overall shape.
			if ( compact && !lightWeightMode )
			{
				forceSmallMode = true;
				br.setHeight( br.height() - 2);
				p->fillRect ( r.x(), r.y() + br.height(), r.width(), 2, cg.background());
			}
				
				
			if ( controls & SC_ComboBoxFrame )
			{
				if (toolbarMode)
					toolbarBlendWidget = widget;

				if ( widget == hoverWidget )
					flags |= Style_MouseOver;

				combobox = true;
				if (!lightWeightMode) {					
				        drawPrimitive( PE_ButtonCommand, p2, br, cg, flags );
				} else {
					int  x, y, w, h;
					r.rect(&x, &y, &w, &h);

					p2->save();
				        QRect lightBr;
					QRegion lightClip;
					if (revlayout) {
					        lightBr = QRect(x, y-2, lcbbwidth+2, h+4);
					        lightClip |= QRect(x, y, lcbbwidth, h);
					} else {
					        lightBr = QRect(x+w-lcbbwidth, y-2, lcbbwidth+2, h+4);
					        lightClip |= QRect(x+w-lcbbwidth, y, lcbbwidth, h);
					}
					p2->setClipping(true);
					p2->setClipRegion( lightClip );
				        drawPrimitive( PE_ButtonCommand, p2, lightBr, cg, flags );
					p2->setClipping(false);

					QColor frameMid = getBlendColor(cg.background().dark(135), cg.dark(), 50);
					QColor frameBG  = cg.background();
					QColor frameInner = cg.base();
					if (!cb->editable()) {
					        frameInner = getBlendColor(cg.button(), cg.base(), 50);
					}

					if (!disabled) {
					        p2->setPen( frameInner );
						if (revlayout) {
							p2->drawPoint(x+lcbbwidth-1, y+1);
							p2->drawPoint(x+lcbbwidth-1, y+h-2);
						} else {
							p2->drawPoint(x+w-lcbbwidth, y+1);
							p2->drawPoint(x+w-lcbbwidth, y+h-2);
						}
					}

					if (revlayout) {
					        if (disabled) {
						        p2->fillRect( x+lcbbwidth, y, w-lcbbwidth, h, cg.background() );
						} else {
						        p2->fillRect( x+lcbbwidth, y, w-lcbbwidth, h, frameInner );
						}
					} else {
					        if (disabled) {
						        p2->fillRect( x, y, w-lcbbwidth, h, cg.background() );
						} else {
						        p2->fillRect( x, y, w-lcbbwidth, h, frameInner );
						}
					}

					p2->setPen( frameMid );
					if (revlayout) {
					        p2->drawLine(x+lcbbwidth-2, y, x+w-1, y);
						p2->drawLine(x+lcbbwidth-2, y+h-1, x+w-1, y+h-1);
						p2->drawLine(x+w-1, y, x+w-1, y+h-1);
					} else {
					        p2->drawLine(x, y, x+w-lcbbwidth+1, y);
					        p2->drawLine(x, y+h-1, x+w-lcbbwidth+1, y+h-1);
					        p2->drawLine(x, y, x, y+h-1);
					}
					p2->setPen( getBlendColor( frameMid, frameBG, 50 ) );
					if (revlayout) {
					        p2->drawPoint(x+w-1 ,y);
						p2->drawPoint(x+w-1 ,y+h-1);
					} else {
					        p2->drawPoint(x ,y);
						p2->drawPoint(x ,y+h-1);
					}

					p2->restore();
				}

				toolbarBlendWidget = 0;
			}

			// don't draw the focus rect etc. on the mask
			if ( cg.button() == color1 && cg.background() == color0 )
				break;

			if ( controls & SC_ComboBoxArrow )
			{
				if ( active )
					flags |= Style_On;
					
				QRect ar = querySubControlMetrics( CC_ComboBox, widget,
													 SC_ComboBoxArrow );
				if (!compact && !lightWeightMode)
				{
					ar.setWidth(ar.width()-13);
					QRect rr = visualRect( QRect( ar.x(), ar.y() + 4,
							loader.size(thinkeramik_ripple ).width(), ar.height() - 8 ),
							widget );

					ar = visualRect( QRect( ar.x() + loader.size( thinkeramik_ripple ).width() + 4, ar.y(), 
											11, ar.height() ), 
									widget );

					QPointArray a;
	
					a.setPoints(QCOORDARRLEN(thinkeramik_combo_arrow), thinkeramik_combo_arrow);
	
					a.translate( ar.x() + ar.width() / 2, ar.y() + ar.height() / 2 );
					p2->setPen( cg.buttonText() );
					p2->drawLineSegments( a );
	
					ThinKeramik::ScaledPainter( thinkeramik_ripple ).draw( p2, rr, cg.button(), Qt::black, disabled, ThinKeramik::TilePainter::PaintFullBlend );
				}
				else //Size-constrained combo -- loose the ripple.
				{
					ar.setWidth(ar.width() - 7);
					ar = visualRect( QRect( ar.x(), ar.y(), 11, ar.height() ), widget);
					QPointArray a;
	
					if (lightWeightMode) {
					        if (cb->editable()) {
						        a.setPoints(QCOORDARRLEN(thinkeramik_combo_arrow_light), thinkeramik_combo_arrow_light);
						} else {
						        a.setPoints(QCOORDARRLEN(thinkeramik_combo_arrow_light_ne), thinkeramik_combo_arrow_light_ne);
						}
						if (revlayout) {
						        a.translate( ar.x() + ar.width() / 2 - ( (lcbbwidth - 11) / 2), ar.y() + ar.height() / 2 );
						} else {
						        a.translate( ar.x() + ar.width() / 2 + ( (lcbbwidth - 11) / 2), ar.y() + ar.height() / 2 );
						}
					} else {
					        a.setPoints(QCOORDARRLEN(thinkeramik_combo_arrow), thinkeramik_combo_arrow);
						a.translate( ar.x() + ar.width() / 2, ar.y() + ar.height() / 2 );
					}
	
					p2->setPen( cg.buttonText() );
					p2->drawLineSegments( a );
				}
			}

			if ( controls & SC_ComboBoxEditField )
			{
				if ( cb->editable() )
				{
				        if (!lightWeightMode) {
					        QRect er = visualRect( querySubControlMetrics( CC_ComboBox, widget, SC_ComboBoxEditField ), widget );
						er.addCoords( -2, -2, 2, 2 );
						p2->fillRect( er, cg.base() );
						combobox = true;
						drawPrimitive( PE_PanelLineEdit, p2, er, cg );
						QColor frameShadowColor = cg.button().dark(160);
						ThinKeramik::RectTilePainter( thinkeramik_frame_shadow, false, false, 2, 2 ).draw( p2, er, cg.button(),
					               frameShadowColor, false, pmodeFullBlend() );
					}
				}
				else if ( cb->hasFocus() )
				{
					if (!lightWeightMode) {
					        QRect re = QStyle::visualRect(subRect(SR_ComboBoxFocusRect, cb), widget);
						if ( compact )
						        re.addCoords( 3, 3, 0, -3 );
						p2->fillRect( re, cg.brush( QColorGroup::Highlight ) );
					        drawPrimitive( PE_FocusRect, p2, re, cg,
							       Style_FocusAtBorder, QStyleOption( cg.highlight() ) );
					} else {
					        QRect re = QStyle::visualRect(subRect(SR_ComboBoxFocusRect, cb), widget);
						re.addCoords( 0, 1, 0, -1 );
					        QColor frameInner = getBlendColor(cg.button(), cg.base(), 50);
						p2->fillRect( re, frameInner );
					        drawPrimitive( PE_FocusRect, p2, re, cg,
							       Style_FocusAtBorder, QStyleOption( frameInner  ) );
					}
				}
				// QComboBox draws the text on its own and uses the painter's current colors
				if ( cb->hasFocus() && !(!cb->editable() && lightWeightMode) )
				{
					p->setPen( cg.highlightedText() );
					p->setBackgroundColor( cg.highlight() );
				}
				else
				{
					QColor frameInner = cg.base();
					if (!cb->editable()) {
					        frameInner = getBlendColor(cg.button(), cg.base(), 50 );
					}
					p->setPen( cg.text() );
					p->setBackgroundColor( lightWeightMode ? frameInner : cg.button() );
				}
			}

			if (p2 != p)
			{
				p2->end();
				delete p2;
				p->drawPixmap(r.x(), r.y(), *buf);
				delete buf;
			}

			formMode = false;
			lightWeightMode = false;
			break;
		}

		case CC_SpinWidget:
		{
			const QSpinWidget* sw = static_cast< const QSpinWidget* >( widget );
			QRect br = visualRect( querySubControlMetrics( CC_SpinWidget, widget, SC_SpinWidgetButtonField ), widget );
			if ( controls & SC_SpinWidgetButtonField )
			{
				ThinKeramik::SpinBoxPainter().draw( p, br, cg.button(), cg.background(), !sw->isEnabled() );
				if ( active & SC_SpinWidgetUp )
					ThinKeramik::CenteredPainter( thinkeramik_spinbox_pressed_arrow_up ).draw( p, br.x(), br.y() + 3, br.width(), br.height() / 2, cg.button(), cg.background() );
				else
					ThinKeramik::CenteredPainter( thinkeramik_spinbox_arrow_up ).draw( p, br.x(), br.y() + 3, br.width(), br.height() / 2, cg.button(), cg.background(), !sw->isUpEnabled() );
				if ( active & SC_SpinWidgetDown )
					ThinKeramik::CenteredPainter( thinkeramik_spinbox_pressed_arrow_down ).draw( p, br.x(), br.y() + br.height() / 2 , br.width(), br.height() / 2 - 8, cg.button(), cg.background()  );
				else
					ThinKeramik::CenteredPainter( thinkeramik_spinbox_arrow_down ).draw( p, br.x(), br.y() + br.height() / 2, br.width(), br.height() / 2 - 8, cg.background(), cg.button(), !sw->isDownEnabled() );
			}

			if ( controls & SC_SpinWidgetFrame )
				drawPrimitive( PE_PanelLineEdit, p, r, cg );

			break;
		}
		case CC_ScrollBar:
		{
			const QScrollBar* sb = static_cast< const QScrollBar* >( widget );
			if (highlightScrollBar && sb->parentWidget()) //Don't do the check if not highlighting anyway
			{
				if (sb->parentWidget()->colorGroup().button() != sb->colorGroup().button())
					customScrollMode = true;
			}
			bool horizontal = sb->orientation() == Horizontal;
			QRect slider, subpage, addpage, subline, addline;
			if ( sb->minValue() == sb->maxValue() ) flags &= ~Style_Enabled;

			slider = querySubControlMetrics( control, widget, SC_ScrollBarSlider, opt );
			subpage = querySubControlMetrics( control, widget, SC_ScrollBarSubPage, opt );
			addpage = querySubControlMetrics( control, widget, SC_ScrollBarAddPage, opt );
			subline = querySubControlMetrics( control, widget, SC_ScrollBarSubLine, opt );
			addline = querySubControlMetrics( control, widget, SC_ScrollBarAddLine, opt );

			QColor sbgcol = cg.button();
			if (!customScrollMode && !sbgcolstr.isNull())
			        sbgcol = QColor(sbgcolstr);
			
			if ( controls & SC_ScrollBarSubLine ) {
				drawPrimitive( PE_ScrollBarSubLine, p, subline, cg,
				               flags | ( ( active & SC_ScrollBarSubLine ) ? Style_Down : 0 ) );
				p->setClipping( false );
				QRegion crClip;
				QRect   crRect;
				if (horizontal) {
				        crClip |= QRect(subline.x() + subline.width() - 2, subline.y(), 2, 1);
				        crClip |= QRect(subline.x() + subline.width() - 1, subline.y() + 1, 1, 1);
					crClip |= QRect(subline.x() + subline.width() - 1, subline.y() + subline.height() - 2, 1, 1);
					crClip |= QRect(subline.x() + subline.width() - 2, subline.y() + subline.height() - 1, 2, 1);
					crRect =  QRect(subline.x() + subline.width() - 2, subline.y(), 2, subline.height());
				} else {
				        crClip |= QRect(subline.x(), subline.y() + subline.height() - 2, 1, 2);
				        crClip |= QRect(subline.x() + 1, subline.y() + subline.height() - 1, 1, 1);
					crClip |= QRect(subline.x() + subline.width() - 2, subline.y() + subline.height() - 1, 1, 1);
					crClip |= QRect(subline.x() + subline.width() - 1, subline.y() + subline.height() - 2, 1, 2);
					crRect =  QRect(subline.x(), subline.y() + subline.height() - 2, subline.width(), 2);
				}
				p->setClipRegion( crClip );
				ThinKeramik::ScrollBarPainter( ThinKeramikGroove1, 2, horizontal ).draw( p, crRect, sbgcol, cg.background(), disabled );
				p->setClipping( false );
			}

			QRegion clip;
			if ( controls & SC_ScrollBarSubPage ) clip |= subpage;
			if ( controls & SC_ScrollBarAddPage ) clip |= addpage;
			if ( horizontal )
				clip |= QRect( slider.x(), 0, slider.width(), sb->height() );
			else
				clip |= QRect( 0, slider.y(), sb->width(), slider.height() );
			clip ^= slider;

			p->setClipRegion( clip );
			ThinKeramik::ScrollBarPainter( ThinKeramikGroove1, 2, horizontal ).draw( p, slider | subpage | addpage, sbgcol, cg.background(), disabled );

			if ( controls & SC_ScrollBarSlider )
			{
				if ( horizontal )
					p->setClipRect( slider.x(), slider.y(), addpage.right() - slider.x() + 1, slider.height() );
				else
					p->setClipRect( slider.x(), slider.y(), slider.width(), addpage.bottom() - slider.y() + 1 );
				drawPrimitive( PE_ScrollBarSlider, p, slider, cg,
					flags | ( ( active == SC_ScrollBarSlider ) ? Style_Down : 0 )  );

				p->setClipping( false );
				QRegion crClip;
				QRect   crRect;
				if (horizontal) {
				        crClip |= QRect(slider.x(), slider.y(), 2, 1);
				        crClip |= QRect(slider.x(), slider.y() + 1, 1, 1);
					crClip |= QRect(slider.x(), slider.y() + slider.height() - 2, 1, 1);
					crClip |= QRect(slider.x(), slider.y() + slider.height() - 1, 2, 1);
					crRect =  QRect(slider.x(), slider.y(), 2, slider.height());
				} else {
				        crClip |= QRect(slider.x(), slider.y(), 1, 2);
				        crClip |= QRect(slider.x() + 1, slider.y(), 1, 1);
					crClip |= QRect(slider.x() + slider.width() - 2, slider.y(), 1, 1);
					crClip |= QRect(slider.x() + slider.width() - 1, slider.y(), 1, 2);
					crRect =  QRect(slider.x(), slider.y(), slider.width(), 2);
				}
				p->setClipRegion( crClip );
				ThinKeramik::ScrollBarPainter( ThinKeramikGroove1, 2, horizontal ).draw( p, crRect, sbgcol, cg.background(), disabled );
				p->setClipping( false );
				crClip = QRegion();
				crRect = QRect();
				if (horizontal) {
				        crClip |= QRect(slider.x() + slider.width() - 2, slider.y(), 2, 1);
				        crClip |= QRect(slider.x() + slider.width() - 1, slider.y() + 1, 1, 1);
					crClip |= QRect(slider.x() + slider.width() - 1, slider.y() + slider.height() - 2, 1, 1);
					crClip |= QRect(slider.x() + slider.width() - 2, slider.y() + slider.height() - 1, 2, 1);
					crRect =  QRect(slider.x() + slider.width() - 2, slider.y(), 2, slider.height());
				} else {
				        crClip |= QRect(slider.x(), slider.y() + slider.height() - 2, 1, 2);
				        crClip |= QRect(slider.x() + 1, slider.y() + slider.height() - 1, 1, 1);
					crClip |= QRect(slider.x() + slider.width() - 2, slider.y() + slider.height() - 1, 1, 1);
					crClip |= QRect(slider.x() + slider.width() - 1, slider.y() + slider.height() - 2, 1, 2);
					crRect =  QRect(slider.x(), slider.y() + slider.height() - 2, slider.width(), 2);
				}
				p->setClipRegion( crClip );
				ThinKeramik::ScrollBarPainter( ThinKeramikGroove1, 2, horizontal ).draw( p, crRect, sbgcol, cg.background(), disabled );
				p->setClipping( false );
			}

			p->setClipping( false );

			if ( (( controls & ( SC_ScrollBarSubLine | SC_ScrollBarAddLine ) ) && !removeSublineArea)
			     || (( controls & SC_ScrollBarAddLine ) && removeSublineArea) )
			{
				drawPrimitive( PE_ScrollBarAddLine, p, addline, cg, flags );
				if ( active & SC_ScrollBarSubLine && !removeSublineArea)
				{
					if ( horizontal )
						p->setClipRect( QRect( addline.x(), addline.y(), addline.width() / 2, addline.height() ) );
					else
						p->setClipRect( QRect( addline.x(), addline.y(), addline.width(), addline.height() / 2 ) );
					drawPrimitive( PE_ScrollBarAddLine, p, addline, cg, flags | Style_Down );
				}
				else if ( active & SC_ScrollBarAddLine )
				{
				        if (!removeSublineArea) {
					        if ( horizontal )
						        p->setClipRect( QRect( addline.x() + addline.width() / 2, addline.y(), addline.width() / 2, addline.height() ) );
						else
						        p->setClipRect( QRect( addline.x(), addline.y() + addline.height() / 2, addline.width(), addline.height() / 2 ) );
					}
					drawPrimitive( PE_ScrollBarAddLine, p, addline, cg, flags | Style_Down );
				}

				p->setClipping( false );
				QRegion crClip;
				QRect   crRect;
				if (horizontal) {
				        crClip |= QRect(addline.x(), addline.y(), 2, 1);
				        crClip |= QRect(addline.x(), addline.y() + 1, 1, 1);
					crClip |= QRect(addline.x(), addline.y() + addline.height() - 2, 1, 1);
					crClip |= QRect(addline.x(), addline.y() + addline.height() - 1, 2, 1);
					crRect =  QRect(addline.x(), addline.y(), 2, addline.height());
				} else {
				        crClip |= QRect(addline.x(), addline.y(), 1, 2);
				        crClip |= QRect(addline.x() + 1, addline.y(), 1, 1);
					crClip |= QRect(addline.x() + addline.width() - 2, addline.y(), 1, 1);
					crClip |= QRect(addline.x() + addline.width() - 1, addline.y(), 1, 2);
					crRect =  QRect(addline.x(), addline.y(), addline.width(), 2);
				}
				p->setClipRegion( crClip );
				ThinKeramik::ScrollBarPainter( ThinKeramikGroove1, 2, horizontal ).draw( p, crRect, sbgcol, cg.background(), disabled );
				p->setClipping( false );
			}

			if (!disableScrollbarBorder) {
			        p->save();
				p->setClipping(false);
				if (customScrollMode) {
				        p->setPen(sbgcol.dark(130));
				} else {
			                p->setPen(cg.background().dark(130));
				}
				p->drawRect(r);
				p->restore();
			}
			customScrollMode = false;


			break;
		}

		// TOOLBUTTON
		// -------------------------------------------------------------------
		case CC_ToolButton: {
			//TANGUY
			//drawPrimitive( PE_ButtonCommand, p, r, cg, flags );

			const QToolButton *toolbutton = (const QToolButton *) widget;
			bool onToolbar = widget->parentWidget() && widget->parentWidget()->inherits( "QToolBar" );
			bool onExtender = !onToolbar &&
				widget->parentWidget() && widget->parentWidget()->inherits( "QToolBarExtensionWidget") &&
				widget->parentWidget()->parentWidget()->inherits( "QToolBar" );

			bool onControlButtons = false;
			if (!onToolbar && !onExtender && widget->parentWidget() &&
				 !qstrcmp(widget->parentWidget()->name(),"qt_maxcontrols" ) )
			{
				onControlButtons = true;
				titleBarMode = Maximized;
			}

			QRect button, menuarea;
			button   = querySubControlMetrics(control, widget, SC_ToolButton, opt);
			menuarea = querySubControlMetrics(control, widget, SC_ToolButtonMenu, opt);

			SFlags bflags = flags,
				   mflags = flags;

			if (active & SC_ToolButton)
				bflags |= Style_Down;
			if (active & SC_ToolButtonMenu)
				mflags |= Style_Down;

			if ( widget == hoverWidget )
				bflags |= Style_MouseOver;


			if (onToolbar &&  static_cast<QToolBar*>(widget->parent())->orientation() == Qt::Horizontal)
				bflags |= Style_Horizontal;

			if (controls & SC_ToolButton)
			{
				// If we're pressed or on...
				if (bflags & (Style_Down | Style_On) || onControlButtons) {
				        ThinKeramik::RectTilePainter( thinkeramik_pushbutton_small_pressed, false ).draw( p, r, cg.background().dark(102), cg.background() );
					//drawButtonBorder(p, r, cg.background().dark(buttonBorderDarkValue - 15), 1); // Smallbutton Border

				// If we're raised...
				} else if (bflags & (Style_Raised) || onControlButtons) {
				        ThinKeramik::RectTilePainter( thinkeramik_pushbutton_small, false ).draw( p, r, cg.background().light(105), cg.background() );
					//drawButtonBorder(p, r, cg.background().dark(buttonBorderDarkValue - 15), 1); // Smallbutton Border
				}

				// Check whether to draw a background pixmap
				else if ( toolbutton->parentWidget() &&
						  toolbutton->parentWidget()->backgroundPixmap() &&
						  !toolbutton->parentWidget()->backgroundPixmap()->isNull() )
				{
					QPixmap pixmap = *(toolbutton->parentWidget()->backgroundPixmap());
					p->drawTiledPixmap( r, pixmap, toolbutton->pos() );
				}
				else if (onToolbar)
				{
					p->fillRect(r, cg.background());
				}
				else if (onExtender)
				{
					// This assumes floating toolbars can't have extenders,
					//(so if we're on an extender, we're not floating)
					QWidget*  parent  = static_cast<QWidget*> (widget->parent());
					QToolBar* toolbar = static_cast<QToolBar*>(parent->parent());
					QRect tr    = toolbar->rect();
					bool  horiz = toolbar->orientation() == Qt::Horizontal;

					//Calculate offset. We do this by translating our coordinates,
					//which are relative to the parent, to be relative to the toolbar.
					int xoff = 0, yoff = 0;
					if (horiz)
						yoff = parent->mapToParent(widget->pos()).y();
					else
						xoff = parent->mapToParent(widget->pos()).x();

					//ThinKeramik::GradientPainter::renderGradient( p, r, cg.button(),
					//		horiz, false, /*Not a menubar*/
					//		xoff,  yoff,
					//		tr.width(), tr.height());
					p->fillRect(QRect(r.x(), r.y(), tr.width(), tr.height()), cg.background());
				}
			}

			// Draw a toolbutton menu indicator if required
			if (controls & SC_ToolButtonMenu)
			{
				if (mflags & (Style_Down | Style_On | Style_Raised))
					drawPrimitive(PE_ButtonDropDown, p, menuarea, cg, mflags, opt);
				drawPrimitive(PE_ArrowDown, p, menuarea, cg, mflags, opt);
			}

			if (toolbutton->hasFocus() && !toolbutton->focusProxy()) {
				QRect fr = toolbutton->rect();
				fr.addCoords(3, 3, -3, -3);
				drawPrimitive(PE_FocusRect, p, fr, cg);
			}

			titleBarMode = None;

			break;
		}

		case CC_TitleBar:
			titleBarMode = Regular; //Handle buttons on titlebar different from toolbuttons
		default:
			KStyle::drawComplexControl( control, p, widget,
						r, cg, flags, controls, active, opt );

			titleBarMode = None;
	}
}

void ThinKeramikStyle::drawComplexControlMask( ComplexControl control,
                                         QPainter *p,
                                         const QWidget *widget,
                                         const QRect &r,
                                         const QStyleOption& opt ) const
{
	if (control == CC_ComboBox)
	{
		maskMode = true;
		drawComplexControl(CC_ComboBox, p, widget, r,
				QApplication::palette().active(), Style_Default,
				SC_ComboBoxFrame,SC_None, opt);
		maskMode = false;

	}
	else
		p->fillRect(r, color1);

}

int ThinKeramikStyle::pixelMetric(PixelMetric m, const QWidget *widget) const
{
	switch(m)
	{
		// BUTTONS
		// -------------------------------------------------------------------
		case PM_ButtonMargin:				// Space btw. frame and label
			return 4;

		case PM_SliderLength:
			return 12;
		case PM_SliderControlThickness:
			return loader.size( thinkeramik_slider ).height() - 4;
		case PM_SliderThickness:
			return loader.size( thinkeramik_slider ).height();

		case PM_ButtonShiftHorizontal:
			return 0;
		case PM_ButtonShiftVertical: // Offset by 1
			return 1;


		// CHECKBOXES / RADIO BUTTONS
		// -------------------------------------------------------------------
		case PM_ExclusiveIndicatorWidth:	// Radiobutton size
			return loader.size( thinkeramik_radiobutton_on ).width();
		case PM_ExclusiveIndicatorHeight:
			return loader.size( thinkeramik_radiobutton_on ).height();
		case PM_IndicatorWidth:				// Checkbox size
			return loader.size( thinkeramik_checkbox_on ).width();
		case PM_IndicatorHeight:
			return loader.size( thinkeramik_checkbox_on) .height();

		case PM_ScrollBarExtent:
			return loader.size( thinkeramik_scrollbar_vbar + ThinKeramikGroove1).width();
		case PM_ScrollBarSliderMin:
			return loader.size( thinkeramik_scrollbar_vbar + ThinKeramikSlider1 ).height() +
                        loader.size( thinkeramik_scrollbar_vbar + ThinKeramikSlider3 ).height();

		case PM_DefaultFrameWidth:
		        if (widget && widget->inherits("QWidgetStack") )
  				return 2;
			return 1;

		case PM_MenuButtonIndicator:
			return 13;

		case PM_TabBarTabVSpace:
			return 12;

		case PM_TabBarTabOverlap:
			return 0;
			
		case PM_TabBarTabShiftVertical:
		{
			const QTabBar* tb = ::qt_cast<const QTabBar*>(widget);
			if (tb)
			{
				if (tb->shape() == QTabBar::RoundedBelow || 
					tb->shape() == QTabBar::TriangularBelow)
					return 0;
			}
			
			return 2; //For top, or if not sure
		}
			

		case PM_TitleBarHeight:
			return titleBarH;

		default:
			return KStyle::pixelMetric(m, widget);
	}
}


QSize ThinKeramikStyle::sizeFromContents( ContentsType contents,
										const QWidget* widget,
										const QSize &contentSize,
										const QStyleOption& opt ) const
{
	switch (contents)
	{
		// PUSHBUTTON SIZE
		// ------------------------------------------------------------------
		case CT_PushButton:
		{
			const QPushButton* btn = static_cast< const QPushButton* >( widget );

			int w = contentSize.width() + 2 * pixelMetric( PM_ButtonMargin, widget );
			int h = contentSize.height() + 2 * pixelMetric( PM_ButtonMargin, widget );
			if ( btn->text().isEmpty() && contentSize.width() < 32 ) return QSize( w, h );


			//For some reason kcontrol no longer does this...
			//if ( btn->isDefault() || btn->autoDefault() )
			//            w = QMAX( w, 40 );

			if (reduceButtonSize) {
			        return QSize( w + 26, h + 1 ); //MX: No longer blank space -- can make a bit smaller
			} else {
			        return QSize( w + 30, h + 5 ); //MX: No longer blank space -- can make a bit smaller
			}
		}

		case CT_ToolButton:
		{
			bool onToolbar = widget->parentWidget() && widget->parentWidget()->inherits( "QToolBar" );
			if (!onToolbar) //Behaves like a button, so scale appropriately to the border
			{
				int w = contentSize.width();
				int h = contentSize.height();
				return QSize( w + 10, h + 8 );
			}
			else
			{
				return KStyle::sizeFromContents( contents, widget, contentSize, opt );
			}
		}

		case CT_ComboBox: {
			int arrow = 11 + loader.size( thinkeramik_ripple ).width();
			const QComboBox *cb = static_cast<const QComboBox*>( widget );
			if (useLightCombo && cb->editable()) {
			        return QSize( contentSize.width() + lcbbwidth + 6, contentSize.height() + 4);
			} else if (useLightNECombo && !cb->editable()) {
			        return QSize( contentSize.width() + lcbbwidth + 6, contentSize.height() + 4);
			} else {
			        return QSize( contentSize.width() + arrow + (cb->editable() ? 26 : 22),
					      contentSize.height() + 10 );
			}
		}

		// POPUPMENU ITEM SIZE
		// -----------------------------------------------------------------
		case CT_PopupMenuItem: {
			if ( ! widget || opt.isDefault() )
				return contentSize;

			const QPopupMenu *popup = (const QPopupMenu *) widget;
			bool checkable = popup->isCheckable();
			QMenuItem *mi = opt.menuItem();
			int maxpmw = opt.maxIconWidth();
			int w = contentSize.width(), h = contentSize.height();

			if ( mi->custom() ) {
				w = mi->custom()->sizeHint().width();
				h = mi->custom()->sizeHint().height();
				if ( ! mi->custom()->fullSpan() )
					h += 2*itemVMargin + 2*itemFrame;
			}
			else if ( mi->widget() ) {
			} else if ( mi->isSeparator() ) {
				w = 30; // Arbitrary
				h = 3;
			}
			else {
				if ( mi->pixmap() )
					h = QMAX( h, mi->pixmap()->height() + 2*itemFrame );
				else {
					// Ensure that the minimum height for text-only menu items
					// is the same as the icon size used by KDE.
					h = QMAX( h, 16 + 2*itemFrame );
					h = QMAX( h, popup->fontMetrics().height()
							+ 2*itemVMargin + 2*itemFrame );
				}

				if ( mi->iconSet() )
					h = QMAX( h, mi->iconSet()->pixmap(
								QIconSet::Small, QIconSet::Normal).height() +
								2 * itemFrame );
			}

			if ( ! mi->text().isNull() && mi->text().find('\t') >= 0 )
				w += itemHMargin + itemFrame*2 + 7;
			else if ( mi->popup() )
				w += 2 * arrowHMargin;

			currentStripeWidth = defaultStripeWidth;

			if ( maxpmw ) {
				w += maxpmw + 6;
				currentStripeWidth =  maxpmw + 3;
			}
			if ( checkable && maxpmw < 20 )
				w += 20 - maxpmw;
			if ( checkable || maxpmw > 0 )
				w += 12;

			w += rightBorder;

			return QSize( w, h );
		}
		
		default:
			return KStyle::sizeFromContents( contents, widget, contentSize, opt );
	}
}


QStyle::SubControl ThinKeramikStyle::querySubControl( ComplexControl control,
	                                              const QWidget* widget,
                                                  const QPoint& point,
                                                  const QStyleOption& opt ) const
{
	SubControl result = KStyle::querySubControl( control, widget, point, opt );
	if ( control == CC_ScrollBar && result == SC_ScrollBarAddLine && !removeSublineArea)
	{
		QRect addline = querySubControlMetrics( control, widget, result, opt );
		if ( static_cast< const QScrollBar* >( widget )->orientation() == Horizontal )
		{
			if ( point.x() < addline.center().x() ) result = SC_ScrollBarSubLine;
		}
		else if ( point.y() < addline.center().y() ) result = SC_ScrollBarSubLine;
	}
	return result;
}

QRect ThinKeramikStyle::querySubControlMetrics( ComplexControl control,
									const QWidget* widget,
	                              SubControl subcontrol,
	                              const QStyleOption& opt ) const
{
	switch ( control )
	{
		case CC_ComboBox:
		{
			int arrow;
			bool compact = false;
			if ( isSizeConstrainedCombo(static_cast<const QComboBox*>(widget) ) ) //### constant
				compact = true;

			bool isLightMode = false;
			if ( static_cast< const QComboBox* >( widget )->editable() ) {
			        if (useLightCombo) {
				        isLightMode = true;
				}
			} else {
			        if (useLightNECombo) {
				        isLightMode = true;
				}
			}
				
			if ( compact )
				arrow = 11;
			else
				arrow = 11 + loader.size( thinkeramik_ripple ).width();
				
			switch ( subcontrol )
			{
				case SC_ComboBoxArrow:
				        if (isLightMode)
						return QRect( widget->width() - lcbbwidth, 0, lcbbwidth, widget->height() );
					else if ( compact )
						return QRect( widget->width() - arrow - 7, 0, arrow + 6, widget->height() );
					else
						return QRect( widget->width() - arrow - 14, 0, arrow + 13, widget->height() );

				case SC_ComboBoxEditField:
				{
				        if (isLightMode)
					        return QRect( 2, 2, widget->width() - lcbbwidth - 6, widget->height() - 4 );
					else if ( compact )
						return QRect( 2, 4, widget->width() - arrow - 2 - 7, widget->height() - 8 );
					else if ( widget->width() < 36 || widget->height() < 22 )
						return QRect( 4, 3, widget->width() - arrow - 20, widget->height() - 6 );
					else if ( static_cast< const QComboBox* >( widget )->editable() )
						return QRect( 8, 4, widget->width() - arrow - 26, widget->height() - 11 );
					else
						return QRect( 6, 4, widget->width() - arrow - 22, widget->height() - 9 );
				}

				case SC_ComboBoxListBoxPopup:
				{
					//Note that the widget here == the combo, not the completion
					//box, so we don't get any recursion
					int suggestedWidth = widget->sizeHint().width(); 
					QRect def = opt.rect();
					if (isLightMode) {
					        if (QApplication::reverseLayout()) {
						        def.addCoords( lcbbwidth - 1, -4, 1, 4 );
						} else {
						        def.addCoords( 0, -4, -(lcbbwidth - 3), 4 );
						}
					} else {
					        def.addCoords( 4, -4, -6, 4 );
					}
					
					if ((def.width() - suggestedWidth < -12) && (def.width() < 80))
						def.setWidth(QMIN(80, suggestedWidth - 10));

					return def;
				}

				default: break;
			}
			break;
		}

		case CC_ScrollBar:
		{
			const QScrollBar* sb = static_cast< const QScrollBar* >( widget );
			bool horizontal = sb->orientation() == Horizontal;
			int addline, subline, sliderpos, sliderlen, maxlen, slidermin;
			if ( horizontal )
			{
				subline = loader.size( thinkeramik_scrollbar_hbar_arrow1 ).width();
				if (removeSublineArea) {
				        addline = loader.size( thinkeramik_scrollbar_hbar_arrow2s ).width();
				} else {
				        addline = loader.size( thinkeramik_scrollbar_hbar_arrow2 ).width();
				}
				maxlen = sb->width() - subline - addline;
			}
			else
			{
				subline = loader.size( thinkeramik_scrollbar_vbar_arrow1 ).height();
				if (removeSublineArea) {
				        addline = loader.size( thinkeramik_scrollbar_vbar_arrow2s ).height();
				} else {
				        addline = loader.size( thinkeramik_scrollbar_vbar_arrow2 ).height();
				}
				maxlen = sb->height() - subline - addline;
			}
			sliderpos = sb->sliderStart();
			if ( sb->minValue() != sb->maxValue() )
			{
				int range = sb->maxValue() - sb->minValue();
				sliderlen = ( sb->pageStep() * maxlen ) / ( range + sb->pageStep() );
				slidermin = pixelMetric( PM_ScrollBarSliderMin, sb );
				if ( sliderlen < slidermin ) sliderlen = slidermin;
				if ( sliderlen > maxlen ) sliderlen = maxlen;
			}
			else sliderlen = maxlen;

			switch ( subcontrol )
			{
				case SC_ScrollBarGroove:
					if ( horizontal ) return QRect( subline, 0, maxlen, sb->height() );
					else return QRect( 0, subline, sb->width(), maxlen );

				case SC_ScrollBarSlider:
					if (horizontal) return QRect( sliderpos, 0, sliderlen, sb->height() );
					else return QRect( 0, sliderpos, sb->width(), sliderlen );

				case SC_ScrollBarSubLine:
					if ( horizontal ) return QRect( 0, 0, subline, sb->height() );
					else return QRect( 0, 0, sb->width(), subline );

				case SC_ScrollBarAddLine:
					if ( horizontal ) return QRect( sb->width() - addline, 0, addline, sb->height() );
					else return QRect( 0, sb->height() - addline, sb->width(), addline );

				case SC_ScrollBarSubPage:
					if ( horizontal ) return QRect( subline, 0, sliderpos - subline, sb->height() );
					else return QRect( 0, subline, sb->width(), sliderpos - subline );

				case SC_ScrollBarAddPage:
					if ( horizontal ) return QRect( sliderpos + sliderlen, 0, sb->width() - addline  - (sliderpos + sliderlen) , sb->height() );
					else return QRect( 0, sliderpos + sliderlen, sb->width(), sb->height() - addline - (sliderpos + sliderlen)
                    /*maxlen - sliderpos - sliderlen + subline - 5*/ );

				default: break;
			};
			break;
		}
		case CC_Slider:
		{
			const QSlider* sl = static_cast< const QSlider* >( widget );
			bool horizontal = sl->orientation() == Horizontal;
			QSlider::TickSetting ticks = sl->tickmarks();
			int pos = sl->sliderStart();
			int size = pixelMetric( PM_SliderControlThickness, widget );
			int handleSize = pixelMetric( PM_SliderThickness, widget );
			int len = pixelMetric( PM_SliderLength, widget );

			//Shrink the metrics if the widget is too small
			//to fit our normal values for them.
			if (horizontal)
				handleSize = QMIN(handleSize, sl->height());
			else
				handleSize = QMIN(handleSize, sl->width());

			size = QMIN(size, handleSize);

			switch ( subcontrol )
			{
				case SC_SliderGroove:
					if ( horizontal )
					{
						if ( ticks == QSlider::Both )
							return QRect( 0, ( sl->height() - size ) / 2, sl->width(), size );
						else if ( ticks == QSlider::Above )
							return QRect( 0, sl->height() - size - ( handleSize - size ) / 2, sl->width(), size );
						return QRect( 0, ( handleSize - size ) / 2, sl->width(), size );
					}
					else
					{
						if ( ticks == QSlider::Both )
							return QRect( ( sl->width() - size ) / 2, 0, size, sl->height() );
						else if ( ticks == QSlider::Above )
							return QRect( sl->width() - size - ( handleSize - size ) / 2, 0, size, sl->height() );
						return QRect( ( handleSize - size ) / 2, 0, size, sl->height() );
					}
				case SC_SliderHandle:
					if ( horizontal )
					{
						if ( ticks == QSlider::Both )
							return QRect( pos, ( sl->height() - handleSize ) / 2, len, handleSize );
						else if ( ticks == QSlider::Above )
							return QRect( pos, sl->height() - handleSize, len, handleSize );
						return QRect( pos, 0, len, handleSize );
					}
					else
					{
						if ( ticks == QSlider::Both )
							return QRect( ( sl->width() - handleSize ) / 2, pos, handleSize, len );
						else if ( ticks == QSlider::Above )
							return QRect( sl->width() - handleSize, pos, handleSize, len );
						return QRect( 0, pos, handleSize, len );
					}
				default: break;
			}
			break;
		}
		default: break;
	}
	return KStyle::querySubControlMetrics( control, widget, subcontrol, opt );
}


//TANGUY
#ifdef QT_WS_WIN
#include <config.h>
#endif


#if defined Q_WS_X11 && ! defined K_WS_QTONLY
#ifdef HAVE_X11_EXTENSIONS_SHAPE_H
//Xlib headers are a mess -> include them down here (any way to ensure that we go second in enable-final order?)
#include  <X11/Xlib.h> // schroder
#include <X11/extensions/shape.h> // schroder
#endif
#else
#undef HAVE_X11_EXTENSIONS_SHAPE_H
#endif //QTONLY

bool ThinKeramikStyle::eventFilter( QObject* object, QEvent* event )
{
	if (KStyle::eventFilter( object, event ))
		return true;

	if ( !object->isWidgetType() ) return false;

	//Clear hover highlight when needed
	if ( (event->type() == QEvent::Leave) && (object == hoverWidget) )
		{
			QWidget* button = static_cast<QWidget*>(object);
		hoverWidget = 0;
			button->repaint( false );
		return false;
		}

	//Hover highlight on buttons, toolbuttons and combos
	if ( object->inherits("QPushButton") || object->inherits("QComboBox") || object->inherits("QToolButton") )
		{
		if (event->type() == QEvent::Enter && static_cast<QWidget*>(object)->isEnabled() )
		{
			hoverWidget = static_cast<QWidget*>(object);
			hoverWidget->repaint( false );

		}
		return false;
	}

	//Combo line edits get special frames
	if ( event->type() == QEvent::Paint && object->inherits( "QLineEdit" ) )
	{
		static bool recursion = false;
		if (recursion )
			return false;

		recursion = true;
		object->event( static_cast< QPaintEvent* >( event ) );
		QWidget* widget = static_cast< QWidget* >( object );
		QPainter p( widget );
		QColor frameShadowColor = QColorGroup::Button;
		frameShadowColor = frameShadowColor.dark(160);
		if (!useLightCombo) {
		        ThinKeramik::RectTilePainter( thinkeramik_frame_shadow, false, false, 2, 2 ).draw( &p, widget->rect(),
			        widget->palette().color( QPalette::Normal, QColorGroup::Button ),
		                frameShadowColor, false, ThinKeramik::TilePainter::PaintFullBlend);
		}
		recursion = false;
		return true;
	}
#ifdef HAVE_X11_EXTENSIONS_SHAPE_H
	//Combo dropdowns are shaped
	else if ( event->type() == QEvent::Resize && object->inherits("QListBox") )
	{
		QListBox* listbox = static_cast<QListBox*>(object);
		QResizeEvent* resize = static_cast<QResizeEvent*>(event);
		if (resize->size().height() < 6)
			return false;

		//CHECKME: Not sure the rects are perfect..
		XRectangle rects[5] = {
			{0, 0, resize->size().width()-2, resize->size().height()-6},
			{0, resize->size().height()-6, resize->size().width()-2, 1},
			{1, resize->size().height()-5, resize->size().width()-3, 1},
			{2, resize->size().height()-4, resize->size().width()-5, 1},
			{3, resize->size().height()-3, resize->size().width()-7, 1}
		};

		XShapeCombineRectangles(qt_xdisplay(), listbox->handle(), ShapeBounding, 0, 0,
			  rects, 5, ShapeSet, YXSorted);
	}
#endif
	//Combo dropdowns get fancy borders
	else if ( event->type() == QEvent::Paint && object->inherits("QListBox") )
	{
		static bool recursion = false;
		if (recursion )
			return false;
		QListBox* listbox = (QListBox*) object;
		QPaintEvent* paint = (QPaintEvent*) event;


		if ( !listbox->contentsRect().contains( paint->rect() ) )
		{
			QPainter p( listbox );
			ThinKeramik::RectTilePainter( thinkeramik_combobox_list, false, false ).draw( &p, 0, 0, listbox->width(), listbox->height(),
					listbox->palette().color( QPalette::Normal, QColorGroup::Button ),
					listbox->palette().color( QPalette::Normal, QColorGroup::Background ) );

			QPaintEvent newpaint( paint->region().intersect( listbox->contentsRect() ), paint->erased() );
			recursion = true;
			object->event( &newpaint );
			recursion = false;
			return true;
		}
	}
	//Toolbar background gradient handling
	else if (event->type() == QEvent::Paint &&
			 object->parent() && !qstrcmp(object->name(), kdeToolbarWidget) )
	{
		// Draw a gradient background for custom widgets in the toolbar
		// that have specified a "kde toolbar widget" name.
		QPainter p(static_cast<QWidget*>(object) );
		QWidget *parent = static_cast<QWidget*>(object->parent());
		QRect pr = parent->rect();
		p.fillRect(QRect(0 , 0, pr.width(), pr.height()), parent->colorGroup().background());
		return false;	// Now draw the contents
	}
	else if (event->type() == QEvent::Paint  &&  object->parent() && object->parent()->inherits("QToolBar"))
	{
		// We need to override the paint event to draw a
		// gradient on a QToolBarExtensionWidget.
		QToolBar* toolbar = static_cast<QToolBar*>(object->parent());
		QWidget* widget = static_cast<QWidget*>(object);
		QRect wr = widget->rect (), tr = toolbar->rect();
		QPainter p( widget );

		p.fillRect(QRect(0 , 0, wr.width(), wr.height()), toolbar->colorGroup().background());
		
		//Draw terminator line, too
		p.setPen( toolbar->colorGroup().mid() );
		if ( toolbar->orientation() == Qt::Horizontal )
			p.drawLine( wr.width()-1, 0, wr.width()-1, wr.height()-1 );
		else
			p.drawLine( 0, wr.height()-1, wr.width()-1, wr.height()-1 );
		return true;

	}
	return false;
}

// vim: ts=4 sw=4 noet
// kate: indent-width 4; replace-tabs off; tab-width 4; space-indent off;
