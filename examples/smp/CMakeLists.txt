# ===========================================
# Copyright KAPSARC. Open Source MIT License 
# ===========================================


project(smp)

cmake_minimum_required (VERSION 2.8)
cmake_policy(SET CMP0017 NEW) # prefer built-in modules

set (LOCAL_MODULE_DIR
  ${PROJECT_SOURCE_DIR}/../../KTAB/cmakemodules)

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${LOCAL_MODULE_DIR}
  )


set(CMAKE_BUILD_TYPE Debug)

if (COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 OLD)
endif (COMMAND CMAKE_POLICY)

set(LIBRARY_OUTPUT_PATH      ${PROJECT_SOURCE_DIR}/)
set(EXECUTABLE_OUTPUT_PATH   ${PROJECT_SOURCE_DIR}/)


# -------------------------------------------------
find_package(Sqlite)
if (NOT SQLITE_FOUND)
  message(FATAL_ERROR "Could not find SQLite")
endif (NOT SQLITE_FOUND)

# --------------------------------------------------------
# See this page for useful hints: https://cmake.org/Wiki/CMakeForFLTK

set (SMP_XLIBS  )

if(UNIX)
  find_package(X11)
  if (NOT X11_FOUND)
    message (FATAL_ERROR "Couldn't find X11: required for building smpg")
  endif (NOT X11_FOUND)
  set (SMP_XLIBS
    #           Xaw Xmu Xpm Xt Xft X11 Xinerama dl fontconfig
    Xext Xft fontconfig Xinerama dl m X11
    )
endif (UNIX)


include (${LOCAL_MODULE_DIR}/FLTKHelper.cmake)
find_package(FLTK REQUIRED)
if (NOT FLTK_FOUND)
  message (FATAL_ERROR "Could not find fltk: required for building smpg")
endif (NOT FLTK_FOUND)

set (FLUID_DIR
  fluid)

# --------------------------------------------------------
# See "Findkutils.cmake" in cmakemodules to figure
# out where it looks and what variables it sets

find_package(kutils)
if(NOT KUTILS_FOUND)
  message(FATAL_ERROR "Could not find kutils")
endif(NOT KUTILS_FOUND)


# See "Findkmodel.cmake" in cmakemodules to figure
# out where it looks and what variables it sets

find_package(kmodel)
if(NOT KMODEL_FOUND)
  message(FATAL_ERROR "Could not find kmodel")
endif(NOT KMODEL_FOUND)

# -------------------------------------------------
# files to be built into a library go in libsrc/
# any application or testing code goes in src

set (CSVPARSER_DIR
  ${PROJECT_SOURCE_DIR}/../../KTAB/kmodel/csvparser/
  )

include_directories (
  ${KUTILS_INCLUDE_DIR}
  ${KMODEL_INCLUDE_DIR}
  ${PROJECT_SOURCE_DIR}/libsrc/
  ${PROJECT_SOURCE_DIR}/src/
  ${FLUID_DIR}
  ${FLTK_INCLUDE_DIR}
  ${CSVPARSER_DIR}
  ${SQLITE_INCLUDE_DIR}
  )


set(SMPLIB_SRCS
  ${PROJECT_SOURCE_DIR}/libsrc/smp.cpp
  )

add_library(smp STATIC ${SMPLIB_SRCS})

# -------------------------------------------------
# define a little library of smp GUI stuff

set (SMPGUI_FLS
  ${FLUID_DIR}/mainw.fl
  )

message (STATUS "Use these SMPGUI_FLS: ${SMPGUI_FLS}")

fltk_wrap_ui (smpGUI ${SMPGUI_FLS})
add_library (smpGUI STATIC ${smpGUI_FLTK_UI_SRCS})

# --------------------------------------------------------

set (SMPG_SRCS
  ${PROJECT_SOURCE_DIR}/src/guimain.cpp
  )

set (SMPG_LIBS
  ${FLTK_LIBRARIES}
  smpGUI
  ${SMP_XLIBS}
  )

add_executable (smpg ${SMPG_SRCS})
target_link_libraries(smpg ${SMPG_LIBS})


# -------------------------------------------------

add_executable (smpc
  src/demosmp.cpp
  ${CSVPARSER_DIR}/csv_parser.cpp
  )


target_link_libraries (smpc
  smp
  ${KMODEL_LIBRARY}
  ${KUTILS_LIBRARY}
  #  ${EFENCE_LIBRARIES}
  )

# -------------------------------------------------
# show some useful status/debugging information
message (STATUS "Using PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})
message (STATUS "KUTILS_INCLUDE_DIR: " ${KUTILS_INCLUDE_DIR})
message (STATUS "KMODEL_INCLUDE_DIR: " ${KMODEL_INCLUDE_DIR})

# -------------------------------------------------
# As of early 2014, C++11 is still not the default
# for g++, so I have to provide it here.
# Also on Unix, the C++11 thread library relies on pthreads
if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++11 ")
endif(UNIX)

# ===========================================
# Copyright KAPSARC. Open Source MIT License 
# ===========================================
