# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
# CMake comes with 'helper files' for many common
# libraries (e.g. X11). This is an example of how
# to build one for an uncommon library.
# --------------------------------------------


set(EFENCE_FOUND "NO")

set(EFENCE_PREFIX
  /usr/lib/
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

# if found the library, record the data and let the user know
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
