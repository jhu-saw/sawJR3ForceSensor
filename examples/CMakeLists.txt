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


# create a list of libraries needed for this project
set (REQUIRED_CISST_LIBRARIES cisstCommon 
                              cisstOSAbstraction
                              cisstMultiTask)

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # sawJR3ForceSensor has been compiled within cisst, we should find it automatically
  find_package (sawJR3ForceSensor REQUIRED HINTS "${cisst_saw_BINARY_DIR}/saw/components")

  if (sawJR3ForceSensor_FOUND)

    # sawJR3ForceSensor configuration
    include_directories (${sawJR3ForceSensor_INCLUDE_DIR})
    link_directories (${sawJR3ForceSensor_LIBRARY_DIR})

    add_executable (sawJR3ForceSensorExample main.cpp)

    # link against non cisst libraries and cisst components
    target_link_libraries (sawJR3ForceSensorExample sawJR3ForceSensor ${sawJR3ForceSensor_LIBRARIES})

    # link against cisst libraries (and dependencies)
    cisst_target_link_libraries (sawJR3ForceSensorExample ${REQUIRED_CISST_LIBRARIES})

  endif (sawJR3ForceSensor_FOUND)
else (cisst_FOUND_AS_REQUIRED)
  cisst_information_message_missing_libraries (${REQUIRED_CISST_LIBRARIES})
endif (cisst_FOUND_AS_REQUIRED)
