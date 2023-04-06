/*
 * iconset.cpp - various graphics handling classes
 * Copyright (C) 2001-2003  Justin Karneges, Michail Pishchagin
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "iconset.h"
#include "zip.h"

#include <qobject.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtimer.h>

#include <qpixmap.h>
#include <qimage.h>
#include <qiconset.h>
#include <qregexp.h>
#include <qmime.h>
#include <qdom.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qcstring.h>
#include <qptrlist.h>

#include "anim.h"

// sound support
//#define ICONSET_SOUND
#ifdef ICONSET_SOUND
#	include <qdatastream.h>
#	include <qapplication.h>
#	include <qca.h>
#endif

#include<qapplication.h>

//----------------------------------------------------------------------------
// Impix
//----------------------------------------------------------------------------

//! \class Impix iconset.h
//! \brief Combines a QPixmap and QImage into one class
//!
//!  Normally, it is common to use QPixmap for all application graphics.
//!  However, sometimes it is necessary to access pixel data, which means
//!  a time-costly conversion to QImage.  Impix does this conversion on
//!  construction, and keeps a copy of both a QPixmap and QImage for fast
//!  access to each.  What you gain in speed you pay in memory, as an Impix
//!  should occupy twice the space.
//!
//!  An Impix can be conveniently created from either a QImage or QPixmap
//!  source, and can be casted back to either type.
//!
//!  You may also call unload() to free the image data.
//!
//!  \code
//!  Impix i = QImage("icon.png");
//!  QLabel *iconLabel;
//!  iconLabel->setPixmap(i.pixmap());
//!  QImage image = i.image();
//!  \endcode

//static int impix_p_count = 0; // DEBUG
//static int impix_count = 0;

//! \if _hide_doc_
class Impix::Private : public QShared
{
public:
	Private()
	{
		//impix_p_count++; // DEBUG

		pixmap = 0;
		image = 0;
	}

	~Private()
	{
		unload();

		//qWarning("impix_p_count = %d", --impix_p_count); // DEBUG
	}

	Private(const Private &from)
	: QShared()
	{
		//impix_p_count++; // DEBUG

		pixmap = from.pixmap ? new QPixmap(*from.pixmap) : 0;
		image  = from.image  ? new QImage (*from.image)  : 0;
	}

	void unload()
	{
		if ( pixmap )
			delete pixmap;
		pixmap = 0;
		if ( image )
			delete image;
		image = 0;
	}

	QPixmap *pixmap;
	QImage *image;
};
//! \endif

//! \brief Construct a null Impix
//!
//! Constructs an Impix without any image data.
Impix::Impix()
{
	//impix_count++;

	d = new Private;
}

//! \brief Construct an Impix based on a QPixmap
//!
//! Constructs an Impix by making a copy of a QPixmap and creating a QImage from it.
//! \param from - source QPixmap
Impix::Impix(const QPixmap &from)
{
	//impix_count++;

	d = new Private;
	*this = from;
}

//! \brief Construct an Impix based on a QImage
//!
//! Constructs an Impix by making a copy of a QImage and creating a QPixmap from it.
//! \param from - source QImage
Impix::Impix(const QImage &from)
{
	//impix_count++;

	d = new Private;
	*this = from;
}

//!
//! \brief Copy constructor
Impix::Impix(const Impix &from)
{
	//impix_count++;

	d = from.d;
	d->ref();
}

//!
//! Sets the Impix to \a from
Impix & Impix::operator=(const Impix &from)
{
	if ( d->deref() )
		delete d;

	d = from.d;
	d->ref();

	return *this;
}

//!
//! Unloads image data, making it null.
void Impix::unload()
{
	if ( isNull() )
		return;

	detach();
	d->unload();
}

//!
//! Destroys the image.
Impix::~Impix()
{
	//qWarning("impix_count = %d", --impix_count);

	if ( d->deref() )
		delete d;
}

bool Impix::isNull() const
{
	return d->image || d->pixmap ? false: true;
}

const QPixmap & Impix::pixmap() const
{
	if(!d->pixmap)
		d->pixmap = new QPixmap;
	return *d->pixmap;
}

const QImage & Impix::image() const
{
	if(!d->image)
		d->image = new QImage;
	return *d->image;
}

void Impix::setPixmap(const QPixmap &x)
{
	detach();

	d->unload();
	d->pixmap = new QPixmap(x);
	d->image = new QImage(x.convertToImage());
}

void Impix::setImage(const QImage &x)
{
	detach();

	d->unload();
	d->pixmap = new QPixmap;
	d->pixmap->convertFromImage(x);
	d->image = new QImage(x);
}

bool Impix::loadFromData(const QByteArray &ba)
{
	qApp->lock();

	bool ret = false;
	QImage img;
	if ( img.loadFromData( ba ) ) {
		setImage ( img );
		ret = true;
	}

	qApp->unlock();

	return ret;
}

Impix Impix::copy() const
{
	Impix impix;
	delete impix.d;
	impix.d = new Private( *this->d );

	return impix;
}

void Impix::detach()
{
	if ( d->count > 1 )
		*this = copy();
}

//----------------------------------------------------------------------------
// IconSharedObject
//----------------------------------------------------------------------------

class IconSharedObject : public QObject
{
	Q_OBJECT
public:
	IconSharedObject()
#ifdef ICONSET_SOUND
	: QObject(qApp, "IconSharedObject")
#endif
	{ }

	QString unpackPath;

signals:
	void playSound(QString);

private:
	friend class Icon;
};

static IconSharedObject *iconSharedObject = 0;

//----------------------------------------------------------------------------
// Icon
//----------------------------------------------------------------------------

/*!
	\class Icon
	\brief Can contain Anim and stuff

	This class can be used for storing application icons as well as emoticons
	(it has special functions in order to do that).

	Icons can contain animation, associated sound files, its own names.

	For implementing emoticon functionality, Icon can have associated text
	values and QRegExp for easy searching.
*/

