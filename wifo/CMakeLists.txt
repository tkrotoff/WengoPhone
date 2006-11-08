# opt flags for phapi and plugins
if (MSVC)
	set(PHAPI_OPTIMIZATION_FLAGS
		#/Ox
		#/Ob2
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

if (WIN32)
	if (MSVC)
		subdirs(
			phapi/msdirent
		)
	endif (MSVC)

	subdirs(
		phapi/speex
	)
endif (WIN32)

subdirs(
	httptunnel
	netlib
	ortp
	srtp
	libosip2
	sVoIP
	eXosip
	phapi-util
	phapi/fidlib
	phapi/gsm
	phapi/ilbc
	phapi/speexec
	phapi/stun
	phapi
	#miniua
	wifo-plugins/sfp-plugin
	#tests/WifoPluginsConsole
)
