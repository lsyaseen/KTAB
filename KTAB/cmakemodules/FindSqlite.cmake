# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
# CMake comes with 'helper files' for many common
# libraries (e.g. X11). This is an example of how
# to build one for an uncommon library.
# --------------------------------------------

set(SQLITE_FOUND "NO")

set(SQLITE_PREFIX
  /usr/local/sqlite
  CACHE STRING
  "SQLite is installed in this prefix (if non-standard)"
  )

  # Note Well the locations in and order of search
  # The Unix standard is OK, but Windows is still tricky.
set(SQLITE_POSSIBLE_PATHS
  /usr/local
  /usr/local/sqlite
  /usr/lib
  /usr/lib/x86_64-linux-gnu  # odd path for Debian
  /local/sqlite
  c:/local/sqlite
  )

# try to find the compiled library object
find_library(SQLITE_LIBRARY NAMES sqlite3
  NAMES sqlite3.a  sqlite3.lib
  PATHS ${SQLITE_POSSIBLE_PATHS}
  )

# if the library object was found, record it.
if(SQLITE_LIBRARY)
  set(SQLITE_LIBRARIES  ${SQLITE_LIBRARY})
endif(SQLITE_LIBRARY)

find_path(SQLITE_INCLUDE_DIR sqlite3.h
  PATHS
  PATH_SUFFIXES ./ src/
  ${SQLITE_POSSIBLE_PATHS}
)

# if found both library object and directories,
# record the data and let the user know it succeeded
if(SQLITE_LIBRARIES AND SQLITE_INCLUDE_DIR)
  set(SQLITE_FOUND "YES")
  message(STATUS "Found SQLite library: ${SQLITE_LIBRARY}")
  message(STATUS "Found SQLite header:  ${SQLITE_INCLUDE_DIR}")
endif(SQLITE_LIBRARIES AND SQLITE_INCLUDE_DIR)

# if not found, stop immediately
if(NOT SQLITE_FOUND)
  message(FATAL_ERROR "Could not find SQLite library or header")
endif(NOT SQLITE_FOUND)		
		
# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
