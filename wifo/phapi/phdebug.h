#ifndef __PHDEBUG_H__
#define __PHDEBUG_H__ 1

/*
// debugging in phapi is currently oriented towards selective/functional debugging
// several functional sections are defined that are used across phapi

// this "debugging" architecture is not perfect, but is there as a first step
// in order to remove all printf from the code

SIP_NEGO : around SIP and call state machines

MEDIA_ENGINE : around the activation of audio device and rtp activation in general

CODEC_LOOKUP : around CODEC initialization and lookup

DYNA_AUDIO : around the audio dynamics

*/

// global definitions

#define DBG4_SIP_NEGO(x1, x2, x3, x4)
#define DBG8_SIP_NEGO(x1, x2, x3, x4, x5, x6, x7, x8)
#if 0
#define DBG4_MEDIA_ENGINE(x1, x2, x3, x4)
#define DBG8_MEDIA_ENGINE(x1, x2, x3, x4, x5, x6, x7, x8)
#endif
#define DBG4_CODEC_LOOKUP(x1, x2, x3, x4)
#define DBG5_DYNA_AUDIO(x1, x2, x3, x4, x5)
#define DBG5_DYNA_AUDIO_ECHO(x1, x2, x3, x4, x5)
#define DBG8_DYNA_AUDIO_ECHO(x1, x2, x3, x4, x5, x6, x7, x8)
#define DBG5_DYNA_AUDIO_RX(x1, x2, x3, x4, x5)
#define DBG5_DYNA_AUDIO_TX(x1, x2, x3, x4, x5)

#if 0
#define DBG4_SIP_NEGO(x1, x2, x3, x4) printf(x1, x2, x3, x4)
#define DBG8_SIP_NEGO(x1, x2, x3, x4, x5, x6, x7, x8) printf(x1, x2, x3, x4, x5, x6, x7, x8)
#endif

#if 1
#define DBG1_MEDIA_ENGINE(x1) printf(x1)
#define DBG4_MEDIA_ENGINE(x1, x2, x3, x4) printf(x1, x2, x3, x4)
#define DBG8_MEDIA_ENGINE(x1, x2, x3, x4, x5, x6, x7, x8) printf(x1, x2, x3, x4, x5, x6, x7, x8)
#endif

#if 0
#define DBG4_CODEC_LOOKUP(x1, x2, x3, x4) printf(x1, x2, x3, x4)
#endif

#if 0
#define DBG5_DYNA_AUDIO(x1, x2, x3, x4, x5) printf(x1, x2, x3, x4, x5)
#define DBG5_DYNA_AUDIO_ECHO(x1, x2, x3, x4, x5) printf(x1, x2, x3, x4, x5)
#define DBG8_DYNA_AUDIO_ECHO(x1, x2, x3, x4, x5, x6, x7, x8) printf(x1, x2, x3, x4, x5, x6, x7, x8)
#endif

#if 0
#define DBG5_DYNA_AUDIO_RX(x1, x2, x3, x4, x5) printf(x1, x2, x3, x4, x5)
#define DBG5_DYNA_AUDIO_TX(x1, x2, x3, x4, x5) printf(x1, x2, x3, x4, x5)
#endif

#endif