//! \if _hide_doc_
class Icon::Private : public QObject, public QShared
{
	Q_OBJECT
public:
	Private()
	{
		anim = 0;
		iconSet = 0;
		activatedCount = 0;
		text.setAutoDelete(true);
	}

	~Private()
	{
		unloadAnim();
		if ( iconSet )
			delete iconSet;
	}

	Private(const Private &from)
	: QObject(), QShared()
	{
		name = from.name;
		regExp = from.regExp;
		text = from.text;
		sound = from.sound;
		impix = from.impix;
		anim = from.anim ? new Anim ( *from.anim ) : 0;
		iconSet = 0;
	}

	void unloadAnim()
	{
		if ( anim )
			delete anim;
		anim = 0;
	}

signals:
	void pixmapChanged(const QPixmap &);
	void iconModified(const QPixmap &);

public:
	const QPixmap &pixmap() const
	{
		if ( anim )
			return anim->framePixmap();
		return impix.pixmap();
	}

public slots:
	void animUpdate() { emit pixmapChanged( pixmap() ); }

public:
	QString name;
	QRegExp regExp;
	QDict<QString> text;
	QString sound;

	Impix impix;
	Anim *anim;
	QIconSet *iconSet;

	int activatedCount;
	friend class Icon;
};
//! \endif

//static int icon_count = 0;

//!
//! Constructs empty Icon.
Icon::Icon()
: QObject(0, 0)
{
	//icon_count++;

	d = new Private;

	connect(d, SIGNAL(pixmapChanged(const QPixmap &)), SIGNAL(pixmapChanged(const QPixmap &)));
	connect(d, SIGNAL(iconModified(const QPixmap &)), SIGNAL(iconModified(const QPixmap &)));
}

//!
//! Destroys Icon.
Icon::~Icon()
{
	//qWarning("icon_count = %d", --icon_count);

	if ( d->deref() )
		delete d;
}

//!
//! Creates new icon, that is a copy of \a from. Note, that if one icon will be changed,
//! other will be changed as well. (that's because image data is shared)
Icon::Icon(const Icon &from)
: QObject(0, 0)
{
	//icon_count++;

	d = from.d;
	d->ref();

	connect(d, SIGNAL(pixmapChanged(const QPixmap &)), SIGNAL(pixmapChanged(const QPixmap &)));
	connect(d, SIGNAL(iconModified(const QPixmap &)), SIGNAL(iconModified(const QPixmap &)));
}

//!
//! Creates new icon, that is a copy of \a from. Note, that if one icon will be changed,
//! other will be changed as well. (that's because image data is shared)
Icon & Icon::operator= (const Icon &from)
{
	if ( d->deref() )
		delete d;

	d = from.d;
	d->ref();

	connect(d, SIGNAL(pixmapChanged(const QPixmap &)), SIGNAL(pixmapChanged(const QPixmap &)));
	connect(d, SIGNAL(iconModified(const QPixmap &)), SIGNAL(iconModified(const QPixmap &)));

	return *this;
}

Icon Icon::copy() const
{
	Icon icon;
	delete icon.d;
	icon.d = new Private( *this->d );

	connect(icon.d, SIGNAL(pixmapChanged(const QPixmap &)), &icon, SIGNAL(pixmapChanged(const QPixmap &)));
	connect(icon.d, SIGNAL(iconModified(const QPixmap &)),  &icon, SIGNAL(iconModified(const QPixmap &)));

	return icon;
}

void Icon::detach()
{
	if ( d->count != 1 ) // only if >1 reference
		*this = copy();
}

//!
//! Returns \c true when icon contains animation.
bool Icon::isAnimated() const
{
	return d->anim != 0;
}

