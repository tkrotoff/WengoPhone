/*
 * phlog - logger for phapi
 *
 * Copyright (C) 2006 Wengo SAS
 * Author: Mathieu Stute <mathieu.stute@wengo.fr>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2,
 * or (at your option) any later version.
 *
 * This is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with dpkg; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __PHLOG_H__
#define __PHLOG_H__

/*********************User configuration part *************************/

/* log filename */
#define LOG_FILE "phlogger.log"

/* 0 to disable, 1 log to file LOG_FILE and 2 log to stdout */
#define ACTIVATE_SIPNEGO_DBG 0
#define ACTIVATE_MEDIA_ENGINE_DBG 0
#define ACTIVATE_MEDIA_ENGINE_VIDEO_DBG 1
#define ACTIVATE_CODEC_LOOKUP_DBG 0
#define ACTIVATE_DYNA_AUDIO_DBG 0
#define ACTIVATE_DYNA_AUDIO_RESAMPLE_DBG 0
#define ACTIVATE_DYNA_AUDIO_ECHO_DBG 0
#define ACTIVATE_DYNA_AUDIO_RX_DBG 0
#define ACTIVATE_DYNA_AUDIO_TX_DBG 0
#define ACTIVATE_DYNA_AUDIO_DRV_DBG 0

/*********************End of user configuration part *************************/

# include <stdio.h>

void init_log(char *file, char *mode);
int logToFile(char *fmt, ...);

#ifdef _WIN32
  #pragma warning(disable : 4548)
#endif

#define print_dbg(x) printf("%s:%s:%d ",x, __FILE__, __LINE__); printf

#if (ACTIVATE_SIPNEGO_DBG == 1)
  #define DBG_SIP_NEGO logToFile
#elif (ACTIVATE_SIPNEGO_DBG == 2)
  #define DBG_SIP_NEGO print_dbg("DBG_SIP_NEGO")
#else
  #ifndef _WIN32
    #define DBG_SIP_NEGO(...)
  #else
    #define DBG_SIP_NEGO (void)
  #endif
#endif

#if (ACTIVATE_MEDIA_ENGINE_DBG == 1)
  #define DBG_MEDIA_ENGINE logToFile
#elif (ACTIVATE_MEDIA_ENGINE_DBG == 2)
  #define DBG_MEDIA_ENGINE print_dbg("DBG_MEDIA_ENGINE")
#else
  #ifndef _WIN32
    #define DBG_MEDIA_ENGINE(...)
  #else
    #define DBG_MEDIA_ENGINE (void)
  #endif
#endif

#if (ACTIVATE_MEDIA_ENGINE_VIDEO_DBG == 1)
  #define DBG_MEDIA_ENGINE_VIDEO logToFile
#elif (ACTIVATE_MEDIA_ENGINE_VIDEO_DBG == 2)
  #define DBG_MEDIA_ENGINE_VIDEO print_dbg("DBG_MEDIA_ENGINE_VIDEO")
#else
  #ifndef _WIN32
    #define DBG_MEDIA_ENGINE_VIDEO(...)
  #else
    #define DBG_MEDIA_ENGINE_VIDEO (void)
  #endif
#endif

#if (ACTIVATE_CODEC_LOOKUP_DBG == 1)
  #define DBG_CODEC_LOOKUP logToFile
#elif (ACTIVATE_CODEC_LOOKUP_DBG == 2)
  #define DBG_CODEC_LOOKUP print_dbg("DBG_CODEC_LOOKUP")
#else
  #ifndef _WIN32
    #define DBG_CODEC_LOOKUP(...)
  #else
    #define DBG_CODEC_LOOKUP (void)
  #endif
#endif

#if (ACTIVATE_DYNA_AUDIO_DBG == 1)
  #define DBG_DYNA_AUDIO logToFile
#elif (ACTIVATE_DYNA_AUDIO_DBG == 2)
  #define DBG_DYNA_AUDIO print_dbg("DBG_DYNA_AUDIO")
#else
  #ifndef _WIN32
    #define DBG_DYNA_AUDIO(...)
  #else
    #define DBG_DYNA_AUDIO (void)
  #endif
#endif

#if (ACTIVATE_DYNA_AUDIO_RESAMPLE_DBG == 1)
  #define DBG_DYNA_AUDIO_RESAMPLE logToFile
#elif (ACTIVATE_DYNA_AUDIO_RESAMPLE_DBG == 2)
  #define DBG_DYNA_AUDIO_RESAMPLE print_dbg("DBG_DYNA_AUDIO_RESAMPLE")
#else
  #ifndef _WIN32
    #define DBG_DYNA_AUDIO_RESAMPLE(...)
  #else
    #define DBG_DYNA_AUDIO_RESAMPLE (void)
  #endif
#endif

#if (ACTIVATE_DYNA_AUDIO_ECHO_DBG == 1)
  #define DBG_DYNA_AUDIO_ECHO logToFile
#elif (ACTIVATE_DYNA_AUDIO_ECHO_DBG == 2)
  #define DBG_DYNA_AUDIO_ECHO print_dbg("DBG_DYNA_AUDIO_ECHO")
#else
  #ifndef _WIN32
    #define DBG_DYNA_AUDIO_ECHO(...)
  #else
    #define DBG_DYNA_AUDIO_ECHO (void)
  #endif
#endif

#if (ACTIVATE_DYNA_AUDIO_RX_DBG == 1)
  #define DBG_DYNA_AUDIO_RX logToFile
#elif (ACTIVATE_DYNA_AUDIO_RX_DBG == 2)
  #define DBG_DYNA_AUDIO_RX print_dbg("DBG_DYNA_AUDIO_RX")
#else
  #ifndef _WIN32
    #define DBG_DYNA_AUDIO_RX(...)
  #else
    #define DBG_DYNA_AUDIO_RX (void)
  #endif
#endif

#if (ACTIVATE_DYNA_AUDIO_TX_DBG == 1)
  #define DBG_DYNA_AUDIO_TX logToFile
#elif (ACTIVATE_DYNA_AUDIO_TX_DBG == 2)
  #define DBG_DYNA_AUDIO_TX print_dbg("DBG_DYNA_AUDIO_TX")
#else
  #ifndef _WIN32
    #define DBG_DYNA_AUDIO_TX(...)
  #else
    #define DBG_DYNA_AUDIO_TX (void)
  #endif
#endif

#if (ACTIVATE_DYNA_AUDIO_DRV_DBG == 1)
  #define DBG_DYNA_AUDIO_DRV logToFile
#elif (ACTIVATE_DYNA_AUDIO_DRV_DBG == 2)
  #define DBG_DYNA_AUDIO_DRV print_dbg("DBG_DYNA_AUDIO_DRV")
#else
  #ifndef _WIN32
    #define DBG_DYNA_AUDIO_DRV(...)
  #else
    #define DBG_DYNA_AUDIO_DRV (void)
  #endif
#endif

#endif  //__PHLOG_H__
