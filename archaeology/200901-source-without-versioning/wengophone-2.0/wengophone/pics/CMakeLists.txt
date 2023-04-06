project(pics)

file(GLOB_RECURSE _pics ${CMAKE_CURRENT_SOURCE_DIR}/*.png)

if (_pics)
  foreach (_pic ${_pics})
      file(RELATIVE_PATH _path ${CMAKE_CURRENT_SOURCE_DIR} ${_pic})
      get_filename_component(_path ${_path} PATH)

      install(
        FILES
          ${_pic}
        DESTINATION
          ${DATA_INSTALL_DIR}/pics/${_path}
      )
  endforeach (_pic ${_pics})
endif (_pics)

