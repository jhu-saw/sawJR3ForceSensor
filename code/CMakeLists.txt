#
# $Id: CMakeLists.txt 3181 2011-11-15 15:41:28Z adeguet1 $
#
# Author(s):  Min Yang Jung
# Created on: 2012-02-22
#
# (C) Copyright 2012 Johns Hopkins University (JHU), All Rights
# Reserved.
#
# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

set (REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstVector
                              cisstOSAbstraction
                              cisstMultiTask)

find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES})

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # add definition for code
  set (JR3_COMMON_FILES ../sawJR3ForceSensorExport.h ../mtsJR3ForceSensor.h mtsJR3ForceSensor.cpp)
  if (IS_QNX)
    add_library (sawJR3ForceSensor ${IS_SHARED} ${JR3_COMMON_FILES}
                                   PCI/JR3PCI_QNX.cpp PCI/JR3PCI_QNX.h)
  elseif (CISST_HAS_LINUX_XENOMAI)
    add_library (sawJR3ForceSensor ${IS_SHARED} ${JR3_COMMON_FILES}
                                   PCI/JR3PCI_Xenomai.cpp PCI/JR3PCI_Xenomai.h)
  endif (IS_QNX)

  target_link_libraries (sawJR3ForceSensor ${COMEDI_LIBRARY})

  set (sawJR3ForceSensor_INCLUDE_DIR ${sawComponents_SOURCE_DIR} ${COMEDI_INCLUDE_DIR})
  set (sawJR3ForceSensor_LIBRARY_DIR ${LIBRARY_OUTPUT_PATH} ${COMEDI_LIBRARY_DIR})
  set (sawJR3ForceSensor_LIBRARIES sawJR3ForceSensor comedi)#${COMEDI_LIBRARY})

  configure_file ("${CMAKE_CURRENT_SOURCE_DIR}/sawJR3ForceSensorConfig.cmake.in"
                  "${sawComponents_BINARY_DIR}/sawJR3ForceSensorConfig.cmake"
                   @ONLY@)

  cisst_target_link_libraries (sawJR3ForceSensor ${REQUIRED_CISST_LIBRARIES})

else (cisst_FOUND_AS_REQUIRED)
  message ("Code in ${CMAKE_CURRENT_SOURCE_DIR} will not be compiled since it requires ${REQUIRED_CISST_LIBRARIES}")
endif (cisst_FOUND_AS_REQUIRED)
