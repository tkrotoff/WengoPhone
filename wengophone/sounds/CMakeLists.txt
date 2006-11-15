project(sounds)

file(GLOB_RECURSE soundFileList ${CMAKE_CURRENT_SOURCE_DIR}/*.wav)

if (soundList)
	foreach (sound ${soundList})
		file(RELATIVE_PATH path ${CMAKE_CURRENT_SOURCE_DIR} ${sound})
		get_filename_component(path ${path} PATH)

	install(FILES ${sound}
		DESTINATION ${DATA_INSTALL_DIR}/sounds/${_path}
	)
	endforeach (sound ${soundList})
endif (soundList)
