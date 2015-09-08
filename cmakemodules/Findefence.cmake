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
# CMake comes with 'helper files' for many common
# libraries (e.g. X11). This is an example of how
# to build one for an uncommon library.
# -------------------------------------------------

set(EFENCE_FOUND "NO")

set(EFENCE_PREFIX
  "/usr/lib/"
  CACHE STRING
  "Electric Fence is installed in this prefix (if non-standard)"
  )

  # Note Well the locations in and order of search
  # The only one I now for efence is the Unix standard
set(EFENCE_POSSIBLE_PATHS
  /usr/lib
  )

# try to find the compiled library object
find_library(EFENCE_LIBRARY NAMES efence
  PATHS ${EFENCE_POSSIBLE_PATHS}
  )

# if the library object was found, record it.
# may need to add other system files latter.
if(EFENCE_LIBRARY)
  set(EFENCE_LIBRARIES  ${EFENCE_LIBRARY})
endif(EFENCE_LIBRARY)

# if found both library object
# record the data and let the user know it succeeded
if(EFENCE_LIBRARIES)
  set(EFENCE_FOUND "YES")
  message(STATUS "Found Electric Fence library: ${EFENCE_LIBRARY}")
endif(EFENCE_LIBRARIES)

# if not found, stop immediately
if(NOT EFENCE_FOUND)
  message(FATAL_ERROR "Could not find Electric Fence library")
endif(NOT EFENCE_FOUND)		
		
# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
