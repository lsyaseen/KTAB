# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
# CMake comes with 'helper files' for many common libraries (e.g. X11).
# This is an example of how to build one for an uncommon library.
# --------------------------------------------

set(KUTILS_FOUND "NO")

set (KTAB_INSTALL_DIR)
if (WIN32)
  set (KTAB_INSTALL_DIR
  c:/local/ktab
  )
endif(WIN32)
if (UNIX)
  set (KTAB_INSTALL_DIR
  /usr/local/ktab
  )
endif(UNIX)

set(KUTILS_PREFIX
  ${KTAB_INSTALL_DIR} 
  )

# Note Well the locations in and order of search
set(KUTILS_POSSIBLE_PATHS
  ${KUTILS_PREFIX}/include/
  ${KUTILS_PREFIX}/lib/
  ${CMAKE_CURRENT_SOURCE_DIR}/../kutils/
  ${CMAKE_CURRENT_SOURCE_DIR}/../build/kutils/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../kutils/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../KTAB/kutils/
  )

# try to find a key header
find_path(KUTILS_INCLUDE_DIR kutils.h
  PATH_SUFFIXES libsrc
  PATHS ${KUTILS_POSSIBLE_PATHS}
  )

# try to find the compiled library object
find_library (KUTILS_LIBRARY NAMES kutils
  PATH_SUFFIXES build Debug Release
  PATHS ${KUTILS_POSSIBLE_PATHS}
  )

# if the library object was found, record it.
# may need to add other system files later.
if(KUTILS_LIBRARY)
  set(KUTILS_LIBRARIES  ${KUTILS_LIBRARY})
  message(STATUS "Found kutils library: ${KUTILS_LIBRARY}")
endif(KUTILS_LIBRARY)


if(KUTILS_INCLUDE_DIR) 
  message(STATUS "Found kutils headers: ${KUTILS_INCLUDE_DIR}")
endif(KUTILS_INCLUDE_DIR)

if(KUTILS_LIBRARIES AND KUTILS_INCLUDE_DIR)
  set(KUTILS_FOUND "YES")
endif(KUTILS_LIBRARIES AND KUTILS_INCLUDE_DIR)

# if not found, stop immediately
if(NOT KUTILS_FOUND)
  message(FATAL_ERROR "Could not find both kutils library and headers")
endif(NOT KUTILS_FOUND)		
		
# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
