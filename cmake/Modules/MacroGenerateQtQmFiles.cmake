#
# - This macro generates qm files from ts files and installs them
#
#  Copyright (c) 2006 Andreas Schneider <mail@cynapses.org>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

macro(MACRO_GENERATE_QT_QM_FILES _langdir _sources _installdir)
  find_package(Qt4 REQUIRED)
  get_filename_component(_qt_qmake_path ${QT_QMAKE_EXECUTABLE} PATH)

  find_program(QT_LUPDATE_EXECUTABLE
    NAMES
      lupdate
    PATHS
      ${_qt_qmake_path}
      $ENV{QTDIR}/bin
      "[HKEY_CURRENT_USER\\Software\\Trolltech\\Qt3Versions\\4.0.0;InstallDir]/bin"
      "[HKEY_CURRENT_USER\\Software\\Trolltech\\Versions\\4.0.0;InstallDir]/bin"
  )

  find_program(QT_LRELEASE_EXECUTABLE
    NAMES
      lrelease
    PATHS
      ${_qt_qmake_path}
      $ENV{QTDIR}/bin
      "[HKEY_CURRENT_USER\\Software\\Trolltech\\Qt3Versions\\4.0.0;InstallDir]/bin"
      "[HKEY_CURRENT_USER\\Software\\Trolltech\\Versions\\4.0.0;InstallDir]/bin"
  )

  file(GLOB_RECURSE _ts_files ${CMAKE_CURRENT_SOURCE_DIR}/*.ts)

  if (_ts_files)
    foreach(_ts ${_ts_files})

      add_custom_command(
        OUTPUT
          ${_ts}
        COMMAND
          ${QT_LUPDATE_EXECUTABLE}
          ${${_sources}}
          -ts ${_ts}
      )

      get_filename_component(_basename ${_ts} NAME_WE)

      get_filename_component(_outfile
        ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.qm
        ABSOLUTE
      )

      add_custom_command(
        OUTPUT
          ${_outfile}
        COMMAND
          ${QT_LRELEASE_EXECUTABLE}
          ${_ts}
          -qm ${_outfile}
      )

      set(_qmfiles ${_qmfiles} ${_outfile})

      install(
        FILES
          ${_outfile}
        DESTINATION
          ${_installdir}
      )
    endforeach(_ts ${_ts_files})

    set(${_sources} ${${_sources}} ${_qmfiles})
  endif (_ts_files)
endmacro(MACRO_GENERATE_QT_QM_FILES _langdir _sources _installdir)

