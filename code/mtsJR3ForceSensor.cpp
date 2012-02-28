/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsJR3ForceSensor.cpp 3181 2011-11-15 15:41:28Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-22

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include <cisstCommon/cmnLogger.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>

#include <sawJR3ForceSensor/mtsJR3ForceSensor.h>
#if (CISST_OS == CISST_QNX)
//#include <sawJR3/mtsJR3ForceSensorDSP.h>
#elif (CISST_OS == CISST_LINUX_XENOMAI)
#include <sawJR3ForceSensor/code/PCI/JR3PCI_Xenomai.h>
#endif

CMN_IMPLEMENT_SERVICES_DERIVED_ONEARG(mtsJR3ForceSensor, mtsComponent, std::string)

// Name of standardized interface
const std::string mtsJR3ForceSensor::JR3InterfaceName = "JR3Interface";
// Name of standardized commands
const std::string mtsJR3ForceSensor::JR3CommandNames::ReadRawFT = "ReadRawFT";
const std::string mtsJR3ForceSensor::JR3CommandNames::ReadFilteredFT = "ReadFilteredFT";
const std::string mtsJR3ForceSensor::JR3CommandNames::GetError = "GetError";

mtsJR3ForceSensor::mtsJR3ForceSensor(void) : mtsComponent("NONAME"), PCI(0)
{
    // Default constructor shouldn't be used
    CMN_LOG_INIT_WARNING << "mtsJR3ForceSensor: PCIDevice is not initialized and unavailable" << std::endl;
}

mtsJR3ForceSensor::mtsJR3ForceSensor(const std::string &name) : mtsComponent(name), PCI(0)
{
    // Default constructor shouldn't be used
    CMN_LOG_INIT_WARNING << "mtsJR3ForceSensor: PCIDevice is not initialized and unavailable" << std::endl;
}

#if (CISST_OS == CISST_QNX)
mtsJR3ForceSensor::mtsJR3ForceSensor(const std::string &name, const PCI_ID deviceId) 
#elif (CISST_OS == CISST_LINUX_XENOMAI)
mtsJR3ForceSensor::mtsJR3ForceSensor(const std::string &name, const std::string & devicePath, int subdeviceId)
#endif
    : mtsComponent(name)
{
#if (CISST_OS == CISST_QNX)
    PCI = new mtsJR3ForceSensorPCI(deviceId);
#elif (CISST_OS == CISST_LINUX_XENOMAI)
    PCI = new mtsJR3ForceSensorPCI(subdeviceId, devicePath);
#endif

    PCI->Initialize();

    mtsInterfaceProvided * provided = AddInterfaceProvided(mtsJR3ForceSensor::JR3InterfaceName);
    if (provided) {
        // sampling rate of 8kHz with different cut-off frequency
        provided->AddCommandRead(&mtsJR3ForceSensor::ReadRawFT, this, mtsJR3ForceSensor::JR3CommandNames::ReadRawFT);
        provided->AddCommandQualifiedRead(&mtsJR3ForceSensor::ReadFilteredFT, this, mtsJR3ForceSensor::JR3CommandNames::ReadFilteredFT);
        provided->AddCommandRead(&mtsJR3ForceSensor::GetError, this, mtsJR3ForceSensor::JR3CommandNames::GetError);
    }
}

mtsJR3ForceSensor::~mtsJR3ForceSensor()
{
    if (PCI)
        delete PCI;
}

void mtsJR3ForceSensor::ReadRawFT(FTReading & ft) const 
{
    ft = PCI->ReadFilteredFT(0);
}

void mtsJR3ForceSensor::ReadFilteredFT(const int & filterId, FTReading & ft) const 
{
    ft = PCI->ReadFilteredFT(filterId);
}

void mtsJR3ForceSensor::GetError(unsigned int & errCode) const
{
    PCI->GetError(errCode);
}