//!
//! Returns QPixmap of current frame.
const QPixmap &Icon::pixmap() const
{
	return d->pixmap();
}

//!
//! Returns QImage of current frame.
const QImage &Icon::image() const
{
	if ( d->anim )
		return d->anim->frameImage();
	return d->impix.image();
}

//!
//! Returns Impix of first animation frame.
//! \sa setImpix()
const Impix &Icon::impix() const
{
	return d->impix;
}

//!
//! Returns Impix of current animation frame.
//! \sa impix()
const Impix &Icon::frameImpix() const
{
	if ( d->anim )
		return d->anim->frameImpix();
	return d->impix;
}

//!
//! Returns QIconSet of first animation frame.
//! TODO: Add automatic greyscale icon generation.
const QIconSet &Icon::iconSet() const
{
	if ( d->iconSet )
		return *d->iconSet;

	d->iconSet = new QIconSet( d->impix.pixmap() );
	return *d->iconSet;
}

//!
//! Sets the Icon impix to \a impix.
//! \sa impix()
void Icon::setImpix(const Impix &impix, bool doDetach)
{
	if ( doDetach )
		detach();

	d->impix = impix;
	emit d->pixmapChanged( pixmap() );
	emit d->iconModified( d->impix.pixmap() );
}

//!
//! Returns pointer to Anim object, or \a 0 if Icon doesn't contain an animation.
const Anim *Icon::anim() const
{
	return d->anim;
}

//!
//! Sets the animation for icon to \a anim. Also sets Impix to be the first frame of animation.
//! If animation have less than two frames, it is deleted.
//! \sa anim()
void Icon::setAnim(const Anim &anim, bool doDetach)
{
	if ( doDetach )
		detach();

	d->unloadAnim();
	d->anim = new Anim(anim);

	if ( d->anim->numFrames() > 0 )
		setImpix( d->anim->frame(0) );

	if ( d->anim->numFrames() < 2 ) {
		delete d->anim;
		d->anim = 0;
	}

	if ( d->anim && d->activatedCount > 0 ) {
		d->activatedCount = 0;
		activated(false); // restart the animation, but don't play the sound
	}

	emit d->pixmapChanged( pixmap() );
	emit d->iconModified( d->impix.pixmap() );
}

//!
//! Removes animation from icon.
//! \sa setAnim()
void Icon::removeAnim(bool doDetach)
{
	if ( doDetach )
		detach();

	if ( !d->anim )
		return;

	d->activatedCount = 0;
	stop();

	delete d->anim;
	d->anim = 0;

	emit d->pixmapChanged( pixmap() );
	//emit d->iconModified( d->impix.pixmap() );
}

//!
//! Returns the number of current animation frame.
//! \sa setAnim()
int Icon::framenumber() const
{
	if ( d->anim )
		return d->anim->framenumber();

	return 0;
}

//!
//! Returns name of the Icon.
//! \sa setName()
const QString &Icon::name() const
{
	return d->name;
}

//!
//! Sets the Icon name to \a name
//! \sa name()
void Icon::setName(const QString &name)
{
	detach();

	d->name = name;
}

void Icon::setName(const char *s)
{
	QObject::setName(s);
}

//!
//! Returns Icon's QRegExp. It is used to store information for emoticons.
//! \sa setRegExp()
const QRegExp &Icon::regExp() const
{
	return d->regExp;
}

//!
//! Sets the Icon QRegExp to \a regExp.
//! \sa regExp()
//! \sa text()
void Icon::setRegExp(const QRegExp &regExp)
{
	detach();

	d->regExp = regExp;
}

//!
//! Returns Icon's text. It is used to store information for emoticons.
//! \sa setText()
//! \sa regExp()
const QDict<QString> &Icon::text() const
{
	return d->text;
}

//!
//! Sets the Icon text to \a t.
//! \sa text()
void Icon::setText(const QDict<QString> &t)
{
	detach();

	d->text = t;
}

//!
//! Returns file name of associated sound.
//! \sa setSound()
//! \sa activated()
const QString &Icon::sound() const
{
	return d->sound;
}

//!
//! Sets the sound file name to be associated with this Icon.
//! \sa sound()
//! \sa activated()
void Icon::setSound(const QString &sound)
{
	detach();

	d->sound = sound;
}

//!
//! Initializes Icon's Impix (or Anim, if \a isAnim equals \c true).
//! Iconset::load uses this function.
bool Icon::loadFromData(const QByteArray &ba, bool isAnim)
{
	detach();

	bool ret = false;
	if ( isAnim ) {
		Anim *anim = new Anim(ba);
		setAnim(*anim);
		ret = anim->numFrames();
		delete anim; // shared data rules ;)
	}

	if ( !ret && d->impix.loadFromData(ba) )
		ret = true;

	if ( ret ) {
		emit d->pixmapChanged( pixmap() );
		emit d->iconModified( d->impix.pixmap() );
	}

	return ret;
}

