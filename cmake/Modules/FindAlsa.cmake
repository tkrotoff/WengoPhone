# Alsa check, based on libkmid/configure.in.in.
# Only the support for Alsa >= 0.9.x was included; 0.5.x was dropped (but feel free to re-add it if you need it)

# Copyright (c) 2006, David Faure, <faure@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(CheckIncludeFiles)
include(CheckIncludeFileCXX)
include(CheckLibraryExists)

# Already done by toplevel
#check_include_files(sys/soundcard.h HAVE_SYS_SOUNDCARD_H)
#check_include_files(machine/soundcard.h HAVE_MACHINE_SOUNDCARD_H)

check_include_files(linux/awe_voice.h HAVE_LINUX_AWE_VOICE_H)
check_include_files(awe_voice.h HAVE_AWE_VOICE_H)
check_include_files(/usr/src/sys/i386/isa/sound/awe_voice.h HAVE__USR_SRC_SYS_I386_ISA_SOUND_AWE_VOICE_H)
check_include_files(/usr/src/sys/gnu/i386/isa/sound/awe_voice.h HAVE__USR_SRC_SYS_GNU_I386_ISA_SOUND_AWE_VOICE_H)

check_include_file_cxx(sys/asoundlib.h HAVE_SYS_ASOUNDLIB_H)
check_include_file_cxx(alsa/asoundlib.h HAVE_ALSA_ASOUNDLIB_H)

check_library_exists(asound snd_seq_create_simple_port "" HAVE_LIBASOUND2)
if(HAVE_LIBASOUND2)
    set(ASOUND_LIBRARY "asound")
    check_library_exists(asound snd_pcm_resume ${ASOUND_LIBRARY} ASOUND_HAS_SND_PCM_RESUME)
    message(STATUS "Found ALSA: ${ASOUND_LIBRARY}")
    if(ASOUND_HAS_SND_PCM_RESUME)
        set(HAVE_SND_PCM_RESUME 1)
    endif(ASOUND_HAS_SND_PCM_RESUME)
else(HAVE_LIBASOUND2)
    message(STATUS "ALSA not found")
endif(HAVE_LIBASOUND2)

configure_file(${CMAKE_SOURCE_DIR}/cmake/Modules/config-alsa.h.cmake ${CMAKE_BINARY_DIR}/config-alsa.h )
