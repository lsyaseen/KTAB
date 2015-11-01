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
# Build kgraph and FTetris
# -------------------------------------------------


project(kgraph)

cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

set (LOCAL_MODULE_DIR
  ${PROJECT_SOURCE_DIR}/../../KTAB/cmakemodules)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${LOCAL_MODULE_DIR}
  )

set(LIBRARY_OUTPUT_PATH      ${PROJECT_SOURCE_DIR}/)
set(EXECUTABLE_OUTPUT_PATH   ${PROJECT_SOURCE_DIR}/)

# -------------------------------------------------
set (BUILD_тетрис false CACHE  BOOL "Change executable from 'ftetris' to 'тетрис'")

# ADD_EXECUTABLE(тетрис ...) does work, but it
# makes it hard for English-speakers to type
# the name from the Unix command line. 
# To use this with Visual Studio under Windows, the Language Pack
# is needed, and that may require an expensive OS upgrade
# to the Professional, Ultimate, Enterprise, or whatever edition.
# -------------------------------------------------

set (ENABLE_EFENCE false CACHE  BOOL "Use Electric Fence memory debugger")

if (ENABLE_EFENCE)
  find_package(efence)
  if (NOT EFENCE_FOUND)
    message(FATAL_ERROR "Could not find Electric Fence memory debugger")
  endif (NOT EFENCE_FOUND)
endif (ENABLE_EFENCE) 

# -------------------------------------------------
set (KG_XLIBS  )

IF(UNIX)
  FIND_PACKAGE(X11)
  IF(NOT X11_FOUND)
    MESSAGE(FATAL_ERROR "Couldn't find X11: required for building kgraph")
  ENDIF(NOT X11_FOUND)
  set (KG_XLIBS  Xaw Xmu Xpm Xt Xft X11 dl fontconfig)
ENDIF (UNIX)


INCLUDE(${LOCAL_MODULE_DIR}/FLTKHelper.cmake)
FIND_PACKAGE(FLTK REQUIRED)

IF(NOT FLTK_FOUND)
  MESSAGE(FATAL_ERROR "Could not find fltk: required for building ftetris")
ENDIF(NOT FLTK_FOUND)

set (FLUID_DIR
  fluid)

# --------------------------------------------------------
# See "Findkutils.cmake" in cmakemodules to figure
# out where it looks and what variables it sets

find_package(kutils)
if(NOT KUTILS_FOUND)
  message(FATAL_ERROR "Could not find kutils")
endif(NOT KUTILS_FOUND)

# --------------------------------------------------------
INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}/src/
  ${PROJECT_SOURCE_DIR}/libsrc/
  ${KUTILS_INCLUDE_DIR}
  ${FLUID_DIR}
  ${FLTK_INCLUDE_DIR}
  ${X11_INCLUDE_DIR}
  ${EFENCE_INCLUDE_DIR} 
  )

SET(KG_SRCS
  ${PROJECT_SOURCE_DIR}
  libsrc/kgraph.cpp
  )


ADD_LIBRARY(kgraph STATIC ${KG_SRCS})

# -------------------------------------------
# Under Linux, default start of DESTINATION path is /usr/local/ 
# so "ktab/lib" denotes destination /usr/local/ktab/lib
# Under Windows, default start of DESTINATION path is "C:\Program Files\kutils\"
# so "ktab/lib" denotes destination C:\Program Files\kutils\ktab\lib

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

install(
  TARGETS
    kgraph
  DESTINATION 
    ${KTAB_INSTALL_DIR}/lib) 

install(
  FILES
    libsrc/kgraph.h  
  DESTINATION
    ${KTAB_INSTALL_DIR}/include)  

# -------------------------------------------------
# demo programs

FLTK_WRAP_UI(tui fluid/tetrisUI.fl)
ADD_LIBRARY(tui STATIC ${tui_FLTK_UI_SRCS})

# -------------------------------------------------

if (NOT BUILD_тетрис)
  message (STATUS "Configured to build ftetris in " ${PROJECT_SOURCE_DIR})
  ADD_EXECUTABLE(ftetris
    src/tmain.cpp
    src/tcanvas.cpp
    src/pvcanvas.cpp
    src/board.cpp
    src/shape.cpp
    )
  TARGET_LINK_LIBRARIES(ftetris
    kgraph
    tui 
    ${FLTK_LIBRARIES} 
    ${KUTILS_LIBRARY}
    ${KG_XLIBS} 
    ${EFENCE_LIBRARIES}
    )
endif (NOT BUILD_тетрис)


if (BUILD_тетрис)
  message (STATUS "Configured to build тетрис in " ${PROJECT_SOURCE_DIR})

  ADD_EXECUTABLE(тетрис
    src/tmain.cpp
    src/tcanvas.cpp
    src/pvcanvas.cpp
    src/board.cpp
    src/shape.cpp
    )
  TARGET_LINK_LIBRARIES(тетрис
    kgraph
    tui 
    ${FLTK_LIBRARIES} 
    ${KUTILS_LIBRARY}
    ${KG_XLIBS} 
    ${EFENCE_LIBRARIES}
    )
endif (BUILD_тетрис)

# -------------------------------------------------
# show some useful status/debugging information
message (STATUS "Using PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})
message (STATUS "KUTILS_INCLUDE_DIR: " ${KUTILS_INCLUDE_DIR})

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11 ")
endif(UNIX)

# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
