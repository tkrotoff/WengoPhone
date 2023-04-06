project(util)

subdirs(
  cutil
  util
  serialization
  serializer
  thread
  settings
  shlibloader
  socket
  system
  http
)

if (WIN32)
  subdirs(
    memorydump
  )
endif (WIN32)
