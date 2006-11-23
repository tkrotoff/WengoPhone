ow_create_plugin_library(phamrplugin)

ow_use_private_libraries(
	amrwb
	amrnb
)

ow_add_private_compile_flags(
	${PHAPI_OPTIMIZATION_FLAGS}
)

ow_add_sources(
	../phcodec-amr-wrapper.c
)

if (WITH_SPEEXWB_REPLACES_G726WB)
	ow_add_private_definitions(
		-DPH_SPEEXWB_REPLACES_G726WB
	)
else (WITH_SPEEXWB_REPLACES_G726WB)
	if (WITH_SPEEXWB_REPLACES_AMRNB)
		ow_add_private_definitions(
			-DPH_SPEEXWB_REPLACES_AMRNB
		)
	else (WITH_SPEEXWB_REPLACES_AMRNB)
		if (WITH_SPEEXWB_REPLACES_AMRWB)
			ow_add_private_definitions(
				-DPH_SPEEXWB_REPLACES_AMRWB
			)
		endif (WITH_SPEEXWB_REPLACES_AMRWB)
	endif (WITH_SPEEXWB_REPLACES_AMRNB)
endif (WITH_SPEEXWB_REPLACES_G726WB)

ow_create_binary()
