/*
 * iconset.h - various graphics handling classes
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

#ifndef ICONSET_H
#define ICONSET_H

#include <qobject.h>
#include <qdict.h>
#include <qptrlist.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluevector.h>

class QImage;
class QPixmap;
class QIconSet;
class QMimeSourceFactory;
class Anim;

class Impix
{
public:
	Impix();
	Impix(const QPixmap &);
	Impix(const QImage &);
	Impix(const Impix &);
	Impix & operator=(const Impix &from);
	~Impix();

	void unload();
	bool isNull() const;

	const QPixmap & pixmap() const;
	const QImage & image() const;
	void setPixmap(const QPixmap &);
	void setImage(const QImage &);

	operator const QPixmap &() const { return pixmap(); }
	operator const QImage &() const { return image(); }
	Impix & operator=(const QPixmap &from) { setPixmap(from); return *this; }
	Impix & operator=(const QImage &from) { setImage(from); return *this; }

	bool loadFromData(const QByteArray &);

	Impix copy() const;
	void detach();

private:
	class Private;
	Private *d;
};

class Icon : public QObject
{
	Q_OBJECT
public:
	Icon();
	Icon(const Icon &);
	~Icon();

	Icon & operator= (const Icon &);

	//!
	//! Returns impix().pixmap().
	operator const QPixmap &() const { return impix().pixmap(); }

	//!
	//! Returns impix().image().
	operator const QImage &() const { return impix().image(); }

	//!
	//! see iconSet().
	operator const QIconSet &() const { return iconSet(); }

	virtual bool isAnimated() const;
	virtual const QPixmap &pixmap() const;
	virtual const QImage &image() const;
	virtual const QIconSet & iconSet() const;

	virtual const Impix &impix() const;
	virtual const Impix &frameImpix() const;
	void setImpix(const Impix &, bool doDetach = true);

	const Anim *anim() const;
	void setAnim(const Anim &, bool doDetach = true);
	void removeAnim(bool doDetach = true);

	virtual int framenumber() const;

	virtual const QString &name() const;
	void setName(const QString &);
	void setName(const char *);

	const QRegExp &regExp() const;
	void setRegExp(const QRegExp &);

	const QDict<QString> &text() const;
	void setText(const QDict<QString> &);

	const QString &sound() const;
	void setSound(const QString &);

	bool loadFromData(const QByteArray &, bool isAnimation);

	void stripFirstAnimFrame();

	Icon copy() const;
	void detach();

signals:
	void pixmapChanged(const QPixmap &);
	void iconModified(const QPixmap &);

public slots:
	virtual void activated(bool playSound = true);	// it just has been inserted in the text, or now it's being displayed by
							// some widget. icon should play sound and start animation

	virtual void stop();	// this icon is no more displaying. stop animation

public:
	class Private;
private:
	Private *d;
};

class Iconset
{
public:
	Iconset();
	Iconset(const Iconset &);
	~Iconset();

	Iconset &operator=(const Iconset &);
	Iconset &operator+=(const Iconset &);

	void clear();
	uint count() const;

	bool load(const QString &dir);

	const Icon *icon(const QString &) const;
	void setIcon(const QString &, const Icon &);
	void removeIcon(const QString &);

	const QString &name() const;
	const QString &version() const;
	const QString &description() const;
	const QStringList &authors() const;
	const QString &creation() const;
	const QString &homeUrl() const;

	const QString &fileName() const;
	void setFileName(const QString &);
	
	void setInformation(const Iconset &from);

	const QDict<QString> info() const;
	void setInfo(const QDict<QString> &);

	QPtrListIterator<Icon> iterator() const;

	QMimeSourceFactory *createMimeSourceFactory() const;

	void addToFactory() const;
	void removeFromFactory() const;

	static void setSoundPrefs(QString unpackPath, QObject *receiver, const char *slot);

	Iconset copy() const;
	void detach();

private:
	class Private;
	Private *d;
};

class IconsetFactory
{
public:
	static Icon icon(const QString &name);
	static const QPixmap &iconPixmap(const QString &name);

	static const Icon *iconPtr(const QString &name);
	static const QStringList icons();
};

#endif
