project(openssl)

if (UNIX)
  find_package(OpenSSL REQUIRED)
endif (UNIX)

if (OPENSSL_FOUND)
  set(OPENSSL_INCLUDE_DIRS
    ${OPENSSL_INCLUDE_DIRS}
    CACHE INTERNAL "openssl include directories"
  )

  set(OPENSSL_LIBRARIES
    ${OPENSSL_LIBRARIES}
    CACHE INTERNAL "openssl libraries"
  )
else (OPENSSL_FOUND)
  # needed include directories to build openssl
  # saves the variable in internal cache for later use
  set(OPENSSL_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc32
    CACHE INTERNAL "openssl include directories"
  )

  if (MSVC)
    find_library(LIBEAY32_LIBRARY
      NAMES
        libeay32
      PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/out32/msvc
    )

    find_library(OPENSSL_LIBRARY
      NAMES
        openssl
      PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/out32/msvc
    )

    find_library(SSLEAY32_LIBRARY
      NAMES
        ssleay32
      PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/out32/msvc
    )
  endif (MSVC)

  if (MINGW)
    find_library(LIBEAY32_LIBRARY
      NAMES
        libeay32
      PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/out32/mingw
    )

    find_library(OPENSSL_LIBRARY
      NAMES
        openssl
      PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/out32/mingw
    )

    find_library(SSLEAY32_LIBRARY
      NAMES
        ssleay32
      PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/out32/mingw
    )
  endif (MINGW)

  # openssl lib and dependencies
  # saves the variable in internal cache for later use
  set(OPENSSL_LIBRARIES
    ${LIBEAY32_LIBRARY}
    ${OPENSSL_LIBRARY}
    ${SSLEAY32_LIBRARY}
    CACHE INTERNAL "openssl libraries"
  )
endif (OPENSSL_FOUND)

