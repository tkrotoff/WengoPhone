project(owcurl)

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

add_definitions(
	-DBUILDING_LIBCURL
	-DCURL_DISABLE_FILE
	-DCURL_DISABLE_LDAP
	-DCURL_DISABLE_DICT
	-DCURL_DISABLE_TELNET
	-DCURL_DISABLE_TFTP
)

#includes
set(owcurl_INCLUDE_DIR
	${owcurl_SOURCE_DIR}/include
	CACHE INTERNAL ""
)

include_directories(${owcurl_INCLUDE_DIR} lib)


if (ENABLE_SSL)
	#find_library(OPENSSL_LIBRARY NAMES ssl PATHS /usr/lib /usr/local/lib /sw/lib /opt/lib)
	#target_link_libraries(owcurl ${OPENSSL_LIBRARY})
	add_definitions(-DUSE_SSLEAY -DUSE_OPENSSL)
endif (ENABLE_SSL)

if (APPLE)
	add_library(owcurl ${owcurl_SRCS})

	find_library(z_library names z paths /usr/lib /usr/local/lib)
	target_link_libraries(owcurl ${Z_LIBRARY})
	add_definitions(-DCURL_STATICLIB)
endif (APPLE)

if (WIN32)
	include_directories(${openssl_INCLUDE_DIR})
	link_directories(${openssl_SOURCE_DIR}/out32/msvc)
	add_library(owcurl SHARED ${owcurl_SRCS})
	target_link_libraries(owcurl
		libeay32
		openssl
		ssleay32
		ws2_32
		winmm
	)
endif (WIN32)
