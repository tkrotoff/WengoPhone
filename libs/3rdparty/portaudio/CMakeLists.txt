ow_create_static_library(portaudio)

ow_add_public_include_dirs(
	${CMAKE_CURRENT_SOURCE_DIR}/pa_common
)

ow_add_private_definitions(
	-DPA_LITTLE_ENDIAN
	-DPA_NO_ASIO
	-DPA_NO_WDMKS
	-DPA_NO_DS
)

ow_add_sources(
	pa_common/pa_allocation.c
	pa_common/pa_converters.c
	pa_common/pa_cpuload.c
	pa_common/pa_dither.c
	pa_common/pa_front.c
	pa_common/pa_process.c
	pa_common/pa_skeleton.c
	pa_common/pa_stream.c
	pa_common/pa_trace.c
	pa_win/pa_win_util.c
	pa_win/pa_win_hostapis.c
	pa_win/pa_x86_plain_converters.c
)

if (WITH_PORTAUDIO_DSOUND)
	ow_add_sources(
		pa_win_ds/dsound_wrapper.c
		pa_win_ds/pa_win_ds.c
	)
else (WITH_PORTAUDIO_DSOUND)
	ow_add_sources(
		pa_win_wmme/pa_win_wmme.c
	)
endif (WITH_PORTAUDIO_DSOUND)

ow_create_binary()
