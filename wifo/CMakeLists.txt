# opt flags for phapi and plugins
if (MSVC)
  set(PHAPI_OPTIMIZATION_FLAGS
    /Ox
    /Ob2
    /Oi
    /Ot
    /Oy
    /G7
    /GX-
    /GR-
    /arch:SSE
  )
else (MSVC)
  # TODO: we need a flag on other platforms
  # maybe unroll-loops is nice to have ;)
  set(PHAPI_OPTIMIZATION_FLAGS
    -funroll-loops
  )
endif (MSVC)

subdirs(
  libosip2
  eXosip
  httptunnel
  netlib
  ortp
  phapi-util
  phapi/amrnb
  phapi/amrwb
  phapi/fidlib
  phapi/gsm
  phapi/ilbc
  phapi/speexec
  phapi/stun
  phapi
  sfp-plugin
  #TODO: miniua
)

