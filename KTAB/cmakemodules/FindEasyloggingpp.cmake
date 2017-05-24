# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------

set(LOGGER_FOUND "NO")

if(WIN32)
  set(LOGGER_INSTALL_DIR C:/local/easyloggingpp)
elseif(UNIX)
  set(LOGGER_INSTALL_DIR /usr/local)
endif(WIN32)

UNSET(LOGGER_INCLUDE_DIR CACHE)
find_path(LOGGER_INCLUDE_DIR easylogging++.h
PATHS ${LOGGER_INSTALL_DIR}
  PATH_SUFFIXES include
)

if(NOT LOGGER_INCLUDE_DIR)
  message(SEND_ERROR "Not Found easylogging++.h at ${LOGGER_INSTALL_DIR}/include")
else()
  message(STATUS "Found easylogging++.h at ${LOGGER_INCLUDE_DIR}")
endif()

UNSET(LOGGER_LIBRARY CACHE)
# try to find the compiled library object
find_library (LOGGER_LIBRARY name easyloggingpp
  PATHS ${LOGGER_INSTALL_DIR}
  PATH_SUFFIXES lib
  )

if(LOGGER_LIBRARY)
  set(LOGGER_FOUND "YES")
  message(STATUS "Found easyloggingpp library: ${LOGGER_LIBRARY}")
else(LOGGER_LIBRARY)
  message(SEND_ERROR "Could not find easyloggingpp library")
endif(LOGGER_LIBRARY)

set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS}    -DELPP_NO_DEFAULT_LOG_FILE    -DELPP_THREAD_SAFE    -DELPP_FORCE_USE_STD_THREAD    -DELPP_UTC_DATETIME    -DELPP_DEFAULT_LOGGING_FLAGS=8192" #logging flag Autospacing=8192
  )

# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
