project(sounds)

file(GLOB_RECURSE _sounds ${CMAKE_CURRENT_SOURCE_DIR}/*.wav)

if (_sounds)
  foreach (_sound ${_sounds})
      file(RELATIVE_PATH _path ${CMAKE_CURRENT_SOURCE_DIR} ${_sound})
      get_filename_component(_path ${_path} PATH)

      install(
        FILES
          ${_sound}
        DESTINATION
          ${DATA_INSTALL_DIR}/sounds/${_path}
      )
  endforeach (_sound ${_sounds})
endif (_sounds)

