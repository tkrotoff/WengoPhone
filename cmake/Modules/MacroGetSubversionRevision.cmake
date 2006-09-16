# - MACRO_GET_SUBVERSION_REVISION(<variable>)

macro (MACRO_GET_SUBVERSION_REVISION _revision)
  find_program(_SVN_EXECUTEABLE
    NAMES
      svn
    PATHS
      /usr/bin
      /usr/local/bin
  )

  find_file(_SVN_DOT_DIR
    NAMES
      entries
    PATHS
      ${CMAKE_SOURCE_DIR}/.svn
  )

  if (_SVN_EXECUTEABLE AND _SVN_DOT_DIR)
    execute_process(
      COMMAND
        svn info ${CMAKE_SOURCE_DIR}
      RESULT_VARIABLE
        _SVN_REVISION_RESULT_VARIABLE
      OUTPUT_VARIABLE
        _SVN_REVISION_OUTPUT_VARIABLE
    )

    string(REGEX REPLACE ".*Revision: ([0-9]+).*" "\\1" _SVN_REVISION ${_SVN_REVISION_OUTPUT_VARIABLE})
  endif (_SVN_EXECUTEABLE AND _SVN_DOT_DIR)

  set(${_revision} ${${_revision}} ${_SVN_REVISION})
endmacro (MACRO_GET_SUBVERSION_REVISION _revision)

