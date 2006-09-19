#
# try to find GLIB2
#
# GLIB2_INCLUDE_DIRS  - Directories to include to use GLIB2
# GLIB2_LIBRARIES     - Files to link against to use GLIB2
# GLIB2_FOUND         - If false, don't try to use GLIB2
#
###################################################################
#
#  Copyright (c) 2004 Jan Woetzel
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#  Copyright (c) 2006 Philippe Bernery <philippe.bernery@gmail.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
#
###################################################################
#
#  Copyright (c) 2004 Jan Woetzel
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#  Copyright (c) 2006 Philippe Bernery <philippe.bernery@gmail.com>
#  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in
#   the documentation and/or other materials provided with the
#   distribution.
#
# * Neither the name of the <ORGANIZATION> nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

IF (GLIB2_LIBRARIES AND GLIB2_INCLUDE_DIRS)
  # in cache already
  SET(GLIB2_FOUND TRUE)
ELSE (GLIB2_LIBRARIES AND GLIB2_INCLUDE_DIRS)
  IF(UNIX)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    INCLUDE(UsePkgConfig)

    # Some Linux distributions (e.g. Red Hat) have glibconfig.h
    # and glib.h in different directories, so we need to look
    # for both.
    #  - Atanas Georgiev <atanas@cs.columbia.edu>
    PKGCONFIG(glib-2.0 _GLIB2IncDir _GLIB2inkDir _GLIB2LinkFlags _GLIB2Cflags)
    PKGCONFIG(gmodule-2.0 _GMODULE2IncDir _GMODULE2inkDir _GMODULE2LinkFlags _GMODULE2Cflags)
    SET(GDIR /opt/gnome/lib/glib-2.0/include)

    FIND_PATH(GLIB2_GLIBCONFIG_INCLUDE_PATH glibconfig.h
      ${_GLIB2IncDir}
      /opt/gnome/lib64/glib-2.0/include
      /opt/gnome/lib/glib-2.0/include
      /opt/lib/glib-2.0/include
      /usr/lib64/glib-2.0/include
      /usr/lib/glib-2.0/include
      /sw/lib/glib-2.0/include
    )
    #MESSAGE(STATUS "DEBUG: GLIB2_GLIBCONFIG_INCLUDE_PATH = ${GLIB2_GLIBCONFIG_INCLUDE_PATH}")

    FIND_PATH(GLIB2_GLIB_INCLUDE_PATH glib.h
      ${_GLIB2IncDir}
      /opt/include/glib-2.0
      /opt/gnome/include/glib-2.0
      /usr/include/glib-2.0
      /sw/include/glib-2.0
    )

    FIND_PATH(GLIB2_INTL_INCLUDE_PATH libintl.h
      /opt/include
      /opt/gnome/include
      /usr/include
      /sw/include
    )

    FIND_LIBRARY(GLIB2_INTL_LIBRARY
      NAMES
        intl
      PATHS
        /usr/lib
        /usr/local/lib
        /usr/openwin/lib
        /usr/X11R6/lib
        /opt/gnome/lib
        /opt/lib
        /sw/lib
    )

    FIND_LIBRARY(GLIB2_GMODULE_LIBRARY
      NAMES
        gmodule-2.0
      PATHS
        ${_GMODULE2inkDir}
        /usr/lib
        /usr/local/lib
        /usr/openwin/lib
        /usr/X11R6/lib
        /opt/gnome/lib
        /opt/lib
        /sw/lib
    )

    FIND_LIBRARY(GLIB2_GLIB_LIBRARY
      NAMES
        glib-2.0
      PATHS
        ${_GLIB2inkDir}
        /usr/lib
        /usr/local/lib
        /usr/openwin/lib
        /usr/X11R6/lib
        /opt/gnome/lib
        /opt/lib
        /sw/lib
    )

    FIND_LIBRARY(GLIB2_GTHREAD_LIBRARY
      NAMES
        gthread-2.0
      PATHS
        /usr/lib
        /usr/local/lib
        /usr/openwin/lib
        /usr/X11R6/lib
        /opt/gnome/lib
        /opt/lib
        /sw/lib
    )


    FIND_LIBRARY(GLIB2_GOBJECT_LIBRARY
      NAMES
        gobject-2.0
      PATHS
        /usr/lib
        /usr/local/lib
        /usr/openwin/lib
        /usr/X11R6/lib
        /opt/gnome/lib
        /opt/lib
        /sw/lib
    )

    IF(GLIB2_GLIBCONFIG_INCLUDE_PATH)
      IF(GLIB2_GLIB_INCLUDE_PATH)
        IF(GLIB2_GLIB_LIBRARY)
          # Assume that if glib were found, the other
          # supporting libraries have also been found.

          SET(GLIB2_FOUND TRUE)

          SET(GLIB2_INCLUDE_DIRS
            ${GLIB2_GLIBCONFIG_INCLUDE_PATH}
            ${GLIB2_GLIB_INCLUDE_PATH}
            ${GLIB2_INTL_INCLUDE_PATH}
          )

          SET(GLIB2_LIBRARIES
            ${GLIB2_GLIB_LIBRARY}
            ${GLIB2_INTL_LIBRARY}
          )
          #${GLIB2_GOBJECT_LIBRARY})

          IF(GLIB2_GMODULE_LIBRARY)
            SET(GLIB2_LIBRARIES
              ${GLIB2_LIBRARIES}
              ${GLIB2_GMODULE_LIBRARY}
            )
          ENDIF(GLIB2_GMODULE_LIBRARY)

          IF(GLIB2_GTHREAD_LIBRARY)
            SET(GLIB2_LIBRARIES
              ${GLIB2_LIBRARIES}
              ${GLIB2_GTHREAD_LIBRARY}
            )
            SET(GLIB2_LIBRARIES ${GLIB2_LIBRARIES})
          ENDIF(GLIB2_GTHREAD_LIBRARY)
        ELSE(GLIB2_GLIB_LIBRARY)
          MESSAGE(STATUS "Can not find glib lib")
        ENDIF(GLIB2_GLIB_LIBRARY)
      ELSE(GLIB2_GLIB_INCLUDE_PATH)
        MESSAGE(STATUS "Can not find glib includes")
      ENDIF(GLIB2_GLIB_INCLUDE_PATH)
    ELSE(GLIB2_GLIBCONFIG_INCLUDE_PATH)
      MESSAGE(STATUS "Can not find glibconfig")
    ENDIF(GLIB2_GLIBCONFIG_INCLUDE_PATH)

    IF (GLIB2_FOUND)
      IF (NOT GLIB2_FIND_QUIETLY)
        MESSAGE(STATUS "Found GLIB2: ${GLIB2_LIBRARIES}")
      ENDIF (NOT GLIB2_FIND_QUIETLY)
    ELSE (GLIB2_FOUND)
      IF (GLIB2_FIND_REQUIRED)
        MESSAGE(SEND_ERROR "Could NOT find GLIB2")
      ENDIF (GLIB2_FIND_REQUIRED)
    ENDIF (GLIB2_FOUND)

    MARK_AS_ADVANCED(
      GLIB2_GLIB_INCLUDE_PATH
      GLIB2_GLIB_LIBRARY
      GLIB2_GLIBCONFIG_INCLUDE_PATH
      GLIB2_GMODULE_LIBRARY
      GLIB2_GTHREAD_LIBRARY
      #GLIB2_GOBJECT_LIBRARY
    )
  ENDIF(UNIX)
ENDIF (GLIB2_LIBRARIES AND GLIB2_INCLUDE_DIRS)

# vim:et ts=2 sw=2 comments=\:\#
