# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
# The MIT License (MIT)
# 
# Copyright (c) 2015 King Abdullah Petroleum Studies and Research Center
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software
# and associated documentation files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom 
# the Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
# BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
# -------------------------------------------------
# CMake comes with 'helper files' for many common libraries (e.g. X11).
# This is an example of how to build one for an uncommon library.
# -------------------------------------------------

set(KUTILS_FOUND "NO")

set(KUTILS_PREFIX
  "/usr/local/ktab/"
  CACHE STRING
  "kutils is installed in this prefix (if non-standard)"
  )

  # Note Well the locations in and order of search
set(KUTILS_POSSIBLE_PATHS
  ${KUTILS_PREFIX}/include/
  ${KUTILS_PREFIX}/lib/
  ${CMAKE_CURRENT_SOURCE_DIR}/../kutils/
  ${CMAKE_CURRENT_SOURCE_DIR}/../build/kutils/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../kutils/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../KTAB/kutils/
  ${KTAB_PREFIX}
  "C:/local/ktab"
  #${CMAKE_CURRENT_SOURCE_DIR}/../kutils/Debug
  #${CMAKE_CURRENT_SOURCE_DIR}/../kutils/Release
  #${CMAKE_CURRENT_SOURCE_DIR}/../../KTAB/kutils/Debug
  )

# try to find a key header
find_path(KUTILS_INCLUDE_DIR kutils.h
  PATH_SUFFIXES libsrc
  PATHS ${KUTILS_POSSIBLE_PATHS}
  )

# try to find the compiled library object
find_library(KUTILS_LIBRARY NAMES kutils
  PATH_SUFFIXES build Debug Release
  PATHS ${KUTILS_POSSIBLE_PATHS}
  )

# if the library object was found, record it.
# may need to add other system files later.
if(KUTILS_LIBRARY)
  set(KUTILS_LIBRARIES  ${KUTILS_LIBRARY})
endif(KUTILS_LIBRARY)

# if found both library object and that key header,
# record the data and let the user know it succeeded
if(KUTILS_LIBRARIES) 
  message(STATUS "Found kutils library: ${KUTILS_LIBRARY}")
endif(KUTILS_LIBRARIES)

if(KUTILS_INCLUDE_DIR) 
  message(STATUS "Found kutils headers: ${KUTILS_INCLUDE_DIR}")
endif(KUTILS_INCLUDE_DIR)

if(KUTILS_LIBRARIES AND KUTILS_INCLUDE_DIR)
  set(KUTILS_FOUND "YES")
  #message(STATUS "Found both kutils library and headers.") 
endif(KUTILS_LIBRARIES AND KUTILS_INCLUDE_DIR)

# if not found, stop immediately
if(NOT KUTILS_FOUND)
  message(FATAL_ERROR "Could not find both kutils library and headers")
endif(NOT KUTILS_FOUND)		
		
# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
