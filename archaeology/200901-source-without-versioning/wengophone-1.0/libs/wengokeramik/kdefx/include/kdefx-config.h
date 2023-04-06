/*
   This file is part of the KDE libraries
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/* config.h: definitions hardcoded for win32 target */

#ifndef _KDE_WIN_CONFIG_H_
#define _KDE_WIN_CONFIG_H_

#include <kdelibs_export.h>

#ifndef HAVE_LIMITS_H
#define HAVE_LIMITS_H
#endif

#ifndef HAVE_SYS_STAT_H
#define HAVE_SYS_STAT_H
#endif

#ifndef HAVE_SYS_TIME_H
#define HAVE_SYS_TIME_H
#endif

#ifndef M_PI
#define M_PI            3.14159265358979323846f
#endif

#define KDEDIR "F:\\KDE" /* TODO */

#define KDELIBSUFF ""

#ifdef __cplusplus
extern "C" {
#endif

KDECORE_EXPORT int mkstemps (char* _template, int suffix_len);
KDECORE_EXPORT char* mkdtemp (char* _template);

#ifdef __cplusplus
}
#endif

#endif /* _KDE_WIN_CONFIG_H_ */