//!
//! You need to call this function, when Icon is \e triggered, i.e. it is shown on screen
//! and it must start animation (if it has not animation, this function will do nothing).
//! When icon is no longer shown on screen you MUST call stop().
//! NOTE: For EACH activated() function call there must be associated stop() call, or the
//! animation will go crazy. You've been warned.
//! If \a playSound equals \c true, Icon will play associated sound file.
//! \sa stop()
void Icon::activated(bool playSound)
{
	d->activatedCount++;
	//qWarning("%-25s Icon::activated(): count = %d", name().latin1(), d->activatedCount);

#ifdef ICONSET_SOUND
	if ( playSound && !d->sound.isNull() ) {
		if ( !iconSharedObject )
			iconSharedObject = new IconSharedObject();

		emit iconSharedObject->playSound(d->sound);
	}
#endif

	if ( d->anim ) {
		d->anim->unpause();

		d->anim->disconnectUpdate (d, SLOT(animUpdate())); // ensure, that we're connected to signal exactly one time
		d->anim->connectUpdate (d, SLOT(animUpdate()));
	}
}

//!
//! You need to call this function when Icon is no more shown on screen. It would save
//! processor time, if Icon has animation.
//! NOTE: For EACH activated() function call there must be associated stop() call, or the
//! animation will go crazy. You've been warned.
//! \sa activated()
void Icon::stop()
{
	d->activatedCount--;
	//qWarning("%-25s Icon::stop(): count = %d", name().latin1(), d->activatedCount);

	if ( d->activatedCount <= 0 ) {
		d->activatedCount = 0;
		if ( d->anim ) {
			d->anim->pause();
			d->anim->restart();
		}
	}
}

//!
//! As the name says, this function removes the first animation frame. This is used to
//! create system Psi iconsets.
void Icon::stripFirstAnimFrame()
{
	if ( d && d->anim )
		d->anim->stripFirstFrame();
}

//----------------------------------------------------------------------------
// IconsetFactory
//----------------------------------------------------------------------------

/*!
	\class IconsetFactory
	\brief Class for easy application-wide Icon searching

	You can add several Iconsets to IconsetFactory to use multiple Icons
	application-wide with ease.

	You can reference Icons by their name from any place from your application.
*/

//! \if _hide_doc_
class IconsetFactoryPrivate
{
private:
	static QPtrList<Iconset> *iconsets;
	static QPixmap *emptyPixmap;

public:
	static void registerIconset(const Iconset *);
	static void unregisterIconset(const Iconset *);

public:
	static const Icon *icon(const QString &name);

	friend class IconsetFactory;
};
//! \endif

QPtrList<Iconset> *IconsetFactoryPrivate::iconsets = 0;
QPixmap *IconsetFactoryPrivate::emptyPixmap = 0;

void IconsetFactoryPrivate::registerIconset(const Iconset *i)
{
	if ( !iconsets )
		iconsets = new QPtrList<Iconset>;

	if ( iconsets->find(i) < 0 ) {
		//qWarning("IconsetFactoryPrivate::registerIconset(): %s", i->fileName().latin1());
		iconsets->append ( i );
	}
}

void IconsetFactoryPrivate::unregisterIconset(const Iconset *i)
{
	if ( iconsets && iconsets->find(i) >= 0 ) {
		//qWarning("IconsetFactoryPrivate::unregisterIconset(): %s", i->fileName().latin1());
		iconsets->remove ( i );
	}

	if ( !iconsets || !iconsets->count() ) {
		if ( iconsets ) {
			delete iconsets;
			iconsets = 0;
		}

		if ( emptyPixmap ) {
			delete emptyPixmap;
			emptyPixmap = 0;
		}
	}
}

const Icon *IconsetFactoryPrivate::icon(const QString &name)
{
	if ( !iconsets )
		return 0;

	const Icon *i = 0;
	QPtrListIterator<Iconset> it( *iconsets );
	for (; it.current(); ++it) {
		Iconset *iconset = *it;
		if ( iconset )
			i = iconset->icon(name);

		if ( i )
			break;
	}
	return i;
}

//!
//! Returns pointer to Icon with name \a name, or \a 0 if Icon with that name wasn't
//! found in IconsetFactory.
const Icon *IconsetFactory::iconPtr(const QString &name)
{
	const Icon *i = IconsetFactoryPrivate::icon(name);
	if ( !i ) {
		qDebug("WARNING: IconsetFactory::icon(\"%s\"): icon not found", name.latin1());
	}
	return i;
}

//!
//! Returns Icon with name \a name, or empty Icon if Icon with that name wasn't
//! found in IconsetFactory.
Icon IconsetFactory::icon(const QString &name)
{
	const Icon *i = iconPtr(name);
	if ( i )
		return *i;
	return Icon();
}

