# -----------------------------------------------
#   Copyright Ben Paul Wise. All Rights Reserved.
# -----------------------------------------------
#
# This tries to find ZLIB, before FIND_PACKAGE(ZLIB).
#
# If you build zlib from source under Windows, you need to 
# move the zconf.h file into C:/local/zlib or its equivalent
#
# -----------------------------------------------


SET(CK_ZLIB_EXTRA_SEARCH_PATH
  /usr/local/
  /usr/local/zlib/
  /usr/local/include/zlib/
  /share/zlib/
  /local/
  /local/lib/
  /usr/lib/
  /usr/include/
  C:/local/zlib
  C:/usr/local/zlib
  C:/usr/local/lib/zlib
  C:/usr/local/include/zlib
  /usr/lib/x86_64-linux-gnu/
  )


# try to find the include directory
FIND_PATH(CK_POSSIBLE_ZLIB_INCLUDE_DIR
  NAMES zlib.h libz.h
  PATH_SUFFIXES include/ ./ lib/ bin/ build/Release/ build/Debug/
  PATHS ${CK_ZLIB_EXTRA_SEARCH_PATH})


IF(CK_POSSIBLE_ZLIB_INCLUDE_DIR)
  MESSAGE(STATUS "Seeding ZLIB include variable with ${CK_POSSIBLE_ZLIB_INCLUDE_DIR}")
  SET(ZLIB_INCLUDE_DIR ${CK_POSSIBLE_ZLIB_INCLUDE_DIR}) 
ENDIF(CK_POSSIBLE_ZLIB_INCLUDE_DIR)


# try to find the library
FIND_LIBRARY(ZLIB_LIBRARY
  NAMES libz zlib zlibd zlib.lib zlibd.lib libz.a libz.so 
  PATH_SUFFIXES  lib/ bin/ build/Debug build/Release
  PATHS ${CK_ZLIB_EXTRA_SEARCH_PATH}
  )


IF (NOT ZLIB_LIBRARY)
  MESSAGE(FATAL_ERROR "Could not find zlib")
ELSE ()
  MESSAGE(STATUS "Found ${ZLIB_LIBRARY}")
ENDIF (NOT ZLIB_LIBRARY)


# -----------------------------------------------
#   Copyright Ben Paul Wise. All Rights Reserved.
# -----------------------------------------------
