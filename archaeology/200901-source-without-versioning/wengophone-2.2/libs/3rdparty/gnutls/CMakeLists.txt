if (UNIX)
	if (NOT GNUTLS_FOUND)
		find_package(GnuTLS REQUIRED)
	endif (NOT GNUTLS_FOUND)

	ow_create_project(gnutls)

	ow_use_public_libraries(
		GNUTLS
	)
endif (UNIX)
