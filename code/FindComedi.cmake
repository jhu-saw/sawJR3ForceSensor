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
# COMEDI_INCLUDE_DIR, where to find comedi.h and comedilib.h
# COMEDI_FOUND, If false, do not use sawJR3ForceSensor component..
# COMEDI_LIBRARY, where to find Comedi library (libcomedi)

# For now sawJR3ForceSensor only supports Xenomai (using Comedi) and QNX 
# (direct memory mapped access to DSP memory of JR3).
if (CISST_HAS_LINUX_XENOMAI)
  message(STATUS "Building JR3ForceSensor component for Xenomai")
  find_path (COMEDI_INCLUDE_DIR
             NAMES comedi.h comedilib.h
             PATHS "/usr/local/src/comedi/comedi/include/linux")
  find_library (COMEDI_LIBRARY comedi "/usr/local/lib")
  if (COMEDI_INCLUDE_DIR)
      if (COMEDI_LIBRARY)
        set (COMEDI_FOUND "YES")
        set (COMEDI_LIBRARIES ${COMEDI_LIBRARY})
        mark_as_advanced (COMEDI_LIBRARIES COMEDI_LIBRARY COMEDI_FOUND COMEDI_INCLUDE_DIR)
    endif (COMEDI_LIBRARY)
  endif (COMEDI_INCLUDE_DIR)
elseif (IS_QNX)
  message(STATUS "Building JR3ForceSensor component for QNX")
  # MJ TODO: implement QNX
else (CISST_HAS_LINUX_XENOMAI)
  # other platforms are not supported
  message(STATUS "Skipped building JR3ForceSensor component: Only QNX and Xenomai are supported")
endif (CISST_HAS_LINUX_XENOMAI)
