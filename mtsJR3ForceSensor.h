/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsJR3ForceSensor.h 3181 2011-11-15 15:41:28Z adeguet1 $

  Author(s):  Min Yang Jung
  Created on: 2012-02-22

  (C) Copyright 2012 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

/*!
  \file
  \brief cisst Component for JR3 Force & Torque Sensor
*/

#ifndef _mtsJR3ForceSensor_h
#define _mtsJR3ForceSensor_h

#include <cisstMultiTask/mtsComponent.h>
#include <cisstMultiTask/mtsFixedSizeVector.h>

#include <sawJR3ForceSensor/sawJR3ForceSensorExport.h>

class mtsJR3ForceSensorPCI;

class CISST_EXPORT mtsJR3ForceSensor : public mtsComponent
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:
    // from comedi/jr3_pci.h
   typedef enum PCI_IDs {
       PCI_VENDOR_ID_JR3               = 0x1762,
       PCI_DEVICE_ID_JR3_1_CHANNEL     = 0x3111,
       PCI_DEVICE_ID_JR3_1_CHANNEL_NEW = 0x1111,
       PCI_DEVICE_ID_JR3_2_CHANNEL     = 0x3112,
       PCI_DEVICE_ID_JR3_3_CHANNEL     = 0x3113,
       PCI_DEVICE_ID_JR3_4_CHANNEL     = 0x3114
   } PCI_ID;

    // Name of standardized interface
    const static std::string JR3InterfaceName;
    // Name of standardized commands
    class CISST_EXPORT JR3CommandNames {
    public:
		const static std::string ReadRawFT;
        const static std::string ReadFilteredFT;
        const static std::string ReadErrorCount;
        const static std::string ReadWarning;
        const static std::string ReadError;
    };

	// Typedef for force and torque readings
    typedef vctFixedSizeVector<double, 6> FTReading;

protected:
    mtsJR3ForceSensorPCI * PCI;

    /*! Read unfiltered (raw) data */
    void ReadRawFT(FTReading & ft) const;

    /*! Read filtered FT with sampling rate of 8 kHz and cutoff frequency of
    
         id   cut-off frequency
       -------------------------
          0   unfiltered (raw) data
          1   500 Hz
          2   125 Hz
          3   31.25 Hz
          4   7.813 Hz
          5   1.953 Hz
          6   0.4883 Hz
     
       (from comedi jr3_pci documentation)
       Filter0 is the decoupled, unfiltered data from the JR3 sensor.
       This data has had the offsets removed. The decoupled data is
       passed through cascaded low pass filters. Each succeeding filter
       has a cutoff frequency of 1/4 of the preceding filter. The cutoff
       frequency of filter1 is 1/16 of the sample rate from the sensor.
       For a typical sensor with a sample rate of 8 kHz, the cutoff
       frequency of filter1 would be 500 Hz. The following filters
       cutoff at 125 Hz, 31.25 Hz, 7.813 Hz, 1.953 Hz and 0.4883 Hz. */
    void ReadFilteredFT(const int & filterId, FTReading & ft) const;

    /*! Get error count, warning, and error from JR3 */
    void ReadErrorCount(unsigned int & errorCount) const;
    void ReadWarning(unsigned int & warning) const;
    void ReadError(unsigned int & error) const;

public:
    /*! Constructors and destructor */
    mtsJR3ForceSensor(void);
    mtsJR3ForceSensor(const std::string &name);
#if (CISST_OS == CISST_QNX)
    mtsJR3ForceSensor(const std::string &name, const PCI_ID deviceId);
#elif (CISST_OS == CISST_LINUX_XENOMAI) || (CISST_OS == CISST_LINUX)
    mtsJR3ForceSensor(const std::string & name, 
           const std::string & devicePath, 
           int subdeviceId);
#endif
    ~mtsJR3ForceSensor();
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsJR3ForceSensor);

#endif  //_mtsJR3ForceSensor_h
