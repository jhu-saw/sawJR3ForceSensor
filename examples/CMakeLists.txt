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
set( REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstOSAbstraction
                              cisstMultiTask )

# find cisst and make sure the required libraries have been compiled
find_package (cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} QUIET)

if (cisst_FOUND_AS_REQUIRED)

  # load cisst configuration
  include (${CISST_USE_FILE})

  # sawJR3ForceSensor has been compiled within cisst, we should find it automatically
  cisst_find_saw_component( sawJR3ForceSensor REQUIRED )
  
  if( sawJR3ForceSensor_FOUND )
    
    # sawJR3ForceSensor configuration
    include_directories( ${sawJR3ForceSensor_INCLUDE_DIR} )
    link_directories( ${sawJR3ForceSensor_LIBRARY_DIR} )
    
    add_executable( osaJR3ForceSensorExample osaJR3ForceSensorExample.cpp )
    
    # link against non cisst libraries and cisst components
    target_link_libraries( 
      osaJR3ForceSensorExample 
      sawJR3ForceSensor 
      ${sawJR3ForceSensor_LIBRARIES} )

    # link against cisst libraries (and dependencies)
    cisst_target_link_libraries( 
      osaJR3ForceSensorExample 
      ${REQUIRED_CISST_LIBRARIES} )

    cisst_find_saw_component( sawBarrett )
    cisst_find_saw_component( sawCANBus  )
    cisst_find_saw_component( sawControllers )
    cisst_find_saw_component( sawKeyboard )
    if( sawBarrett_FOUND AND sawControllers_FOUND AND sawKeyboard_FOUND )

      include_directories(
	${sawBarrett_INCLUDE_DIR}
	${sawCANBus_INCLUDE_DIR}
	${sawControllers_INCLUDE_DIR}
	${sawKeyboard_INCLUDE_DIR} )

      link_directories( 
	${sawBarrett_LIBRARY_DIR} 
	${sawCANBus_LIBRARY_DIR} 
	${sawControllers_LIBRARY_DIR} 
	${sawKeyboard_LIBRARY_DIR} )

      add_executable( 
	mtsJR3ForceSensorWAMExample 
	mtsJR3ForceSensorWAMExample.cpp )
      target_link_libraries( mtsJR3ForceSensorWAMExample 
	                     sawJR3ForceSensor
			     sawBarrett
			     sawCANBus
			     sawControllers
			     sawKeyboard )

      cisst_target_link_libraries( mtsJR3ForceSensorWAMExample
                                   ${REQUIRED_CISST_LIBRARIES} )
    
    endif( sawBarrett_FOUND AND sawControllers_FOUND AND sawKeyboard_FOUND )

  endif( sawJR3ForceSensor_FOUND )

endif( cisst_FOUND_AS_REQUIRED )
