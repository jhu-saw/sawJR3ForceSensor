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

set( REQUIRED_CISST_LIBRARIES cisstCommon
                              cisstVector
                              cisstOSAbstraction
                              cisstParameterTypes
                              cisstMultiTask
                              cisstRobot )

find_package( cisst REQUIRED ${REQUIRED_CISST_LIBRARIES} )

if( cisst_FOUND_AS_REQUIRED )

  # load cisst configuration
  include( ${CISST_USE_FILE} )
  
  set( sawJR3ForceSensor_INCLUDE_DIR 
    "${sawJR3ForceSensor_SOURCE_DIR}/include" )
  set( sawJR3ForceSensor_LIBRARY_DIR 
    "${LIBRARY_OUTPUT_PATH}")

  set( 
    HEADER_FILES
    ${sawJR3ForceSensor_INCLUDE_DIR}/sawJR3ForceSensor/osaJR3ForceSensor.h
    ${sawJR3ForceSensor_INCLUDE_DIR}/sawJR3ForceSensor/mtsJR3ForceSensor.h )
  set( SOURCE_FILES osaJR3ForceSensor.cpp mtsJR3ForceSensor.cpp )
  
  add_library( 
    sawJR3ForceSensor 
    ${IS_SHARED} 
    ${HEADER_FILES} 
    ${SOURCE_FILES} )
  set_property( 
    TARGET 
    sawJR3ForceSensor 
    PROPERTY FOLDER 
    "sawJR3ForceSensor" )

  if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
    
    find_package( COMEDI REQUIRED )
    if( COMEDI_FOUND )
      
      set(
        sawJR3ForceSensor_INCLUDE_DIRS 
        ${sawJR3ForceSensor_INCLUDE_DIR} 
        ${COMEDI_INCLUDE_DIR} )
      
      set( sawJR3ForceSensor_LIBRARIES sawJR3ForceSensor ${COMEDI_LIBRARY} )
      target_link_libraries( sawJR3ForceSensor ${COMEDI_LIBRARY} )
      
    endif( COMEDI_FOUND )

  endif( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )

  include_directories( ${sawJR3ForceSensor_INCLUDE_DIRS} )
      
  configure_file( 
    "${CMAKE_CURRENT_SOURCE_DIR}/sawJR3ForceSensorConfig.cmake.in"
    "${sawJR3ForceSensor_BINARY_DIR}/sawJR3ForceSensorConfig.cmake"
    @ONLY@ )

  cisst_target_link_libraries( sawJR3ForceSensor ${REQUIRED_CISST_LIBRARIES} )

endif (cisst_FOUND_AS_REQUIRED)
