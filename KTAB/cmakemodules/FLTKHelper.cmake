# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------

# Places to look, under Linux and Windows.
set (LOCAL_PATH
  /usr/
  c:/local/fltk
  )

# try to find a key header
find_path (LOCAL_INC_DIR 
  NAMES FL/Fl.H
  PATH_SUFFIXES include/ 
  PATHS ${LOCAL_PATH})

# try to find a key program
find_program (LOCAL_BINARY
  NAMES fluid fluid.exe
  PATH_SUFFIXES ./ bin/
  PATHS ${LOCAL_PATH}
  NO_SYSTEM_ENVIRONMENT_PATH
  )

# notify of success
if (LOCAL_INC_DIR)
  message (STATUS "Found FLTK include dir: ${LOCAL_INC_DIR}")
  set (FLTK_INCLUDE_DIR ${LOCAL_INC_DIR})
endif (LOCAL_INC_DIR)

# notify of success
if (LOCAL_BINARY)
  message (STATUS "Found fluid executable: ${LOCAL_BINARY}")
  set (FLTK_FLUID_EXECUTABLE ${LOCAL_BINARY})
endif (LOCAL_BINARY)

# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
