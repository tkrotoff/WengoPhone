project(speex)

set(SPEEX_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  CACHE INTERNAL "speex include directory"
)

if (WIN32)
  set(SPEEX_INCLUDE_DIRS
    ${SPEEX_INCLUDE_DIRS}
    ${speex_SOURCE_DIR}/win32
    CACHE INTERNAL "speex include directory"
  )
endif (WIN32)
if (APPLE)
  set(SPEEX_INCLUDE_DIRS
    ${SPEEX_INCLUDE_DIRS}
    ${speex_SOURCE_DIR}/macosx
    CACHE INTERNAL "speex include directory"
  )
endif (APPLE)


set(SPEEX_LIBRARY
  speex
  CACHE INTERNAL "speex library"
)

set(SPEEX_LINK_LIBRARIES
  ${SPEEX_LIBRARY}
)

set(speex_SRCS
  libspeex/bits.c
  libspeex/cb_search.c
  libspeex/exc_10_16_table.c
  libspeex/exc_10_32_table.c
  libspeex/exc_20_32_table.c
  libspeex/exc_5_256_table.c
  libspeex/exc_5_64_table.c
  libspeex/exc_8_128_table.c
  libspeex/filters.c
  libspeex/gain_table.c
  libspeex/gain_table_lbr.c
  libspeex/hexc_10_32_table.c
  libspeex/hexc_table.c
  libspeex/high_lsp_tables.c
  libspeex/jitter.c
  libspeex/lbr_48k_tables.c
  libspeex/lpc.c
  libspeex/lsp.c
  libspeex/lsp_tables_nb.c
  libspeex/ltp.c
  libspeex/math_approx.c
  libspeex/mdf.c
  libspeex/misc.c
  libspeex/modes.c
  libspeex/nb_celp.c
  libspeex/preprocess.c
  libspeex/quant_lsp.c
  libspeex/sb_celp.c
  libspeex/smallft.c
  libspeex/speex.c
  libspeex/speex_callbacks.c
  libspeex/speex_header.c
  libspeex/stereo.c
  libspeex/vbr.c
  libspeex/vorbis_psy.c
  libspeex/vq.c
  libspeex/window.c
)

include_directories(${SPEEX_INCLUDE_DIRS})

add_library(${SPEEX_LIBRARY} STATIC ${speex_SRCS})
