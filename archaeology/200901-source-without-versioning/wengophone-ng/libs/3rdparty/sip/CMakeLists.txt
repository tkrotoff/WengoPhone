if (NOT SIP_FOUND)
	find_package(SIP)
endif (NOT SIP_FOUND)

ow_create_project(sip)

if (SIP_FOUND)
	ow_use_public_libraries(
		SIP
	)
endif (SIP_FOUND)