//!
//! Returns QPixmap of first animation frame of the specified Icon, or the empty
//! QPixmap, if that Icon wasn't found in IconsetFactory.
//! This function is faster than the call to IconsetFactory::icon() and cast to QPixmap,
//! because the intermediate Icon object is not created and destroyed.
const QPixmap &IconsetFactory::iconPixmap(const QString &name)
{
	const Icon *i = iconPtr(name);
	if ( i )
		return i->impix().pixmap();

	if ( !IconsetFactoryPrivate::emptyPixmap )
		IconsetFactoryPrivate::emptyPixmap = new QPixmap();
	return *IconsetFactoryPrivate::emptyPixmap;
}

//!
//! Returns list of all Icon names that are in IconsetFactory.
const QStringList IconsetFactory::icons()
{
	QStringList list;

	QPtrList<Iconset> *iconsets = IconsetFactoryPrivate::iconsets;
	uint count = 0;
	if ( iconsets )
		count = iconsets->count();
	for (uint i = 0; i < count; i++) {
		QPtrListIterator<Icon> it = iconsets->at(i)->iterator();
		for ( ; it.current(); ++it)
			list << (*it)->name();
	}

	return list;

}

//----------------------------------------------------------------------------
// Iconset
//----------------------------------------------------------------------------

/*!
	\class Iconset
	\brief Class for easy Icon grouping

	This class supports loading Icons from .zip arhives. It also provides additional
	information: name(), authors(), version(), description() and creation() date.

	This class is very handy to manage emoticon sets as well as usual application icons.

	\sa IconsetFactory
*/

//! \if _hide_doc_
class Iconset::Private : public QShared
{
private:
	void init()
	{
		name = "Unnamed";
		//version = "1.0";
		//description = "No description";
		//authors << "I. M. Anonymous";
		//creation = "1900-01-01";
		homeUrl = QString::null;

		dict.setAutoDelete(true);
	}

public:
	QString name, version, description, creation, homeUrl, filename;
	QStringList authors;
	QDict<Icon> dict;
	QPtrList<Icon> list;
	QDict<QString> info;
	//Icon nullIcon;

public:
	Private()
	{
		init();
	}

	Private(const Private &from)
	: QShared()
	{
		init();

		setInformation(from);

		QDictIterator<Icon> it( from.dict );
		for ( ; it.current(); ++it)
			dict.insert(it.currentKey(), new Icon(*it.current()));

		QPtrListIterator<Icon> it2( from.list );
		for ( ; it2.current(); ++it2)
			list.append(new Icon(*it2.current()));
	}

	void append(QString n, Icon *icon)
	{
		// all Icon names in Iconset must be unique
		if ( !dict.find(n) ) {
			dict.replace (n, icon);
			list.append (icon);
		}
	}

	QByteArray loadData(const QString &fileName, const QString &dir)
	{
		QByteArray ba;

		QFileInfo fi(dir);
		if ( fi.isDir() ) {
			QFile file ( dir + "/" + fileName );
			file.open (IO_ReadOnly);

			ba = file.readAll();
		}
		else if ( fi.extension(false) == "jisp" || fi.extension(false) == "zip" ) {
			UnZip z(dir);
			if ( !z.open() )
				return ba;

			QString n = fi.baseName(true) + "/" + fileName;
			if ( !z.readFile(n, &ba) ) {
				n = "/" + fileName;
				z.readFile(n, &ba);
			}
		}

		return ba;
	}

	void loadMeta(const QDomElement &i, const QString &dir)
	{
		Q_UNUSED(dir);

		for (QDomNode node = i.firstChild(); !node.isNull(); node = node.nextSibling()) {
			QDomElement e = node.toElement();
			if( e.isNull() )
				continue;

			QString tag = e.tagName();
			if ( tag == "name" ) {
				name = e.text();
			}
			else if ( tag == "version" ) {
				version = e.text();
			}
			else if ( tag == "description" ) {
				description = e.text();
			}
			else if ( tag == "author" ) {
				QString n = e.text();
				QString tmp = "<br>&nbsp;&nbsp;";
				if ( !e.attribute("email").isEmpty() ) {
					QString s = e.attribute("email");
					n += tmp + QString("Email: <a href='mailto:%1'>%2</a>").arg( s ).arg( s );
				}
				if ( !e.attribute("jid").isEmpty() ) {
					QString s = e.attribute("jid");
					n += tmp + QString("JID: <a href='jabber:%1'>%2</a>").arg( s ).arg( s );
				}
				if ( !e.attribute("www").isEmpty() ) {
					QString s = e.attribute("www");
					n += tmp + QString("WWW: <a href='%1'>%2</a>").arg( s ).arg( s );
				}
				authors += n;
			}
			else if ( tag == "creation" ) {
				creation = e.text();
			}
			else if ( tag == "home" ) {
				homeUrl = e.text();
			}
		}
	}

