project(owcurl)

if (WIN32)
  find_package(OpenSSLInternal REQUIRED)
else (WIN32)
  find_package(OpenSSL REQUIRED)
endif (WIN32)

# needed include directories to build owcurl
# saves the variable in internal cache for later use
set(OWCURL_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OPENSSL_INCLUDE_DIRS}
  CACHE INTERNAL "owcurl include directories"
)

# owcurl lib and dependencies
# saves the variable in internal cache for later use
set(OWCURL_LIBRARIES
  owcurl
  ${OPENSSL_LIBRARIES}
  CACHE INTERNAL "owcurl libraries"
)

set(OWCURL_DEFINITIONS
  -DBUILDING_LIBCURL
  -DCURL_DISABLE_FILE
  -DCURL_DISABLE_LDAP
  -DCURL_DISABLE_DICT
  -DCURL_DISABLE_TELNET
  -DCURL_DISABLE_TFTP
  -DUSE_SSLEAY
  -DUSE_OPENSSL
)

if (APPLE)
  find_package(ZILIB REQUIRED)

  # FIXME: should be ${ZLIB_INCLUDE_DIRS} soon
  set(OWCURL_INCLUDE_DIRS
    ${OWCURL_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIR}
    CACHE INTERNAL "owcurl include directories"
  )

  set(OWCURL_LIBRARIES
    ${OWCURL_LIBRARIES}
    ${ZLIB_LIBRARIES}
    CACHE INTERNAL "owcurl libraries"
  )

  set(OWCURL_DEFINITIONS
    ${OWCURL_DEFINITIONS}
    -DCURL_STATICLIB
  )
endif (APPLE)

include_directories(
  ${OWCURL_INCLUDE_DIRS}
)

add_definitions(
  ${OWCURL_DEFINITIONS}
)

set(owcurl_SRCS
  lib/base64.c
  lib/connect.c
  lib/content_encoding.c
  lib/cookie.c
  #lib/dict.c
  lib/easy.c
  lib/escape.c
  #lib/file.c
  lib/formdata.c
  lib/ftp.c
  lib/getenv.c
  lib/getinfo.c
  lib/gtls.c
  lib/hash.c
  lib/hostares.c
  lib/hostasyn.c
  lib/hostip.c
  lib/hostip4.c
  lib/hostip6.c
  lib/hostsyn.c
  lib/hostthre.c
  lib/http.c
  lib/http_chunks.c
  lib/http_digest.c
  lib/http_negotiate.c
  lib/http_ntlm.c
  lib/if2ip.c
  lib/inet_ntop.c
  lib/inet_pton.c
  lib/krb4.c
  #lib/ldap.c
  lib/llist.c
  lib/md5.c
  lib/memdebug.c
  lib/mprintf.c
  lib/multi.c
  lib/netrc.c
  #lib/nwlib.c
  lib/parsedate.c
  lib/progress.c
  lib/security.c
  lib/select.c
  lib/sendf.c
  lib/share.c
  lib/speedcheck.c
  lib/sslgen.c
  lib/ssluse.c
  lib/strequal.c
  lib/strerror.c
  lib/strtok.c
  lib/strtoofft.c
  #lib/telnet.c
  #lib/tftp.c
  lib/timeval.c
  lib/transfer.c
  lib/url.c
  lib/version.c
)

add_library(owcurl STATIC ${owcurl_SRCS})

target_link_libraries(${OWCURL_LIBRARIES})

