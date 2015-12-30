# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
# CMake comes with 'helper files' for many common libraries (e.g. X11).
# This is an example of how to build one for an uncommon library.
# --------------------------------------------

set(KGRAPH_FOUND "NO")

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

set(KGRAPH_PREFIX
  ${KTAB_INSTALL_DIR} 
  )

# Note Well the locations in and order of search
set(KGRAPH_POSSIBLE_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/../kgraph/
  ${CMAKE_CURRENT_SOURCE_DIR}/../build/kgraph/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../kgraph/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../KTAB/kgraph/
  ${KGRAPH_PREFIX}/include/
  ${KGRAPH_PREFIX}/lib/
  )

# try to find a key header
find_path(KGRAPH_INCLUDE_DIR kgraph.h
  PATH_SUFFIXES libsrc
  PATHS ${KGRAPH_POSSIBLE_PATHS}
  )

# try to find the compiled library object
find_library (KGRAPH_LIBRARY NAMES kgraph
  PATH_SUFFIXES build Debug Release
  PATHS ${KGRAPH_POSSIBLE_PATHS}
  )

# if the library object was found, record it.
# may need to add other system files later.
if(KGRAPH_LIBRARY)
  set(KGRAPH_LIBRARIES  ${KGRAPH_LIBRARY})
  message(STATUS "Found kgraph library: ${KGRAPH_LIBRARY}")
endif(KGRAPH_LIBRARY)


if(KGRAPH_INCLUDE_DIR) 
  message(STATUS "Found kgraph headers: ${KGRAPH_INCLUDE_DIR}")
endif(KGRAPH_INCLUDE_DIR)

if(KGRAPH_LIBRARIES AND KGRAPH_INCLUDE_DIR)
  set(KGRAPH_FOUND "YES")
endif(KGRAPH_LIBRARIES AND KGRAPH_INCLUDE_DIR)

# if not found, stop immediately
if(NOT KGRAPH_FOUND)
  message(FATAL_ERROR "Could not find both kgraph library and headers")
endif(NOT KGRAPH_FOUND)		
		
# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