	static int icon_counter; // used to give unique names to icons

	// will return 'true' when icon is loaded ok
	bool loadIcon(const QDomElement &i, const QString &dir)
	{
		Icon icon;

		QDict<QString> text, graphic, sound, object;
		graphic.setAutoDelete(true);
		sound.setAutoDelete(true);
		object.setAutoDelete(true);

		QString name;
		name.sprintf("icon_%04d", icon_counter++);
		bool isAnimated = false;
		bool isImage = false;

		for(QDomNode n = i.firstChild(); !n.isNull(); n = n.nextSibling()) {
			QDomElement e = n.toElement();
			if ( e.isNull() )
				continue;

			QString tag = e.tagName();
			if ( tag == "text" ) {
				QString lang = e.attribute("xml:lang");
				if ( lang.isEmpty() )
					lang = ""; // otherwise there would be many warnings :-(
				text.insert( lang, new QString(e.text()));
			}
			else if ( tag == "object" ) {
				object.insert( e.attribute("mime"), new QString(e.text()));
			}
			else if ( tag == "x" ) {
				QString attr = e.attribute("xmlns");
				if ( attr == "name" ) {
					name = e.text();
				}
				else if ( attr == "type" ) {
					if ( e.text() == "animation" )
						isAnimated = true;
					else if ( e.text() == "image" )
						isImage = true;
				}
			}
			// leaved for compatibility with old JEP
			else if ( tag == "graphic" ) {
				graphic.insert( e.attribute("mime"), new QString(e.text()));
			}
			else if ( tag == "sound" ) {
				sound.insert( e.attribute("mime"), new QString(e.text()));
			}
		}

		icon.setText (text);
		icon.setName (name);

		QStringList graphicMime, soundMime, animationMime;
		graphicMime << "image/png"; // first item have higher priority than latter
		graphicMime << "video/x-mng";
		graphicMime << "image/gif";
		graphicMime << "image/x-xpm";
		graphicMime << "image/bmp";
		graphicMime << "image/jpeg";
		// TODO: untested
		graphicMime << "image/svg+xml";

		soundMime << "audio/x-wav";
		soundMime << "audio/x-ogg";
		soundMime << "audio/x-mp3";
		soundMime << "audio/x-midi";

		// MIME-types, that support animations
		animationMime << "image/gif";
		//animationMime << "image/png";
		animationMime << "image/x-mng";

		if ( !object.isEmpty() ) {
			// fill the graphic & sound tables, if there are some
			// 'object' entries. inspect the supported mimetypes
			// and copy mime info and file path to 'graphic' and
			// 'sound' dictonaries.

			QStringList::Iterator it = graphicMime.begin();
			for ( ; it != graphicMime.end(); ++it)
				if ( object[*it] && !object[*it]->isNull() )
					graphic.insert( *it, new QString(*object[*it]));

			it = soundMime.begin();
			for ( ; it != soundMime.end(); ++it)
				if ( object[*it] && !object[*it]->isNull() )
					sound.insert( *it, new QString(*object[*it]));
		}

		bool loadSuccess = false;

		{
			QStringList::Iterator it = graphicMime.begin();
			for ( ; it != graphicMime.end(); ++it) {
				if ( graphic[*it] && !graphic[*it]->isNull() ) {
					// if format supports animations, then load graphic as animation, and
					// if there is only one frame, then later it would be converted to single Impix
					if ( !isAnimated && !isImage ) {
						QStringList::Iterator it2 = animationMime.begin();
						for ( ; it2 != animationMime.end(); ++it2)
							if ( *it == *it2 ) {
								isAnimated = true;
								break;
							}
					}

					if ( icon.loadFromData( loadData(*graphic[*it], dir), isAnimated ) ) {
						loadSuccess = true;
						break;
					}
					else {
						qDebug("Iconset::load(): Couldn't load %s graphic for the %s icon for the %s iconset", (*it).latin1(), name.latin1(), this->name.latin1());
						loadSuccess = false;
					}
				}
			}
		}

		{
			QFileInfo fi(dir);
			QStringList::Iterator it = soundMime.begin();
			for ( ; it != soundMime.end(); ++it) {
				if ( sound[*it] && !sound[*it]->isNull() ) {
					if ( !fi.isDir() ) { // it is a .zip file then
#ifdef ICONSET_SOUND
						if ( !iconSharedObject )
							iconSharedObject = new IconSharedObject();

						QString path = iconSharedObject->unpackPath;
						if ( path.isEmpty() )
							break;

						QFileInfo ext(*sound[*it]);
						path += "/" + QCA::SHA1::hashToString(QString(fi.absFilePath() + "/" + *sound[*it]).utf8()) + "." + ext.extension();

						QFile file ( path );
						file.open ( IO_WriteOnly );
						QDataStream out ( &file );

						QByteArray data = loadData(*sound[*it], dir);
						out.writeRawBytes (data, data.size());

						icon.setSound ( path );
						break;
#endif
					}
					else {
						icon.setSound ( fi.absFilePath() + "/" + *sound[*it] );
						break;
					}
				}
			}
		}

		if ( text.count() )
		{	// construct RegExp
			QString regexp;
			QDictIterator<QString> it( text );
			for ( ; it.current(); ++it ) {
				if ( !regexp.isEmpty() )
					regexp += '|';

				regexp += QRegExp::escape(**it);
			}

			// make sure there is some form of whitespace on at least one side of the text string
			//regexp = QString("(\\b(%1))|((%2)\\b)").arg(regexp).arg(regexp);
			icon.setRegExp ( regexp );
		}

		if ( loadSuccess )
			append( name, new Icon(icon) );

		return loadSuccess;
	}

