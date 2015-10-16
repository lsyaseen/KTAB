# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
# CMake comes with 'helper files' for many common libraries (e.g. X11).
# This is an example of how to build one for an uncommon library.
# --------------------------------------------


set(KMODEL_FOUND "NO")

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

set(KMODEL_PREFIX
  ${KTAB_INSTALL_DIR} 
  )

  # Note Well the locations in and order of search
set(KMODEL_POSSIBLE_PATHS
  ${KMODEL_PREFIX}/include/
  ${KMODEL_PREFIX}/lib/
  ${CMAKE_CURRENT_SOURCE_DIR}/../kmodel/
  ${CMAKE_CURRENT_SOURCE_DIR}/../build/kmodel/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../kmodel/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../KTAB/kmodel/ 
  )

# try to find a key header
find_path(KMODEL_INCLUDE_DIR kmodel.h
  PATH_SUFFIXES libsrc
  PATHS ${KMODEL_POSSIBLE_PATHS}
  )

# try to find the compiled library object
find_library(KMODEL_LIBRARY NAMES kmodel
  PATH_SUFFIXES build Debug Release
  PATHS ${KMODEL_POSSIBLE_PATHS}
  )

# if the library object was found, record it.
# may need to add other system files latter.
if(KMODEL_LIBRARY)
  set(KMODEL_LIBRARIES  ${KMODEL_LIBRARY})
endif(KMODEL_LIBRARY)

# if found both library object and that key header,
# record the data and let the user know it succeeded
if(KMODEL_LIBRARIES) 
  message(STATUS "Found kmodel library: ${KMODEL_LIBRARY}") 
endif(KMODEL_LIBRARIES)

if(KMODEL_INCLUDE_DIR)
  message(STATUS "Found kmodel headers: ${KMODEL_INCLUDE_DIR}")
endif(KMODEL_INCLUDE_DIR)

if(KMODEL_LIBRARIES AND KMODEL_INCLUDE_DIR)
  set(KMODEL_FOUND "YES")
endif(KMODEL_LIBRARIES AND KMODEL_INCLUDE_DIR)

# if not found, stop immediately
if(NOT KMODEL_FOUND)
  message(FATAL_ERROR "Could not find both kmodel library and headers")
endif(NOT KMODEL_FOUND)		
		
# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
