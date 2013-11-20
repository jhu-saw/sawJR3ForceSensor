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

# To find FindMicroScribe.cmake

set(
  CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/code/cmake/Modules
  )

project( sawJR3ForceSensor )
add_subdirectory( code )
add_subdirectory( examples )