	// would return 'true' on success
	bool load(const QDomDocument &doc, const QString dir)
	{
		QDomElement base = doc.documentElement();
		if ( base.tagName() != "icondef" )
			return false;

		bool success = true;

		for(QDomNode node = base.firstChild(); !node.isNull(); node = node.nextSibling()) {
			QDomElement i = node.toElement();
			if( i.isNull() )
				continue;

			QString tag = i.tagName();
			if ( tag == "meta" ) {
				loadMeta (i, dir);
			}
			else if ( tag == "icon" ) {
				bool ret = loadIcon (i, dir);
				if ( !ret )
					success = false;
			}
			else if ( tag == "x" ) {
				info.insert( i.attribute("xmlns"), new QString(i.text()) );
			}
		}

		return success;
	}
	
	void setInformation(const Private &from) {
		name = from.name;
		version = from.version;
		description = from.description;
		creation = from.creation;
		homeUrl = from.homeUrl;
		filename = from.filename;
		authors = from.authors;
		info = from.info;
	}
};
//! \endif

int Iconset::Private::icon_counter = 0;

//static int iconset_counter = 0;

//!
//! Creates empty Iconset.
Iconset::Iconset()
{
	//iconset_counter++;

	d = new Private;
}

//!
//! Creates shared copy of Iconset \a from.
Iconset::Iconset(const Iconset &from)
{
	//iconset_counter++;

	d = from.d;
	d->ref();
}

//!
//! Destroys Iconset, and frees all allocated Icons.
Iconset::~Iconset()
{
	//qWarning("iconset_counter = %d", --iconset_counter);

	if ( d->deref() )
		delete d;

	IconsetFactoryPrivate::unregisterIconset(this);
}

//!
//! Copies all Icons as well as additional information from Iconset \a from.
Iconset &Iconset::operator=(const Iconset &from)
{
	if ( d->deref() )
		delete d;

	d = from.d;
	d->ref();

	return *this;
}

Iconset Iconset::copy() const
{
	Iconset is;
	delete is.d;
	is.d = new Private( *this->d );

	return is;
}

void Iconset::detach()
{
	if ( d->count > 1 )
		*this = copy();
}

//!
//! Appends icons from Iconset \a from to this Iconset.
Iconset &Iconset::operator+=(const Iconset &i)
{
	detach();

	QDictIterator<Icon> it( i.d->dict );
	for ( ; it.current(); ++it)
		d->dict.insert(it.currentKey(), new Icon(*it.current()));

	QPtrListIterator<Icon> it2( i.d->list );
	for ( ; it2.current(); ++it2)
		d->list.append(new Icon(*it2.current()));

	return *this;
}

//!
//! Frees all allocated Icons.
void Iconset::clear()
{
	detach();

	d->list.clear();
	d->dict.clear();
}

//!
//! Returns the number of Icons in Iconset.
uint Iconset::count() const
{
	return d->list.count();
}

//!
//! Loads Icons and additional information from directory \a dir. Directory can usual directory,
//! or a .zip/.jisp archive. There must exist file named \c icondef.xml in that directory.
bool Iconset::load(const QString &dir)
{
	detach();

	// make it run okay on windows 9.x (where the pixmap memory is limited)
	QPixmap::Optimization optimization = QPixmap::defaultOptimization();
	QPixmap::setDefaultOptimization( QPixmap::MemoryOptim );

	bool ret = false;

	QByteArray ba;
	ba = d->loadData ("icondef.xml", dir);
	if ( !ba.isEmpty() ) {
		QDomDocument doc;
		if ( doc.setContent(ba, false) ) {
			if ( d->load(doc, dir) ) {
				d->filename = dir;
				ret = true;
			}
		}
		else
			qWarning("Iconset::load(): Failed to load iconset: icondef.xml is invalid XML");
	}

	QPixmap::setDefaultOptimization( optimization );

	return ret;
}

