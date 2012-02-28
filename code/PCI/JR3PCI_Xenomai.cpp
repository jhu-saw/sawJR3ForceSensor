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

#include <cisstCommon/cmnLogger.h>
#include <cisstCommon/cmnThrow.h>

#include <sawJR3ForceSensor/code/PCI/JR3PCI_Xenomai.h>

#include <comedi.h>
#include <comedilib.h>
#include <errno.h>
#include <string.h>
#include <iomanip>
#include <math.h>

mtsJR3ForceSensorPCI::mtsJR3ForceSensorPCI(int subdeviceId, const std::string & devicePath) 
    : SubdeviceId(subdeviceId)
{
    PCIDevice = comedi_open(devicePath.c_str());
    if (!PCIDevice) {
        std::stringstream ss;
        ss << "mtsJR3ForceSensorPCI: Failed to open JR3 comedi device \"" << devicePath << "\"" << std::endl;
        CMN_LOG_INIT_ERROR << ss;
        cmnThrow(std::string(ss.str()));
    }

    CMN_LOG_INIT_VERBOSE << "mtsJR3ForceSensorPCI: JR3 comedi device open success" << std::endl;
}

mtsJR3ForceSensorPCI::~mtsJR3ForceSensorPCI()
{
    if (PCIDevice) {
        if (comedi_close(PCIDevice) == -1)
            CMN_LOG_RUN_ERROR << "mtsJR3ForceSensorPCI: Failed to close JR3" << std::endl;
    }
    CMN_LOG_INIT_VERBOSE << "mtsJR3ForceSensorPCI: JR3 comdei device closed" << std::endl;
}

void mtsJR3ForceSensorPCI::Initialize(void)
{
    // Get range information of each channel (1~6)
    for (int i = 0; i < 6; ++i) {
        Ranges[i] = comedi_get_range(PCIDevice, SubdeviceId, i, 0);
        if (Ranges[i] == 0) {
            std::stringstream ss;
            ss << "mtsJR3ForceSensorPCI: Initialize: Failed to get range information for channel " << i << std::endl;
            CMN_LOG_INIT_ERROR << ss;
            cmnThrow(std::string(ss.str()));
        }
    }
}

mtsJR3ForceSensor::FTReading mtsJR3ForceSensorPCI::ReadFilteredFT(int filterId)
{
    mtsJR3ForceSensor::FTReading ft;
    ft.SetAll(0.0);
    if (filterId < 0 || filterId > 6) {
        return ft;
    }

    static int channelId = 0;
    static int range = 0;
    static int aref = AREF_GROUND;

    lsampl_t sample;
    for (int i = 0; i < 6; ++i) {
        sample = 0;
        channelId = filterId * 8 + i;
        if (-1 == comedi_data_read(PCIDevice, 
                                   SubdeviceId, 
                                   channelId, 
                                   range, 
                                   aref, 
                                   &sample))
        {
            sample = 0;
            CMN_LOG_RUN_ERROR << "mtsJR3ForceSensorPCI: ReadFilteredFT: Failed to read sample from mtsJR3ForceSensorPCI: " 
                << SubdeviceId << ", " << channelId << ", " << range << ", " << aref << std::endl;
        } else {
            ft[i] = ((double) sample) / 16384.0 * (fabs(Ranges[i]->min));
            /*
            double a = comedi_to_phys(sample, Ranges[i], 65536);
            if (NAN == a)
                CMN_LOG_RUN_ERROR << "mtsJR3ForceSensorPCI: ReadFilteredFT: Failed to convert to physical reading" << std::endl;
            else
                std::cout << sample << "\t" << ft[i] << "\t" << a << std::endl;
            */
        }

    }
    return ft;
}

void mtsJR3ForceSensorPCI::GetError(unsigned int & errCode) const
{
    // MJ: not implemented yet
    errCode = 0;
}
