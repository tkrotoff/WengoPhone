if (NOT SIP_FOUND)
	find_package(SIP)
endif (NOT SIP_FOUND)

if (SIP_FOUND)
	subdirs(
		sip
	)
endif (SIP_FOUND)
