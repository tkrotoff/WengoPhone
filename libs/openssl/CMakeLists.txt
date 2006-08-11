project(openssl)

#includes
set(openssl_INCLUDE_DIR
	${openssl_SOURCE_DIR}/inc32
	CACHE INTERNAL ""
)

include_directories(${owserializer_INCLUDE_DIR})
