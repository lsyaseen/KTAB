# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
# CMake comes with 'helper files' for many common libraries (e.g. X11).
# This is an example of how to build one for an uncommon library.
# --------------------------------------------

set(SMP_FOUND "NO")

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

set(SMP_PREFIX
  ${KTAB_INSTALL_DIR} 
  )

# Note Well the locations in and order of search
set(SMP_POSSIBLE_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/../smp/
  ${CMAKE_CURRENT_SOURCE_DIR}/../build/smp/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../smp/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../examples/libsrc/smp/
  ${SMP_PREFIX}/include/
  ${SMP_PREFIX}/lib/
  )

# try to find a key header
find_path(SMP_INCLUDE_DIR smp.h
  PATH_SUFFIXES libsrc
  PATHS ${SMP_POSSIBLE_PATHS}
  )

# try to find the compiled library object
find_library (SMP_LIBRARY NAMES smp
  PATH_SUFFIXES build Debug Release
  PATHS ${SMP_POSSIBLE_PATHS}
  )

# if the library object was found, record it.
# may need to add other system files later.
if(SMP_LIBRARY)
  set(SMP_LIBRARIES  ${SMP_LIBRARY})
  message(STATUS "Found smp library: ${SMP_LIBRARY}")
endif(SMP_LIBRARY)


if(SMP_INCLUDE_DIR) 
  message(STATUS "Found smp headers: ${SMP_INCLUDE_DIR}")
endif(SMP_INCLUDE_DIR)

if(SMP_LIBRARIES AND SMP_INCLUDE_DIR)
  set(SMP_FOUND "YES")
endif(SMP_LIBRARIES AND SMP_INCLUDE_DIR)

# if not found, stop immediately
if(NOT SMP_FOUND)
  message(FATAL_ERROR "Could not find both smp library and headers")
endif(NOT SMP_FOUND)		
		
# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
