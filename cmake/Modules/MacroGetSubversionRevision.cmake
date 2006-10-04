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
    message("-- Generating subversion revison")
    execute_process(
      COMMAND
        svnversion -n ${CMAKE_SOURCE_DIR}/cmake
      RESULT_VARIABLE
        _SVN_REVISION_RESULT_VARIABLE
      OUTPUT_VARIABLE
        _SVN_REVISION_OUTPUT_VARIABLE
    )

    if (_SVN_REVISION_RESULT_VARIABLE EQUAL 0)
      set(_SVN_REVISION ${_SVN_REVISION_OUTPUT_VARIABLE})
    else (_SVN_REVISION_RESULT_VARIABLE EQUAL 0)
      set(_SVN_REVISION 0)
    endif (_SVN_REVISION_RESULT_VARIABLE EQUAL 0)
  endif (_SVN_EXECUTEABLE AND _SVN_DOT_DIR)

  set(${_revision}
    ${_SVN_REVISION}
    CACHE INTERNAL "svn revison"
  )
endmacro (MACRO_GET_SUBVERSION_REVISION _revision)

