/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsJR3ForceSensorPCI_Xenomai.cpp 3181 2011-11-15 15:41:28Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-22

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#ifndef _mtsJR3ForceSensorPCI_h
#define _mtsJR3ForceSensorPCI_h

#include <sawJR3ForceSensor/mtsJR3ForceSensor.h>

#include <comedilib.h>

// Xenomai version of JR3 PCI based on Comedi:
//   comedi: linux control and measurement device interface 
//   http://www.comedi.org
class mtsJR3ForceSensorPCI
{
    comedi_t * PCIDevice;
    comedi_range * Ranges[6];

    int SubdeviceId;

public:
    mtsJR3ForceSensorPCI(int subdeviceId, const std::string & devicePath = "/dev/comedi0");
    ~mtsJR3ForceSensorPCI();

    void Initialize(void);

    mtsJR3ForceSensor::FTReading ReadFilteredFT(int filterId);
    unsigned int ReadErrorCount(void) const;
    unsigned int ReadWarning(void) const;
    unsigned int ReadError(void) const;
};

#endif
