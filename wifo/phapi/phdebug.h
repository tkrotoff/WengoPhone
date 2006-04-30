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
/*
debugging is also helped by spikes in the code :
these are specific comments that keep track of a specific feature and are
a mean to answer questions like :
- how does the rtp stack adheres in the code
- how is half duplex implemented ?

available SPIKES are :
- SPIKE_HDX : half duplex

*/

// global definitions

// SIP_NEGO
#if 0
#define DBG4_SIP_NEGO(x1, x2, x3, x4) printf(x1, x2, x3, x4)
#define DBG8_SIP_NEGO(x1, x2, x3, x4, x5, x6, x7, x8) printf(x1, x2, x3, x4, x5, x6, x7, x8)
#else
#define DBG4_SIP_NEGO(x1, x2, x3, x4)
#define DBG8_SIP_NEGO(x1, x2, x3, x4, x5, x6, x7, x8)
#endif

// MEDIA_ENGINE
#if 0
#define DBG1_MEDIA_ENGINE(x1) printf(x1)
#define DBG2_MEDIA_ENGINE(x1, x2) printf(x1, x2)
#define DBG3_MEDIA_ENGINE(x1, x2, x3) printf(x1, x2, x3)
#define DBG4_MEDIA_ENGINE(x1, x2, x3, x4) printf(x1, x2, x3, x4)
#define DBG8_MEDIA_ENGINE(x1, x2, x3, x4, x5, x6, x7, x8) printf(x1, x2, x3, x4, x5, x6, x7, x8)
#else
#define DBG1_MEDIA_ENGINE(x1)
#define DBG2_MEDIA_ENGINE(x1, x2)
#define DBG3_MEDIA_ENGINE(x1, x2, x3)
#define DBG4_MEDIA_ENGINE(x1, x2, x3, x4)
#define DBG8_MEDIA_ENGINE(x1, x2, x3, x4, x5, x6, x7, x8)
#endif

// CODEC_LOOKUP
#if 0
#define DBG4_CODEC_LOOKUP(x1, x2, x3, x4) printf(x1, x2, x3, x4)
#else
#define DBG4_CODEC_LOOKUP(x1, x2, x3, x4)
#endif

// DYNA_AUDIO
#if 0
#define DBG1_DYNA_AUDIO(x1) printf(x1)
#define DBG5_DYNA_AUDIO(x1, x2, x3, x4, x5) printf(x1, x2, x3, x4, x5)
#else
#define DBG1_DYNA_AUDIO(x1)
#define DBG5_DYNA_AUDIO(x1, x2, x3, x4, x5)
#endif

// DYNA_AUDIO_DRV
#if 0
#define DBG5_DYNA_AUDIO_DRV(x1, x2, x3, x4, x5) printf(x1, x2, x3, x4, x5)
#define DBG8_DYNA_AUDIO_DRV(x1, x2, x3, x4, x5, x6, x7, x8) printf(x1, x2, x3, x4, x5, x6, x7, x8)
#else
#define DBG5_DYNA_AUDIO_DRV(x1, x2, x3, x4, x5)
#define DBG8_DYNA_AUDIO_DRV(x1, x2, x3, x4, x5, x6, x7, x8)
#endif

// DYNA_AUDIO_ECHO
#if 0
#define DBG1_DYNA_AUDIO_ECHO(x1) printf(x1)
#define DBG5_DYNA_AUDIO_ECHO(x1, x2, x3, x4, x5) printf(x1, x2, x3, x4, x5)
#define DBG8_DYNA_AUDIO_ECHO(x1, x2, x3, x4, x5, x6, x7, x8) printf(x1, x2, x3, x4, x5, x6, x7, x8)
#else
#define DBG1_DYNA_AUDIO_ECHO(x1)
#define DBG5_DYNA_AUDIO_ECHO(x1, x2, x3, x4, x5)
#define DBG8_DYNA_AUDIO_ECHO(x1, x2, x3, x4, x5, x6, x7, x8)
#endif

// DYNA_AUDIO_RX
#if 0
#define DBG5_DYNA_AUDIO_RX(x1, x2, x3, x4, x5) printf(x1, x2, x3, x4, x5)
#else
#define DBG5_DYNA_AUDIO_RX(x1, x2, x3, x4, x5)
#endif

// DYNA_AUDIO_TX
#if 0
#define DBG5_DYNA_AUDIO_TX(x1, x2, x3, x4, x5) printf(x1, x2, x3, x4, x5)
#else
#define DBG5_DYNA_AUDIO_TX(x1, x2, x3, x4, x5)
#endif

#endif