//!
//! Returns pointer to Icon, if Icon with name \a name was found in Iconset, or \a 0 otherwise.
//! \sa setIcon()
const Icon *Iconset::icon(const QString &name) const
{
	if ( !d || d->dict.isEmpty() )
		return 0;

	return d->dict.find(name);
}

//!
//! Appends (or replaces, if Icon with that name already exists) Icon to Iconset.
void Iconset::setIcon(const QString &name, const Icon &icon)
{
	detach();

	d->dict.replace (name, new Icon(icon));

	QPtrListIterator<Icon> it( d->list );
	for ( ; it.current(); ++it)
		if ( it.current()->name() == icon.name() ) {
			Icon *icon = it.current();
			d->list.remove(icon);
			break;
		}

	d->list.append (new Icon(icon));
}

//!
//! Removes Icon with the name \a name from Iconset.
void Iconset::removeIcon(const QString &name)
{
	detach();

	d->dict.remove (name);

	QPtrListIterator<Icon> it( d->list );
	for ( ; it.current(); ++it)
		if ( it.current()->name() == name ) {
			Icon *icon = it.current();
			d->list.remove(icon);
			break;
		}
}

//!
//! Returns the Iconset name.
const QString &Iconset::name() const
{
	return d->name;
}

//!
//! Returns the Iconset version.
const QString &Iconset::version() const
{
	return d->version;
}

//!
//! Returns the Iconset description string.
const QString &Iconset::description() const
{
	return d->description;
}

//!
//! Returns the Iconset authors list.
const QStringList &Iconset::authors() const
{
	return d->authors;
}

//!
//! Returns the Iconset creation date.
const QString &Iconset::creation() const
{
	return d->creation;
}

//!
//! Returns the Iconsets' home URL.
const QString &Iconset::homeUrl() const
{
	return d->homeUrl;
}

QPtrListIterator<Icon> Iconset::iterator() const
{
	//detach(); // ???

	QPtrListIterator<Icon> it( d->list );
	return it;
}

//!
//! Returns directory (or .zip/.jisp archive) name from which Iconset was loaded.
const QString &Iconset::fileName() const
{
	return d->filename;
}

//!
//! Sets the Iconset directory (.zip archive) name.
void Iconset::setFileName(const QString &f)
{
	d->filename = f;
}

/**
 * Sets the information (meta-data) of this iconset to the information from the given iconset.
 */
void Iconset::setInformation(const Iconset &from) {
	detach();
	d->setInformation( *(from.d) );
}

//!
//! Returns additional Iconset information.
//! \sa setInfo()
const QDict<QString> Iconset::info() const
{
	return d->info;
}

//!
//! Sets additional Iconset information.
//! \sa info()
void Iconset::setInfo(const QDict<QString> &i)
{
	d->info = i;
}

//!
//! Created QMimeSourceFactory with names of icons and their images.
QMimeSourceFactory *Iconset::createMimeSourceFactory() const
{
	QMimeSourceFactory *m = new QMimeSourceFactory;

	QDictIterator<Icon> it( d->dict );
	for ( ; it.current(); ++it)
		m->setImage(it.currentKey(), it.current()->image());

	return m;
}

//!
//! Adds Iconset to IconsetFactory.
void Iconset::addToFactory() const
{
	IconsetFactoryPrivate::registerIconset(this);
}

//!
//! Removes Iconset from IconsetFactory.
void Iconset::removeFromFactory() const
{
	IconsetFactoryPrivate::unregisterIconset(this);
}

//!
//! Use this function before creation of Iconsets and it will enable the
//! sound playing ability when Icons are activated().
//!
//! \a unpackPath is the path, to where the sound files will be unpacked, if
//! required (i.e. when sound is stored inside packed iconset.zip file). Unpacked
//! file name will be unique, and you can empty the unpack directory at the application
//! exit. Calling application MUST ensure that the \a unpackPath is already created.
//! If specified \a unpackPath is empty ("" or QString::null) then sounds from archives
//! will not be unpacked, and only sounds from already unpacked iconsets (that are not
//! stored in archives) will be played.
//! \a receiver and \a slot are used to specify the object that will be connected to
//! the signal 'playSound(QString fileName)'. Slot should play the specified sound
//! file.
void Iconset::setSoundPrefs(QString unpackPath, QObject *receiver, const char *slot)
{
#ifdef ICONSET_SOUND
	if ( !iconSharedObject )
		iconSharedObject = new IconSharedObject();

	iconSharedObject->unpackPath = unpackPath;
	QObject::connect(iconSharedObject, SIGNAL(playSound(QString)), receiver, slot);
#endif
}

#include "iconset.moc"
