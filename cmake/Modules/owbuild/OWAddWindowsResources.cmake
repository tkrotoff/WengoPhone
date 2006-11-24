# - OW_ADD_WINDOWS_RESOURCES()
# shows debug informations about the current project
#
# Copyright (C) 2006  Wengo
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING file.


macro (OW_ADD_WINDOWS_RESOURCES outfiles)

    FOREACH (it ${ARGN})
      GET_FILENAME_COMPONENT(outfilename ${it} NAME_WE)
      GET_FILENAME_COMPONENT(infile ${it} ABSOLUTE)
      GET_FILENAME_COMPONENT(rc_path ${infile} PATH)
      SET(outfile ${CMAKE_CURRENT_BINARY_DIR}/qrc_${outfilename}.cxx)
      #  parse file for dependencies
      FILE(READ "${infile}" _RC_FILE_CONTENTS)
      STRING(REGEX MATCHALL "<file>[^<]*" _RC_FILES "${_RC_FILE_CONTENTS}")
      SET(_RC_DEPENDS)
      FOREACH(_RC_FILE ${_RC_FILES})
        STRING(REGEX REPLACE "^<file>" "" _RC_FILE "${_RC_FILE}")
        SET(_RC_DEPENDS ${_RC_DEPENDS} "${rc_path}/${_RC_FILE}")
      ENDFOREACH(_RC_FILE)
      ADD_CUSTOM_COMMAND(OUTPUT ${outfile}
        COMMAND ${QT_RCC_EXECUTABLE}
        ARGS -name ${outfilename} -o ${outfile} ${infile}
        MAIN_DEPENDENCY ${infile}
        DEPENDS ${_RC_DEPENDS})
      SET(${outfiles} ${${outfiles}} ${outfile})
    ENDFOREACH (it)

endmacro (OW_ADD_WINDOWS_RESOURCES)
