# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------

set(SASQ_FOUND "NO")
#------------------------------------------------------------------
set(SASQ_POSSIBLE_PATHS
  ${CMAKE_CURRENT_SOURCE_DIR}/SASQ/
  ${CMAKE_CURRENT_SOURCE_DIR}/../SASQ/
  ${CMAKE_CURRENT_SOURCE_DIR}/../examples/SASQ/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../SASQ/
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../SASQ/
  )

#------------------------------------------------------------------
# Finding SASQ directory - with mainwindow.h header as refrence
find_path(SASQ_SOURCE_DIR mainwindow.h PATHS ${SASQ_POSSIBLE_PATHS})

if(SASQ_SOURCE_DIR)
  message(STATUS "Found SASQ source directory: ${SASQ_SOURCE_DIR}")

  #manually adding files to the variables
  set (QtSASQProjectLib_src
    ${SASQ_SOURCE_DIR}/mainwindow.cpp
#    ${SASQ_SOURCE_DIR}/csv.cpp
#    ${SASQ_SOURCE_DIR}/database.cpp
#    ${SASQ_SOURCE_DIR}/bargraph.cpp
#    ${SASQ_SOURCE_DIR}/linegraph.cpp
#    ${SASQ_SOURCE_DIR}/runsmp.cpp
#    ${SASQ_SOURCE_DIR}/quadmap.cpp
#    ${SASQ_SOURCE_DIR}/xmlwidget.cpp
#    ${SASQ_SOURCE_DIR}/xmlparser.cpp
#    ${SASQ_SOURCE_DIR}/colorpickerdialog.cpp
#    ${SASQ_SOURCE_DIR}/popupwidget.cpp
)

  set (QtSASQProjectLib_hdr
    ${SASQ_SOURCE_DIR}/mainwindow.h
#    ${SASQ_SOURCE_DIR}/csv.h
#    ${SASQ_SOURCE_DIR}/database.h
#    ${SASQ_SOURCE_DIR}/xmlparser.h
#    ${SASQ_SOURCE_DIR}/colorpickerdialog.h
#    ${SASQ_SOURCE_DIR}/popupwidget.h
)

  set (QtSASQProjectLib_ui  ${SASQ_SOURCE_DIR}/mainwindow.ui)
  set (QtSASQProjectRsc_qrc ${SASQ_SOURCE_DIR}/sasqwindow.qrc)
  set (QtSASQProjectBin_src ${SASQ_SOURCE_DIR}/main.cpp)

endif(SASQ_SOURCE_DIR)

if(NOT SASQ_SOURCE_DIR)
  message(FATAL_ERROR "Could not find SASQ source directory")
endif(NOT SASQ_SOURCE_DIR)


#------------------------------------------------------------------
# Checking availibity of all required resources
if(SASQ_SOURCE_DIR)
  set(SASQ_FOUND "YES")
endif(SASQ_SOURCE_DIR)

# if not found, stop immediately
if(NOT SASQ_FOUND)
  message(FATAL_ERROR "Required Source files are not found for SASQ")
endif(NOT SASQ_FOUND)

# --------------------------------------------
# Copyright KAPSARC. Open source MIT License.
# --------------------------------------------
