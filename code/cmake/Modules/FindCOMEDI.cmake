#
# $Id: FindComedi.cmake 3181 2011-11-15 15:41:28Z adeguet1 $
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
#
# Find Comedi Comedi header and library files
#
# This module defines
# COMEDI_FOUND, If false, do not use sawJR3 component..
# COMEDI_INCLUDE_DIR, where to find comedi.h and comedilib.h
# COMEDI_LIBRARY, Comedi library (libcomedi)
# COMEDI_LIBRARY_DIR, where to find Comedi library (e.g. /usr/local/lib) 

# For now sawJR3 only supports Xenomai (using Comedi) and QNX 
# (direct memory mapped access to DSP memory of JR3).

if( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )

  message(STATUS "Building JR3 component for Linux")
  find_path (COMEDI_INCLUDE_DIR
    NAMES comedi.h comedilib.h
    PATHS "/usr/local/src/comedi/comedi/include/linux")
  find_library( COMEDI_LIBRARY comedi )

  if (COMEDI_INCLUDE_DIR)
    if (COMEDI_LIBRARY)
      set (COMEDI_LIBRARY_DIR "/usr/local/lib")
      set (COMEDI_FOUND "YES")      
      mark_as_advanced (COMEDI_LIBRARY COMEDI_FOUND COMEDI_INCLUDE_DIR)
    endif (COMEDI_LIBRARY)
  endif (COMEDI_INCLUDE_DIR)

elseif (IS_QNX)

  message(STATUS "Building JR3 component for QNX")
  # MJ TODO: implement QNX

else( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )

  # other platforms are not supported
  message( STATUS "Skipped building JR3 component: Only QNX and Xenomai are supported")

endif( ${CMAKE_SYSTEM_NAME} MATCHES "Linux" )
